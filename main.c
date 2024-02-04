#include <stdio.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#include "files/files.h"
#include "logging/logging.h"
#include "network/address.h"

// The maximum amount of TCP connections the server can handle
const int TCP_STACK = 5;

// The root directory of the server
char * rootDirectory = NULL;

[[noreturn]] int main(int argc, char** argv) {
    server_log(INFO, "Booting the server...");
    server_log(INFO, "Initializing MIME file types hash table...");

    // Initialize the MIME file types hash table
    int mimeContentTypesInsertedAmount = 0;
    initMimeContentTypes(&mimeContentTypesInsertedAmount);
    server_log(INFO, "MIME file types hash table initialized, %i files types supported", mimeContentTypesInsertedAmount);

    // Check if the server has been launched with the correct amount of arguments
    if(argc != 3){
        server_log(ERROR, "Usage : %s port rootDirectoryPath", argv[0]);
        exit(EXIT_FAILURE);
    }

    ushort port = atoi(argv[1]);
    if(!(1024 < port && port <= 65535)){
        server_log(FATAL, "Server port range is invalid, should be in range ]1024 -> 65535]");
        exit(EXIT_FAILURE);
    }

    DIR * dir = opendir(argv[2]);
    if(dir){
        rootDirectory = argv[2];
        closedir(dir);
    } else if(errno == ENOENT){
        server_log(FATAL, "Content directory does not exist !");
    } else if(errno == EACCES) {
        server_log(FATAL, "Specified content folder isn't accessible due to permission configuration.");
    } else {
        server_log(FATAL, "Server content folder isn't accessible for some reason.");
    }

    server_log(INFO, "Server launched on port %i and root directory %s", port, rootDirectory);

    // Create the virtual socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        server_log(FATAL, "Error during creating socket : %s", argv[0], strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        server_log(INFO, "Created socket for server with file descriptor %i", sock);
    }

    // Prepare the server address
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // Internet address with port "port" converted as network value and IP chosen by kernel
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind the server address to the socket
    int bind_returned_value = bind(sock, (const struct sockaddr *) &server, sizeof(server));

    // Check if the binding was successful
    if(bind_returned_value < 0){
        server_log(FATAL, "Could not bind server address [port:%i] to socket %i due to error : %s", port, sock, strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        char * addr = addressToString(server);
        server_log(INFO, "Successfully bound server address [addr: %s w/ port %i] to socket %i", addr, ntohs(server.sin_port), sock);
        if(addr != NULL) free(addr);
    }

    // Make the socket listen to incoming connections with a maximum of TCP_STACK connections in the stack
    int listen_returned_value = listen(sock, TCP_STACK);

    // Check if the socket is now in listen mode
    if(listen_returned_value != 0){
        server_log(FATAL, "Could not make the socket listen due to error : %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        server_log(INFO, "Successfully made the socket go in listen mode");
    }

    while(true){

        // Prepare the client address structure
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int dialog_socket;

        // Accept the connection from the client socket
        dialog_socket = accept(sock, (struct sockaddr *) &client, &len);

        // Placing the connection treatment in a child process of the main server process to avoid blocking the server during the treatment
        if(fork() == 0){

            // Check if the connection was successful
            if(dialog_socket < 0){
                server_log(ERROR, "An error occurred while accepting the connection : %s", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // Convert the client address to a human-readable string for logging purposes
            char* client_addr = addressToString(client);

            // Prepare request reading buffer for 1024 bytes chunks reading
            ushort buff_len = 1024;
            char buff_read[buff_len];

            // Allocate memory for the request
            char * request = NULL;

            // Read the request from the client
            ssize_t bytes_received = 0;
            while(request == NULL || bytes_received > 0) {

                // Read bytes from the connection
                bytes_received = recv(dialog_socket, buff_read, buff_len, 0);

                // Check if an error occurred while reading bytes
                if(bytes_received < 0) {
                    server_log(ERROR, "An error occurred while reading bytes from connection : %s", strerror(errno));
                    exit(EXIT_FAILURE);
                } else if(bytes_received > 0){

                    // Allocate memory for the request if it's the first chunk, else expand the allocated memory chunk
                    if(request == NULL) {

                        // Allocate memory for the request and copy the first chunk of bytes
                        request = calloc(bytes_received + 1, sizeof(char));
                        strncpy(request, buff_read, bytes_received);
                    } else {

                        // Expand the allocated memory chunk and concatenate the next chunk of bytes
                        request = reallocarray(request, strlen(request) + strlen(buff_read) + 1, sizeof(char));
                        if(request == NULL){
                            printf("Reallocation of buffer at address %p as failed during expanding resources expansion", request);
                            continue;
                        }
                        strcat(request, buff_read);
                    }

                    // Check if the request has been fully read.
                    if(strstr(request, "\r\n\r\n") || strstr(request, "\n\n")){
                        break;
                    }
                }
            }

            // Build the response for the response algorithm
            HTTPResponse httpResponse = buildResponse(rootDirectory, request);

            // Convert the response to an HTTP valid string
            char * responseStr = responseToStr(httpResponse);

            // Send the response to the client if the response is valid
            if(responseStr != NULL){
                send(dialog_socket, responseStr, strlen(responseStr), 0);
            } else {
                // TODO : NYI
                server_log(ERROR, "Response is NULL ???");
            }

            // Close the connection with the client
            close(dialog_socket);

            // Freeing http response manual memory allocations
            if(httpResponse.date            != NULL) free(httpResponse.date);
            if(httpResponse.expires         != NULL) free(httpResponse.expires);
            if(httpResponse.lastModified    != NULL) free(httpResponse.lastModified);
            if(httpResponse.body            != NULL) free(httpResponse.body);

            // Freeing response related strings manual memory allocations
            if(client_addr != NULL) free(client_addr);
            if(request     != NULL) free(request);
            if(responseStr != NULL) free(responseStr);

            // Log the connection closure
            server_log(INFO, "Server connection with client on socket %i has been closed and resources have been freed", sock);
            server_log(INFO, "Child process [ppid: %lu, pid: %lu] has finished its treatment and is now exiting", (ulong) getppid(), (ulong) getpid());
            // Exit the child process when the connection is closed
            exit(EXIT_SUCCESS);
        }
    }
}
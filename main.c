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

const int TCP_STACK = 5;
char * rootDirectory = NULL;

[[noreturn]] int main(int argc, char** argv) {
    server_log(INFO, "Booting the server...");
    server_log(INFO, "Initializing MIME file types hash table...");

    int mimeContentTypesInsertedAmount = 0;
    initMimeContentTypes(&mimeContentTypesInsertedAmount);
    server_log(INFO, "MIME file types hash table initialized, %i files types supported", mimeContentTypesInsertedAmount);

    struct sockaddr_in server;

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

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == -1){
        server_log(FATAL, "Error during creating socket : %s", argv[0], strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        server_log(INFO, "Created socket for server with file descriptor %i", sock);
    }

    // Internet address with port "port" converted as network value and IP chosen by kernel
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int bind_rtrn = bind(sock, (const struct sockaddr *) &server, sizeof(server));

    if(bind_rtrn < 0){
        server_log(FATAL, "Could not bind server address [port:%i] to socket %i due to error : %s", port, sock, strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        char * addr = addressToString(server);
        server_log(INFO, "Successfully bound server address [addr: %s w/ port %i] to socket %i", addr, ntohs(server.sin_port), sock);
        if(addr != NULL) free(addr);
    }

    int listen_rtrn = listen(sock, TCP_STACK);

    if(listen_rtrn != 0){
        server_log(FATAL, "Could not make the socket listen due to error : %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        server_log(INFO, "Successfully made the socket go in listen mode");
    }

    int connection_id = 0;

    while(true){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int dialog_socket;

        dialog_socket = accept(sock, (struct sockaddr *) &client, &len);
        char* client_addr = addressToString(client);
        connection_id++;

        ushort buff_len = 1024;
        char buff_read[buff_len];

        char * request = NULL;

        ssize_t bytes_received = 0;
        while(request == NULL || bytes_received > 0) {
            bytes_received = recv(dialog_socket, buff_read, buff_len, 0);
            if(bytes_received < 0) {
                server_log(ERROR, "An error occurred while reading bytes from connection : %s", strerror(errno));
                exit(EXIT_FAILURE);
            } else if(bytes_received > 0){
                if(request == NULL) {
                    request = calloc(bytes_received + 1, sizeof(char));
                    strncpy(request, buff_read, bytes_received);
                } else {
                    request = reallocarray(request, strlen(request) + strlen(buff_read) + 1, sizeof(char));
                    if(request == NULL){
                        printf("Reallocation of buffer at address %p as failed during expanding resources expansion", request);
                    }
                    strcat(request, buff_read);
                }
                if(strstr(request, "\r\n\r\n") || strstr(request, "\n\n")){
                    break;
                }
            }
        }

        HTTPResponse httpResponse = buildResponse(rootDirectory, request);
        char * responseStr = responseToStr(httpResponse);

        if(responseStr != NULL){
            send(dialog_socket, responseStr, strlen(responseStr), 0);
        } else {
            // TODO : NYI
            server_log(ERROR, "Response is NULL ???");
        }

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

        server_log(INFO, "Server connection %i with client on socket %i has been closed and resources have been freed", connection_id, sock);
    }
}
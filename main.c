#include <stdio.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "requests/requests.h"
#include "logging/logging.h"
#include "network/address.h"
#include "files/files.h"

const int TCP_STACK = 5;

void startServer(int argc, char** argv);

int main(int argc, char** argv) {

    HTTPRequest req = {0};

    req.command.type = GET;
    req.command.path = "/";
    req.command.version = HTTP1;

    char * req2 = "GET / HTTP/1.1\r\n"
                  "Host: 127.0.0.1:8080\r\n"
                  "Connection: keep-alive\r\n"
                  "Cache-Control: max-age=0\r\n"
                  "sec-ch-ua: \"Not_A Brand\";v=\"8\", \"Chromium\";v=\"120\", \"Google Chrome\";v=\"120\"\r\n"
                  "sec-ch-ua-mobile: ?0\r\n"
                  "sec-ch-ua-platform: \"Linux\"\r\n"
                  "DNT: 1\r\n"
                  "Upgrade-Insecure-Requests: 1\r\n"
                  "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
                  "Sec-Fetch-Site: none\r\n"
                  "Sec-Fetch-Mode: navigate\r\n"
                  "Sec-Fetch-User: ?1\r\n"
                  "Sec-Fetch-Dest: document\r\n"
                  "Accept-Encoding: gzip, deflate, br\r\n"
                  "Accept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
                  "\r\n"
                  "Request body is here :3\r\n"
                  "\r\n"
                  "\r\n";

    char * req3 = "GET / HTTP/1.1\r\n"
                  "Host: 127.0.0.1:8080\r\n"
                  "Connection: keep-alive\r\n"
                  "Cache-Control: max-age=0\r\n"
                  "sec-ch-ua: \"Not_A Brand\";v=\"8\", \"Chromium\";v=\"120\", \"Google Chrome\";v=\"120\"\r\n"
                  "sec-ch-ua-mobile: ?0\r\n"
                  "sec-ch-ua-platform: \"Linux\"\r\n"
                  "DNT: 1\r\n"
                  "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
                  "Sec-Fetch-Site: none\r\n"
                  "Sec-Fetch-Mode: navigate\r\n"
                  "Sec-Fetch-User: ?1\r\n"
                  "Sec-Fetch-Dest: document\r\n"
                  "Accept-Encoding: gzip, deflate, br\r\n"
                  "Accept-Language: fr-FR,fr;q=0.9,en-US;q=0.8,en;q=0.7\r\n\r\n";

    // char * header = "Host: 127.0.0.1:8080";

    // server_log(INFO, "Header %s parsed as %s", header, headerToStr(headerFromStr(header)));

    requestFromStr(req2);

    // startServer(argc, argv);

    /*
    int fd = openFile("/home/altaks/Documents/dev/c/HTTPServer/", "README.md");
    char *text = NULL;
    ssize_t text_length;
    readFile(fd, &text, &text_length);
    closeFile(fd);
    server_log(INFO, "Read content of size %i from file : %s", text_length, text);
     */

    return 0;
}

[[noreturn]] void startServer(int argc, char** argv){
    server_log(INFO, "Booting the server...");

    struct sockaddr_in server;

    if(argc != 3){
        server_log(ERROR, "Usage : %s id port", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* id = argv[1];
    short port = atoi(argv[2]);

    server_log(INFO, "Server launched with id %s and port %i", id, port);

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
                    request = malloc(sizeof(char) * bytes_received);
                    strcpy(request, buff_read);
                } else {
                    request = realloc(request, strlen(request) + strlen(buff_read) + 1);
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

        if(strlen(request) > 0) {
            char* request_cpy = malloc(strlen(request) + 1);
            strcpy(request_cpy, request);

            char* req_head = strtok(request_cpy, "\r\n");

            HTTPCommand command = commandFromStr(req_head);
            free(request_cpy);

            server_log(INFO, "Request command parsed as [method: %s, path: %s, version: %s] from [address: %s, port: %i]",
                       requestTypeToStr(command.type),
                       command.path,
                       httpVersionToStr(command.version),
                       client_addr,
                       ntohs(client.sin_port));
        }

        close(dialog_socket);

        if(client_addr != NULL) free(client_addr);
        if(request     != NULL) free(request);

        server_log(INFO, "Server connection %i with client on socket %i has been closed and resources have been freed", connection_id, sock);
    }
}
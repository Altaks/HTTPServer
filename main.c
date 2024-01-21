#include <stdio.h>

#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "logging/logging.h"

const int TCP_STACK = 5;

[[noreturn]] int main(int argc, char** argv) {
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

        char addr[INET6_ADDRSTRLEN > INET_ADDRSTRLEN ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN];

        switch (((struct sockaddr *) &server)->sa_family) {
            case AF_INET: {
                inet_ntop(AF_INET, &(server.sin_addr), addr, INET_ADDRSTRLEN);
            }
            case AF_INET6: {
                inet_ntop(AF_INET6, &((struct sockaddr_in6 *) &server)->sin6_addr, addr, INET6_ADDRSTRLEN);
            }
        }
        server_log(INFO, "Successfully bound server address [addr: %s w/ port %i] to socket %i", addr, ntohs(server.sin_port), sock);
    }

    int listen_rtrn = listen(sock, TCP_STACK);

    if(listen_rtrn != 0){
        server_log(FATAL, "Could not make the socket listen due to error : %s", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        server_log(INFO, "Successfully made the socket go in listen mode");
    }

    while(true){
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int dialog_socket;

        dialog_socket = accept(sock, (struct sockaddr *) &client, &len);

        char buff_read[1024];
        int buff_len = 1024;

        ssize_t bytes_received = recv(dialog_socket, buff_read, buff_len, 0);
        while(bytes_received > 0){
            printf("%s", buff_read);
            bytes_received = recv(dialog_socket, buff_read, buff_len, 0);
            if(bytes_received < 0){
                server_log(ERROR, "An error occurred while reading bytes from connection : %s", strerror(errno));
            }
        }

        close(dialog_socket);

    }
    /*
    server_log(INFO, "Closing server socket with file descriptor %i", sock);
    close(sock);
    server_log(INFO, "Server socket with file descriptor %i closed", sock);
    */

    return 0;
}

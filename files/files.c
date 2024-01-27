#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "files.h"
#include "../logging/logging.h"

bool endsWithSlash(char* path){
    return path[strlen(path) - 1] == '/';
}

int openFile(char* rootDirectory, char* filePath) {

    // reserve enough memory
    u_int16_t length = strlen(rootDirectory) + strlen(filePath) + (endsWithSlash(rootDirectory) ? 1 : 2);
    char * fullPath = calloc(length, sizeof(char));

    // Construct path using values and add a slash if necessary
    strcat(fullPath, rootDirectory);
    if(!endsWithSlash(rootDirectory)) {
        strcat(fullPath, "/");
    }
    strcat(fullPath, filePath);

    // open the file using the sys_open syscall
    server_log(INFO, "Opening file %s : [dir: %s |file: %s]", fullPath, rootDirectory, filePath);
    int fd = open(fullPath, O_RDONLY);

    // free the used memory and return the FD
    free(fullPath);
    return fd;
}

void readFile(int fd, char** text_ptr, ssize_t * text_length_ptr) {

    *text_ptr = NULL;
    u_short buffer_size = 1024;
    char buffer[buffer_size];

    ssize_t bytes_read = 0;
    while(*text_ptr == NULL || bytes_read >= buffer_size) {

        // clear buffer between reads (just in case)
        memset(&buffer, 0, buffer_size);
        bytes_read = read(fd, buffer, buffer_size);

        if(bytes_read != 0){
            if(*text_ptr == NULL){
                *text_ptr = calloc(bytes_read + 1, sizeof(char));
                strncpy(*text_ptr, buffer, bytes_read);
                *text_length_ptr = bytes_read;
            } else {
                *text_ptr = reallocarray(*text_ptr, strlen(*text_ptr) + bytes_read + 1, sizeof(char));
                strncat(*text_ptr, buffer, bytes_read);
                *text_length_ptr += bytes_read;
            }
        }
    }
}

void closeFile(int fd) {
    server_log(INFO, "Closed file w/ fd: %i", fd);
    close(fd);
}
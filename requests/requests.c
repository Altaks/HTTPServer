#include <string.h>
#include <stdlib.h>
#include "requests.h"
#include "../logging/logging.h"

char* requestTypeToStr(RequestType requestType) {
    switch (requestType) {
        case GET:
            return "GET";
        case HEAD:
            return "HEAD";
        case POST:
            return "POST";
        case PUT:
            return "PUT";
        case DELETE:
            return "DELETE";
        case CONNECT:
            return "CONNECT";
        case OPTIONS:
            return "OPTIONS";
        case TRACE:
            return "TRACE";
        case PATCH:
            return "PATCH";
        default:
            return "Unknown request type";
    }
}

RequestType requestTypeFromStr(char* str){
    for(int i = 0; i < RequestType_REG_MAX; i++){
        if (strcmp(str, requestTypeToStr(i)) == 0) {
            return (RequestType)(i);
        }
    }
    return -1;
}

char* httpVersionToStr(HTTPVersion version){
    switch (version) {
        case HTTP0_9:
            return "HTTP/0.9";
        case HTTP1:
            return "HTTP/1.0";
        case HTTP1_1:
            return "HTTP/1.1";
        default:
            return "Unknown HTTP version";
    }
}

HTTPVersion httpVersionFromStr(char* str){
    for(int i = 0; i < HTTPVersion_REG_MAX; i++){
        if(strcmp(str, httpVersionToStr(i)) == 0){
            return i;
        }
    }
    return -1;
}

HTTPRequest requestFromStr(char* str) {

    // Creating empty request data
    HTTPRequest request = {0};

    server_log(INFO, "Obtained request header of length : %i", strlen(str));

    // Cloning request line to another string in order to use strtok safely (avoid segfaults due to array overflows)
    char * phrase = malloc(strlen(str) > 256 ? strlen(str) : 256);
    strcpy(phrase, str);

    // Start the string parkour and keeping track of the token index.
    char * token = strtok(phrase, " ");
    int8_t token_index = 0;

    // As long as there are tokens :
    while(token != NULL){

        // According to the index of the token, we try to convert it to non-string value and save it in the data struct.
        switch (token_index) {
            case 0:
                // Method of the request
                request.type = requestTypeFromStr(token);
                break;
            case 1:
                // Path of the request
                request.path = token;
                break;
            case 2:
                // HTTP version of the request
                request.version = httpVersionFromStr(token);
                break;
            default:
                continue;
        }

        // Go to the next part of the splitting and increment index.
        token = strtok(NULL, " ");
        token_index++;
    }

    server_log(INFO,
               "Request parsed as [method: %s, path: %s, version: %s]",
               requestTypeToStr(request.type),
               request.path,
               httpVersionToStr(request.version)
    );

    free(phrase);

    return request;
}
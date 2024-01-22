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

HTTPVersion requestTypeFromStr(char* str){
    for(int i = 0; i < RequestType_REG_MAX; i++){
        if (strcmp(str, requestTypeToStr(i)) == 0) {
            return (HTTPVersion)(i);
        }
    }
    return -1;
}

char* httpVersionToStr(HTTPVersion version){
    switch (version) {
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

HTTPRequest requestFromStr(char* str){
    server_log(INFO, "Parsing request from string : %s", str);

    HTTPRequest request = {0};

    char * phrase = malloc(strlen(str) + 1);
    strcpy(phrase, str);

    char * token = strtok(phrase, " ");
    int8_t token_index = 0;
    while(token != NULL){
        switch (token_index) {
            case 0:
                // Method of the request
                request.version = requestTypeFromStr(token);
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
        token = strtok(NULL, " ");
        token_index++;
    }

    server_log(INFO,
               "Request parsed as [method: %s, path: %s, version: %s]",
               requestTypeToStr(request.type), request.path,
               httpVersionToStr(request.version)
    );

    free(phrase);
    free(token);

    return request;
}
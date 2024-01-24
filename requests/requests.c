#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

HTTPCommand commandFromStr(char* str) {

    // Creating empty request data
    HTTPCommand request = {0};

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
                request.path = malloc(sizeof(char) * strlen(token));
                strcpy(request.path, token);
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

    free(phrase);
    return request;
}


char* headerToStr(HTTPHeader header) {
    char * str = malloc(sizeof(char) * (strlen(header.key) + strlen(header.value) + 4));
    sprintf(str, "%s: %s", header.key, header.value);
    return str;
}


HTTPHeader headerFromStr(char* str) {
    HTTPHeader header = {0};

    char * phrase = malloc(strlen(str));
    strcpy(phrase, str);

    char * token = strtok(phrase, ": ");
    int8_t token_index = 0;

    while(token != NULL){
        switch (token_index) {
            case 0:
                header.key = malloc(sizeof(char) * strlen(token));
                strcpy(header.key, token);
                break;
            case 1:
                header.value = malloc(sizeof(char) * strlen(token));
                strcpy(header.value, token);
                break;
            default:
                continue;
        }

        token = strtok(NULL, ": ");
        token_index++;
    }

    free(phrase);
    return header;
}


char* requestToStr(HTTPRequest request) {
    char * req = malloc(
            sizeof(char) * (
                strlen(requestTypeToStr(request.command.type)) +
                strlen(request.command.path) +
                strlen(httpVersionToStr(request.command.version)) +
                2 // for two spaces
           )
    );

    strcat(req, requestTypeToStr(request.command.type));
    strcat(req, " ");
    strcat(req, request.command.path);
    strcat(req, " ");
    strcat(req, httpVersionToStr(request.command.version));

    return req;
}

/*
    // char * req2 = "GET /page.html HTTP/1.0\r\n"
                 "Host: example.com\r\n"
                 "Referer: http://example.com/\r\n"
                 "User-Agent: CERN-LineMode/2.15 libwww/2.17b3";
 */

HTTPRequest requestFromStr(char* str) {
    char * req_cpy = malloc(strlen(str) + 1);
    strcpy(req_cpy, str);
    char * curr_req_pos = req_cpy;

    char * delimiter = (strstr(str, "\r\n\r\n") != NULL) ? "\r\n\r\n" : "\n\n";

    server_log(INFO, "Request curr ptr is at %p", curr_req_pos);

    long req_head_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
    char * req_head = malloc(sizeof(char) * req_head_size + 1);
    strncpy(req_head, curr_req_pos, req_head_size);
    curr_req_pos += (sizeof(char) * req_head_size) + strlen(delimiter);

    server_log(INFO, "Request header has been determined as %s", req_head);

    // If request has a body
    if(strstr(curr_req_pos, delimiter)){
        long req_body_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
        char * req_body = malloc(sizeof(char) * req_head_size  + 1);
        strncpy(req_body, curr_req_pos, req_body_size);
        curr_req_pos += (sizeof(char) * req_body_size) + strlen(delimiter);

        server_log(INFO, "Request body has been determined as %s", req_body);
    }

    free(req_cpy);
}
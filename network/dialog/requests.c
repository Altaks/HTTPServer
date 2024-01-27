#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "requests.h"
#include "../../logging/logging.h"

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
                request.path = malloc(sizeof(char) * strlen(token) + 1);
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

    char * phrase = calloc(strlen(str) + 1, sizeof(char));
    strcpy(phrase, str);

    const char * delimiter = ": ";
    char * phrase_pos = phrase;

    long prefix_len = strstr(phrase, delimiter) - phrase;
    char * prefix = calloc( prefix_len + 1, sizeof(char));
    strncpy(prefix, phrase, prefix_len);
    phrase_pos += prefix_len + strlen(delimiter);

    header.key = prefix;

    long content_len = strlen(str) - prefix_len - strlen(delimiter);
    char * content = calloc( content_len + 1, sizeof(char));
    strncpy(content, phrase_pos, content_len);

    header.value = content;

    free(phrase);
    return header;
}


char* requestToStr(HTTPRequest request) {
    return "Not implemented yet";
}

HTTPRequest requestFromStr(char* str) {
    char * req_cpy = malloc(strlen(str) + 1);
    strcpy(req_cpy, str);
    char * curr_req_pos = req_cpy;

    const char * delimiter = (strstr(str, "\r\n\r\n") != NULL) ? "\r\n\r\n" : "\n\n";

    long req_head_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
    char * req_head = malloc(sizeof(char) * req_head_size + 1);
    strncpy(req_head, curr_req_pos, req_head_size);
    curr_req_pos += (sizeof(char) * req_head_size) + strlen(delimiter);

    server_log(INFO, "Request header has been determined as %s", req_head);

    char * req_body = NULL;

    // If request has a body
    if(strstr(curr_req_pos, delimiter)){
        long req_body_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
        req_body = malloc(sizeof(char) * req_head_size + 1);
        strncpy(req_body, curr_req_pos, req_body_size);
        curr_req_pos += (sizeof(char) * req_body_size) + strlen(delimiter);

        server_log(INFO, "Request body has been determined as %s", req_body);
    }

    free(req_cpy);

    const char * line_delimiter = (strstr(req_head, "\r\n") != NULL) ? "\r\n" : "\n";

    // -------------------- REQUEST PROCESSING BEGIN -----------------------

    HTTPRequest request = {0};

    // -------------------- REQ_COMMAND TREATMENT --------------------

    char * req_head_pos = req_head;

    long req_command_size = strstr(req_head_pos, line_delimiter) - req_head_pos;
    char * req_command = malloc(sizeof(char) * req_command_size + 1);
    strncpy(req_command, req_head_pos, req_command_size);

    // Move ptr after the first line
    req_head_pos += req_command_size + strlen(line_delimiter);

    server_log(INFO, "Request command determined as %s", req_command);

    request.command = commandFromStr(req_command);
    free(req_command);

    // -------------------- REQ_HEAD TREATMENT --------------------

    HTTPHeader * headers = NULL;
    int headers_count = 0;
    int headers_allocated = 0;

    while(strstr(req_head_pos, line_delimiter) != NULL) {
        long req_header_size = strstr(req_head_pos, line_delimiter) - req_head_pos;
        char* req_header = malloc(sizeof(char) * req_header_size + 1);
        memset(req_header, '\0', req_header_size + 1);
        strncpy(req_header, req_head_pos, req_header_size);
        req_head_pos += req_header_size + strlen(line_delimiter);

        if(headers == NULL){
            headers = malloc(sizeof(HTTPHeader) * 5);
            headers_allocated = 5;
        } else if(headers_allocated >= headers_count) {
            headers = realloc(headers, sizeof(HTTPHeader) * (headers_count + 5));
            headers_allocated += 5;
        }

        server_log(INFO, "Found header to parse : %s", req_header);
        headers[headers_count++] = headerFromStr(req_header);
        free(req_header);
    }

    for(int i = 0; i < headers_count; i++){
        server_log(INFO, "%i'th header : %s", i, headerToStr(headers[i]));
    }

    request.headers = headers;
    request.headers_count = headers_count;

    // -------------------- REQ_BODY TREATMENT --------------------

    if(req_body != NULL){
        request.body = req_body;
        server_log(INFO, "Request body is %s", request.body);
    }

    // -------------------- MEMORY CLEANING --------------------

    free(req_head);
}
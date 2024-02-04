#include <string.h>
#include <stdlib.h>
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
        case UNKNOWN:
            return "UNKNOWN";
        default:
            return "Invalid request type";
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

    // Creating empty command data
    HTTPCommand command = {0};

    // Copying command to avoid modifying the original string
    char * command_cpy = calloc(strlen(str) + 1, sizeof(char)); // Removing \r\n from the end of the string
    strncpy(command_cpy, str, strlen(str));

    // Splitting the command using tokenization
    char * token = strtok(command_cpy, " ");
    int8_t token_index = 0;

    // As long as there are tokens :
    while(token != NULL){

        // According to the index of the token, we try to convert it to non-string value and save it in the data struct.
        switch (token_index) {
            case 0:
                // Method of the request
                command.type = requestTypeFromStr(token);
                break;
            case 1:
                // Path of the request
                command.path = calloc(strlen(token) + 1, sizeof(char));

                // If the path is just a slash, we consider it as a request to the index.html file.
                if(strcmp("/", token) == 0){
                    strcpy(command.path, "/index.html");
                } else {
                    strcpy(command.path, token);
                }
                break;
            case 2:
                // HTTP version of the request
                command.version = httpVersionFromStr(token);
                break;
            default:
                continue;
        }

        // Go to the next part of the splitting and increment index.
        token = strtok(NULL, " ");
        token_index++;
    }

    free(command_cpy);
    return command;
}

HTTPHeader headerFromStr(char* str) {

    if(strlen(str) == 0) {
        return (HTTPHeader) {0};
    }

    // Creating empty header data
    HTTPHeader header = {0};

    // Copying header to avoid modifying the original string
    char * phrase = calloc(strlen(str) + 1, sizeof(char));
    strcpy(phrase, str);

    // Splitting the header using tokenization
    const char * delimiter = ": ";
    char * phrase_pos = phrase;

    // Getting the prefix of the header
    ulong prefix_len = strstr(phrase, delimiter) - phrase;
    char * prefix = calloc( prefix_len + 1, sizeof(char));
    strncpy(prefix, phrase, prefix_len);
    phrase_pos += prefix_len + strlen(delimiter);

    header.key = prefix;

    // Getting the content of the header
    ulong content_len = strlen(str) - prefix_len - strlen(delimiter);
    char * content = calloc( content_len + 1, sizeof(char));
    strncpy(content, phrase_pos, content_len);

    header.value = content;

    // Freeing the memory allocated for the phrase
    free(phrase);
    return header;
}

HTTPRequest requestFromStr(char* str) {

    // Allocating memory for the request copy
    char * req_cpy = malloc(strlen(str) + 1);
    strcpy(req_cpy, str);
    char * curr_req_pos = req_cpy;

    // Determining the delimiter of the request
    const char * delimiter = (strstr(str, "\r\n\r\n") != NULL) ? "\r\n\r\n" : "\n\n";

    // Determining the request head
    long req_head_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
    char * req_head = malloc(sizeof(char) * req_head_size + 1);
    strncpy(req_head, curr_req_pos, req_head_size);

    // Move ptr after the delimiter (to the next matching part of the string)
    curr_req_pos += (sizeof(char) * req_head_size) + strlen(delimiter);

    char * req_body = NULL;

    // If request has a body, determine it.
    if(strstr(curr_req_pos, delimiter)){
        long req_body_size = strstr(curr_req_pos, delimiter) - curr_req_pos;
        req_body = malloc(sizeof(char) * req_head_size + 1);
        strncpy(req_body, curr_req_pos, req_body_size);
        server_log(INFO, "Request body has been determined as %s", req_body);
    }

    // Freeing the memory allocated for the request copy
    free(req_cpy);

    // Determining the line delimiter inside the request head
    const char * line_delimiter = (strstr(req_head, "\r\n") != NULL) ? "\r\n" : "\n";

    // -------------------- REQUEST PROCESSING BEGIN -----------------------

    // Creating empty request data
    HTTPRequest request = {0};

    // -------------------- REQ_COMMAND TREATMENT --------------------

    // Placing a ptr at the beginning of the request head to move it.
    char * req_head_pos = req_head;

    // Determining the command of the request
    long req_command_size = strstr(req_head_pos, line_delimiter) - req_head_pos;
    char * req_command = calloc(req_command_size + 1, sizeof(char));
    strncpy(req_command, req_head_pos, req_command_size);

    // Move ptr after the first line
    req_head_pos += req_command_size + strlen(line_delimiter);

    // Determining the command of the request
    request.command = commandFromStr(req_command);

    server_log(INFO, "Request command determined as [method: %s, path: %s, version: %s]", requestTypeToStr(request.command.type), request.command.path,
               httpVersionToStr(request.command.version));

    // Freeing the memory allocated for the command string
    free(req_command);

    // -------------------- REQ_HEAD TREATMENT --------------------

    // Creating empty headers data
    HTTPHeader * headers = NULL;
    int headers_count = 0;
    int headers_allocated = 0;

    // As long as there are headers, we determine them
    while(strstr(req_head_pos, line_delimiter) != NULL) {

        // Determining the header and copying it to avoid modifying the original string
        long req_header_size = strstr(req_head_pos, line_delimiter) - req_head_pos;
        char* req_header = calloc( req_header_size + 1, sizeof(char));
        strncpy(req_header, req_head_pos, req_header_size);
        req_head_pos += req_header_size + strlen(line_delimiter);

        // If the headers array is not allocated, we allocate it with a default length of 5.
        // If it is allocated, we reallocate it with a bigger size.
        if(headers == NULL){
            headers = calloc( 5, sizeof(HTTPHeader));
            headers_allocated = 5;
        } else if(headers_allocated >= headers_count) {
            headers = reallocarray(headers, headers_count + 5, sizeof(HTTPHeader));
            headers_allocated += 5;
        }

        // We add the header to the headers array and increment the headers count.
        if(strcmp("", req_header) != 0) headers[headers_count++] = headerFromStr(req_header);

        // Freeing the memory allocated for the header string
        free(req_header);
    }

    // We set the headers and the headers count to the request.
    request.headers = headers;
    request.headers_count = headers_count;

    // -------------------- REQ_BODY TREATMENT --------------------

    // If the request has a body, we set it to the request.
    if(req_body != NULL){
        request.body = req_body;
        server_log(INFO, "Request body is %s", request.body);
    }

    // -------------------- MEMORY CLEANING --------------------

    free(req_head);

    return request;
}
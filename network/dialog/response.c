#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "math.h"
#include "response.h"
#include "../../logging/logging.h"
#include "../../util/date.h"
#include "../../files/files.h"

char* HTTPResponseCodeToStr(HTTPResponseCode code) {
    switch (code) {

        case RESPONSE_INFORMATION_CONTINUE:
            return "Continue";
        case RESPONSE_INFORMATION_SWITCHING_PROTOCOLS:
            return "Switching Protocols";
        case RESPONSE_INFORMATION_PROCESSING:
            return "Processing";
        case RESPONSE_INFORMATION_EARLY_HINTS:
            return "Early Hints";

        case RESPONSE_SUCCESS_OK:
            return "OK";
        case RESPONSE_SUCCESS_CREATED:
            return "Created";
        case RESPONSE_SUCCESS_ACCEPTED:
            return "Accepted";
        case RESPONSE_SUCCESS_NON_AUTHORITATIVE_INFORMATION:
            return "Non-Authoritative Information";
        case RESPONSE_SUCCESS_NO_CONTENT:
            return "No Content";
        case RESPONSE_SUCCESS_RESET_CONTENT:
            return "Reset Content";
        case RESPONSE_SUCCESS_PARTIAL_CONTENT:
            return "Partial Content";
        case RESPONSE_SUCCESS_MULTI_STATUS:
            return "Multi-Status";
        case RESPONSE_SUCCESS_ALREADY_REPORTED:
            return "Already Reported";
        case RESPONSE_SUCCESS_CONTENT_DIFFERENT:
            return "Content Different";
        case RESPONSE_SUCCESS_IM_USED:
            return "IM Used";

        case RESPONSE_REDIRECT_MULTIPLE_CHOICES:
            return "Multiple Choices";
        case RESPONSE_REDIRECT_MOVED_PERMANENTLY:
            return "Moved Permanently";
        case RESPONSE_REDIRECT_FOUND:
            return "Found";
        case RESPONSE_REDIRECT_SEE_OTHER:
            return "See Other";
        case RESPONSE_REDIRECT_NOT_MODIFIED:
            return "Not Modified";
        case RESPONSE_REDIRECT_USE_PROXY:
            return "Use Proxy";
        case RESPONSE_REDIRECT_USE_PROXY_LEGACY:
            return "Use Proxy Legacy";
        case RESPONSE_REDIRECT_TEMPORARY_REDIRECT:
            return "Temporary Redirect";
        case RESPONSE_REDIRECT_PERMANENT_REDIRECT:
            return "Permanent Redirect";
        case RESPONSE_REDIRECT_TOO_MANY_REDIRECTS:
            return "Too Many Redirects";

        case RESPONSE_CLIENT_ERROR_BAD_REQUEST:
            return "Bad Request";
        case RESPONSE_CLIENT_ERROR_UNAUTHORIZED:
            return "Unauthorized";
        case RESPONSE_CLIENT_ERROR_PAYMENT_REQUIRED:
            return "Payment Required";
        case RESPONSE_CLIENT_ERROR_FORBIDDEN:
            return "Forbidden";
        case RESPONSE_CLIENT_ERROR_NOT_FOUND:
            return "Not Found";
        case RESPONSE_CLIENT_ERROR_METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case RESPONSE_CLIENT_ERROR_NOT_ACCEPTABLE:
            return "Not Acceptable";
        case RESPONSE_CLIENT_ERROR_PROXY_AUTH_REQUIRED:
            return "Proxy Authentication Required";
        case RESPONSE_CLIENT_ERROR_REQUEST_TIME_OUT:
            return "Request Time-out";
        case RESPONSE_CLIENT_ERROR_CONFLICT:
            return "Conflict";
        case RESPONSE_CLIENT_ERROR_GONE:
            return "Gone";
        case RESPONSE_CLIENT_ERROR_LENGTH_REQUIRED:
            return "Length Required";
        case RESPONSE_CLIENT_ERROR_PRECONDITION_FAILED:
            return "Precondition Failed";
        case RESPONSE_CLIENT_ERROR_REQUEST_ENTITY_TOO_LARGE:
            return "Request Entity Too Large";
        case RESPONSE_CLIENT_ERROR_REQUEST_URI_TOO_LONG:
            return "Request-URI Too Long";
        case RESPONSE_CLIENT_ERROR_UNSUPPORTED_MEDIA_TYPE:
            return "Unsupported Media Type";
        case RESPONSE_CLIENT_ERROR_REQUEST_RANGE_UNSATISFIABLE:
            return "Request Range Unsatifiable";
        case RESPONSE_CLIENT_ERROR_EXPECTATION_FAILED:
            return "Expectation Failed";
        case RESPONSE_CLIENT_ERROR_IM_A_TEAPOT:
            return "I'm a teapot";
        case RESPONSE_CLIENT_ERROR_PAGE_EXPIRED:
            return "Page Expired";
        case RESPONSE_CLIENT_ERROR_BAD_MAPPING_OR_MISDIRECTED_REQUEST:
            return "Bad Mapping or Misdirected Request";
        case RESPONSE_CLIENT_ERROR_UNPROCESSABLE_ENTITY:
            return "Unprocessable Entity";
        case RESPONSE_CLIENT_ERROR_LOCKED:
            return "Locked";
        case RESPONSE_CLIENT_ERROR_METHOD_FAILURE:
            return "Method Failure";
        case RESPONSE_CLIENT_ERROR_TOO_EARLY:
            return "Too Early";
        case RESPONSE_CLIENT_ERROR_UPGRADE_REQUIRED:
            return "Upgrade Required";
        case RESPONSE_CLIENT_ERROR_INVALID_DIGITAL_SIGNATURE:
            return "Invalid Digital Signature";
        case RESPONSE_CLIENT_ERROR_PRECONDITION_REQUIRED:
            return "Precondition Required";
        case RESPONSE_CLIENT_ERROR_TOO_MANY_REQUESTS:
            return "Too Many Requests";
        case RESPONSE_CLIENT_ERROR_REQUEST_HEADER_FIELDS_TOO_LARGE:
            return "Request Header Fields Too Large";
        case RESPONSE_CLIENT_ERROR_RETRY_WITH:
            return "Retry With";
        case RESPONSE_CLIENT_ERROR_BLOCKED_BY_WINDOWS_PARENTAL_CONTROL:
            return "Blocked by Windows Parental Control";
        case RESPONSE_CLIENT_ERROR_UNAVAILABLE_FOR_LEGAL_REASONS:
            return "Unavailable For Legal Reasons";
        case RESPONSE_CLIENT_ERROR_UNRECOVERABLE_ERROR:
            return "Unrecoverable Error";

        case NGINX_NO_RESPONSE:
            return "No Response";
        case NGINX_SSL_CERTIFICATE_ERROR:
            return "SSL Certificate Error";
        case NGINX_SSL_CERTIFICATE_REQUIRED:
            return "SSL Certificate Required";
        case NGINX_HTTP_REQ_SENT_TO_HTTPS_PORT:
            return "HTTP Request Sent to HTTPS Port";
        case NGINX_TOKEN_EXPIRED_OR_INVALID:
            return "Token Expired or Invalid";
        case NGINX_CLIENT_CLOSED_REQUEST:
            return "Client Closed Request";

        case RESPONSE_SERVER_ERROR_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case RESPONSE_SERVER_ERROR_NOT_IMPLEMENTED:
            return "Not Implemented";
        case RESPONSE_SERVER_ERROR_BAD_GATEWAY_OR_PROXY_ERROR:
            return "Bad Gateway or Proxy Error";
        case RESPONSE_SERVER_ERROR_SERVICE_UNAVAILABLE:
            return "Service Unavailable";
        case RESPONSE_SERVER_ERROR_GATEWAY_TIME_OUT:
            return "Gateway Time-out";
        case RESPONSE_SERVER_ERROR_HTTP_VERSION_NOT_SUPPORTED:
            return "HTTP Version not supported";
        case RESPONSE_SERVER_ERROR_VARIANT_ALSO_NEGOTIATES:
            return "Variant Also Negotiates";
        case RESPONSE_SERVER_ERROR_INSUFFICIENT_STORAGE:
            return "Insufficient Storage";
        case RESPONSE_SERVER_ERROR_LOOP_DETECTED:
            return "Loop Detected";
        case RESPONSE_SERVER_ERROR_BANDWIDTH_LIMIT_EXCEEDED:
            return "Bandwidth Limit Exceeded";
        case RESPONSE_SERVER_ERROR_NOT_EXTENDED:
            return "Not Extended";
        case RESPONSE_SERVER_ERROR_NETWORK_AUTH_REQUIRED:
            return "Network Authentication Required";

        case CLOUDFLARE_UNKNOWN_ERROR:
            return "Unknown Error";
        case CLOUDFLARE_WEB_SERVER_IS_DOWN:
            return "Web Server is Down";
        case CLOUDFLARE_CONNECTION_TIMED_OUT:
            return "Connection Timed Out";
        case CLOUDFLARE_ORIGIN_IS_UNREACHABLE:
            return "Origin is Unreachable";
        case CLOUDFLARE_A_TIMEOUT_OCCURRED:
            return "A Timeout Occurred";
        case CLOUDFLARE_SSL_HANDSHAKE_FAILED:
            return "SSL Handshake Failed";
        case CLOUDFLARE_INVALID_SSL_CERTIFICATE:
            return "Invalid SSL Certificate";
        case CLOUDFLARE_RAILGUN_ERROR:
            return "Railgun Error";

        default:
            return "Unknown Error Code";
    }
}

HTTPResponse buildResponse(char * rootDirectory, char * request){

    if(strlen(request) > 0) {

        // Parse request into HTTP Request

        HTTPRequest convertedRequest = requestFromStr(request);
        HTTPResponse responseToSend = {0};

        // set response date

        responseToSend.date = calloc(38, sizeof(char));
        strncat(responseToSend.date, "Date: ", 6);
        strncat(responseToSend.date, getOffsetDate(0), 30);
        strncat(responseToSend.date, "\r\n", 2);
        printf("%s", responseToSend.date);

        responseToSend.expires = calloc(41, sizeof(char));
        strncat(responseToSend.expires, "Expires: ", 9);
        strncat(responseToSend.expires, getOffsetDate(+3600), 30);
        strncat(responseToSend.expires, "\r\n", 2);
        printf("%s", responseToSend.expires);

        // set response server type
        responseToSend.server = "HTTPServer/0.0.1";

        if(convertedRequest.command.type == UNKNOWN || convertedRequest.command.path == NULL || convertedRequest.command.version > HTTP1_1) {
            responseToSend.code = RESPONSE_CLIENT_ERROR_BAD_REQUEST;
            responseToSend.contentType = CONTENT_TYPE_TEXT_PLAIN;
            responseToSend.body = "Bad Request";
            return responseToSend;
        }

        // Analyse HTTP Command
        switch (convertedRequest.command.type) {
            case GET:
                if(strlen(convertedRequest.command.path) <= 0){
                    int fileDescriptor = openFile(rootDirectory, convertedRequest.command.path);

                    if(fileDescriptor == -1){
                        server_log(FATAL, "An error has occured while opening file : %s", strerror(errno));
                        responseToSend.code = RESPONSE_CLIENT_ERROR_NOT_FOUND;
                        responseToSend.body = "Content not found";
                        responseToSend.contentType = CONTENT_TYPE_TEXT_PLAIN;
                        return responseToSend;
                    } else {
                        // Collect file content
                        readFile(fileDescriptor, &responseToSend.body, &responseToSend.contentLength);
                        closeFile(fileDescriptor);
                        // Detect file last modified
                        responseToSend.lastModified = getLastModifiedTime(rootDirectory, convertedRequest.command.path);
                        // Detect content type
                        responseToSend.contentType = detectMimeContentTypes(convertedRequest.command.path);
                        responseToSend.code = RESPONSE_SUCCESS_OK;
                        return responseToSend;
                    }
                } else {
                    responseToSend.code = RESPONSE_CLIENT_ERROR_BAD_REQUEST;
                    responseToSend.contentType = CONTENT_TYPE_TEXT_PLAIN;
                    responseToSend.body = "Bad Request";
                    return responseToSend;
                }
                break;
            default:
                responseToSend.code = RESPONSE_SERVER_ERROR_NOT_IMPLEMENTED;
                responseToSend.contentType = CONTENT_TYPE_TEXT_PLAIN;
                responseToSend.body = "Method isn't supported yet";
                return responseToSend;
        }

        // return response
        return responseToSend;
    }

}

char* responseToStr(HTTPResponse response) {
    char * responseStr = calloc(
            strlen(httpVersionToStr(HTTP1_1)) + // length of HTTP version
            1 + // space
            3 + // length of response code
            1 + // space
            strlen(HTTPResponseCodeToStr(response.code)) + // length of response code text
            2 +  // for the "\r\n"
            1 /* for the terminating zero */,sizeof(char));
    sprintf(responseStr, "%s %i %s\r\n", httpVersionToStr(HTTP1_1), response.code, HTTPResponseCodeToStr(response.code));

    if(response.date != NULL){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Date : ") + strlen(response.date) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Date: ");
        strcat(responseStr, response.date);
        strcat(responseStr, "\r\n");
    }

    if(response.server != NULL){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Server: ") + strlen(response.server) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Server: ");
        strcat(responseStr, response.server);
        strcat(responseStr, "\r\n");
    }

    if(response.contentType != UNKNOWN){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Content-Type: ") + strlen(contentTypeToString(response.contentType)) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Content-Type: ");
        strcat(responseStr, contentTypeToString(response.contentType));
        strcat(responseStr, "\r\n");
    }

    if(response.contentLength != 0){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Content-Length: ") + ((int)log10(response.contentLength) + 1) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Content-Length: ");

        char buffer[(int) log10(response.contentLength) + 1];
        sprintf(buffer, "%i", (int)response.contentLength);
        strcat(responseStr, buffer);
        strcat(responseStr, "\r\n");
    }

    if(response.expires != NULL){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Expires: ") + strlen(response.expires) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Expires: ");
        strcat(responseStr, response.expires);
        strcat(responseStr, "\r\n");
    }

    if(response.expires != NULL){
        responseStr = reallocarray(responseStr, strlen(responseStr) + strlen("Last-Modified: ") + strlen(response.lastModified) + strlen("\r\n") + 1,sizeof(char));
        strcat(responseStr, "Last-Modified: ");
        strcat(responseStr, response.lastModified);
        strcat(responseStr, "\r\n");
    }

    ulong bodyLength = response.body != NULL ? strlen(response.body) + strlen("\r\n"): 0;
    responseStr = reallocarray(responseStr, strlen(responseStr) + bodyLength + (strlen("\r\n") * 3) + 1,sizeof(char));
    strcat(responseStr, "\r\n");

    if(response.body != NULL){
        strcat(responseStr, response.body);
        strcat(responseStr, "\r\n");
    }

    strcat(responseStr, "\r\n\r\n");

    return responseStr;
}

#ifndef REQUESTS_H_
#define REQUESTS_H_

/**
 * Represents a HTTP request type
 */
typedef enum RequestType RequestType;
enum RequestType {
    UNKNOWN,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,

    RequestType_REG_MAX // This is not a request type, it is used to know the maximum amount of request types
};

/**
 * Represents a HTTP version
 */
typedef enum HTTPVersion HTTPVersion;
enum HTTPVersion {
    HTTP0_9,
    HTTP1,
    HTTP1_1,

    HTTPVersion_REG_MAX // This is not an HTTP version, it is used to know the maximum amount of HTTP versions
};

/**
 *  Represents a HTTP request
 */
typedef struct HTTPCommand HTTPCommand;
struct HTTPCommand {
    RequestType type;
    char * path;
    HTTPVersion version;
};

/**
 * Represents a HTTP header
 */
typedef struct HTTPHeader HTTPHeader;
struct HTTPHeader {
    __attribute__((unused)) char * key;
    __attribute__((unused)) char * value;
};

/**
 * Represents a HTTP request
 */
typedef struct HTTPRequest HTTPRequest;
struct HTTPRequest {
    HTTPCommand command;

    HTTPHeader * headers;
    __attribute__((unused)) int headers_count;

    char * body;
};

/**
 * Converts a request type to a string
 * @param requestType the request type to convert
 * @return the string corresponding to the request type
 */
char* requestTypeToStr(RequestType requestType);

/**
 * Converts a string to a request type
 * @param str the string to convert
 * @return the request type corresponding to the string
 */
RequestType requestTypeFromStr(char* str);

/**
 * Converts a HTTP version to a string
 * @param version the HTTP version to convert
 * @return the string corresponding to the HTTP version
 */
char* httpVersionToStr(HTTPVersion version);

/**
 * Converts a string to a HTTP version
 * @param str the string to convert
 * @return the HTTP version corresponding to the string
 */
HTTPVersion httpVersionFromStr(char* str);

/**
 * Converts a string to a HTTP command
 * @param str the string to convert
 * @return the HTTP command corresponding to the string
 */
HTTPCommand commandFromStr(char* str);

/**
 * Converts a string to a HTTP header
 * @param str the string to convert
 * @return the HTTP header corresponding to the string
 */
HTTPHeader headerFromStr(char* str);
/**
 * Converts a string to a HTTP request
 * @param str the string to convert
 * @return the HTTP request corresponding to the string
 */
HTTPRequest requestFromStr(char* str);

#endif
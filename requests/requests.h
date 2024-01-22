/**
 * Represents a HTTP request type
 */
typedef enum RequestType RequestType;
enum RequestType {
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
    HTTP1,
    HTTP1_1,

    HTTPVersion_REG_MAX // This is not an HTTP version, it is used to know the maximum amount of HTTP versions
};

/**
 *  Represents a HTTP request
 */
typedef struct HTTPRequest HTTPRequest;
struct HTTPRequest {
    RequestType type;
    char * path;
    HTTPVersion version;
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
HTTPVersion requestTypeFromStr(char* str);

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
 * Converts a string to a HTTP request
 * @param str the string to convert
 * @return the HTTP request corresponding to the string
 */
HTTPRequest requestFromStr(char* str);
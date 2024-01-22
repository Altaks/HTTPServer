#include "requests.h"

enum RequestType {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

enum HTTPVersion {
    HTTP1,
    HTTP1_1
};

typedef enum RequestType RequestType;
typedef enum HTTPVersion HTTPVersion;

struct HTTPRequest {
    RequestType type;
    char * path;
    HTTPVersion version;
};
typedef enum LogLevel LogLevel;

enum LogLevel {
    FATAL, ERROR, WARNING, INFO
};

void server_log(LogLevel, const char*, ...);
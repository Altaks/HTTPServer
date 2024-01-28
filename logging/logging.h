#ifndef LOGGING_H
#define LOGGING_H

typedef enum LogLevel LogLevel;

enum LogLevel {
    FATAL, ERROR, WARNING, INFO
};

void server_log(LogLevel, const char*, ...);

#endif
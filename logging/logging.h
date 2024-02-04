#ifndef LOGGING_H
#define LOGGING_H

/**
 * Represents a log level
 */
typedef enum LogLevel LogLevel;
enum LogLevel {
    FATAL, ERROR, WARNING, INFO
};

/**
 * Logs the message same as printf but adds time and log level to the beginning of the message
 * @param ... printf arguments
 */
void server_log(LogLevel, const char *, ...);

#endif
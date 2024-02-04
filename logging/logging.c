#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "logging.h"

/**
 * Logs the message same as printf but adds time and log level to the beginning of the message
 * @param logLevel determines the importance of the message
 * @param message corresponds to the message than is displayed
 * @param ... printf arguments
 */
void server_log(LogLevel logLevel, const char * restrict message, ...) {

    // First get the current time
    time_t now = time(NULL);
    struct tm * time = localtime(&now);

    // Then get the log level string
    char logLevelBuffer[16];
    switch (logLevel) {
        case FATAL:
            sprintf(logLevelBuffer, "\x1b[1;160m[FATAL]");
            break;
        case ERROR:
            sprintf(logLevelBuffer, "\x1b[0;210m[ERROR]");
            break;
        case WARNING:
            sprintf(logLevelBuffer, "\x1b[0;215m[WARN] ");
            break;
        case INFO:
            sprintf(logLevelBuffer, "\x1b[0;252m[INFO] ");
            break;
    }

    // Now get function arguments from stack input
    va_list args;
    va_start(args, message);

    switch (logLevel) {
        case FATAL:
        case ERROR:
            // Print to stderr if the log level is FATAL or ERROR
            fprintf(stderr, "[%02d/%02d/%d %02d:%02d:%02d] %s | ", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec, logLevelBuffer);
            vfprintf(stderr, message, args);
            fprintf(stderr, "\x1b[0m\n");
            break;
        default:
            // Print to stdout if the log level is WARNING or INFO
            printf("[%02d/%02d/%d %02d:%02d:%02d] %s | ", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec, logLevelBuffer);
            vprintf(message, args);
            printf("\x1b[0m\n");
            break;
    }

    // Clean up the stack
    va_end(args);
}
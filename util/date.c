#include <time.h>
#include <stdlib.h>

/**
 * Returns the current date in the format "Day, DD Month YYYY HH:MM:SS GMT" as HTTP valid date time
 * @param offset the offset to add to the current date
 * @return the current date with the given offset as a human readable string
 */
char* getOffsetDate(int offset){
    // Allocate memory for the date string
    char * buffer = calloc(30, sizeof(char));

    // Get the current date and time
    time_t raw_time = time(NULL) + offset;

    // Convert the date and time to a human-readable string
    struct tm * time_info;
    time_info = localtime (&raw_time);

    // Format the date and time as GMT
    timegm(time_info);

    // Write the date and time to the buffer
    strftime (buffer, 30, "%a, %d %b %Y %H:%M:%S %Z", time_info);
    return buffer;
}
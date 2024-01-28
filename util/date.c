#include <time.h>
#include <stdlib.h>

char* getOffsetDate(int offset){
    char * buffer = calloc(30, sizeof(char));
    time_t rawtime = time(NULL) + offset;
    struct tm * timeinfo;
    timeinfo = localtime (&rawtime);
    timegm(timeinfo);
    strftime (buffer,30,"%a, %d %b %Y %H:%M:%S %Z",timeinfo);
    return buffer;
}
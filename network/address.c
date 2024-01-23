#include <stdlib.h>
#include "address.h"

char* addressToString(struct sockaddr_in address){
    char * addr = malloc(sizeof(char) * (INET6_ADDRSTRLEN > INET_ADDRSTRLEN ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN) + 1);
    switch (((struct sockaddr *) &address)->sa_family) {
        case AF_INET: {
            inet_ntop(AF_INET, &(address.sin_addr), addr, INET_ADDRSTRLEN);
        }
        case AF_INET6: {
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *) &address)->sin6_addr, addr, INET6_ADDRSTRLEN);
        }
    }
    return addr;
}
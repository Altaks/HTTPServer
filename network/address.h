#include <arpa/inet.h>

/**
 * Converts a sockaddr_in to a human readable string
 * @param address the address to convert
 * @return the string corresponding to the address
 */
char* addressToString(struct sockaddr_in address);
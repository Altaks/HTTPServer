#include <stdlib.h>
#include "../network/dialog/response.h"

/**
 * Opens a file in the filesystem and gives back the file descriptor
 * @param rootDirectory the directory to search the file in
 * @param filePath the relative path of the file
 * @return the file descriptor
 */
int openFile(char* rootDirectory, char* filePath);

/**
 * Reads the full content of a files and allocates memory according to this, returns values by pointers passed through it.
 * @param fd the file descriptor to use
 * @param text_ptr the pointer to store the text
 * @param text_length_ptr the pointer to store the length of the text in bytes
 */
void readFile(int fd, char** text_ptr, ssize_t * text_length_ptr);

/**
 * Closes the file
 * @param fd the file descriptor of the file/stream to close.
 */
inline void closeFile(int fd);

/**
 * Detects the last modified timestamp of the file and converts it to textual date time
 * @param directory the root directory to search the file in
 * @param path the path of the file
 * @return the date as a HTTP valid format
 */
char* getLastModifiedTime(char* directory, char* path);

/**
 * Detects the content type of the searched file
 * @param directory the root directory to search the file in
 * @param path the path of the file
 * @return the content type of the file
 */
MIMEContentType detectMIMEContentType(char* directory, char* path);
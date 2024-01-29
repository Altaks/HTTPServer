#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "files.h"
#include "../logging/logging.h"
#include "sys/stat.h"
#include "../util/date/hashmap.h"

bool endsWithSlash(char* path){
    return path[strlen(path) - 1] == '/';
}

int openFile(char* rootDirectory, char* filePath) {

    // reserve enough memory
    u_int16_t length = strlen(rootDirectory) + strlen(filePath) + (endsWithSlash(rootDirectory) ? 1 : 2);
    char * fullPath = calloc(length, sizeof(char));

    // Construct path using values and add a slash if necessary
    strcat(fullPath, rootDirectory);
    if(!endsWithSlash(rootDirectory)) {
        strcat(fullPath, "/");
    }
    strcat(fullPath, filePath);

    // open the file using the sys_open syscall
    server_log(INFO, "Opening file %s : [dir: %s |file: %s]", fullPath, rootDirectory, filePath);
    int fd = open(fullPath, O_RDONLY);

    // free the used memory and return the FD
    free(fullPath);
    return fd;
}

void readFile(int fd, char** text_ptr, ssize_t * text_length_ptr) {

    *text_ptr = NULL;
    u_short buffer_size = 1024;
    char buffer[buffer_size];

    ssize_t bytes_read = 0;
    while(*text_ptr == NULL || bytes_read >= buffer_size) {

        // clear buffer between reads (just in case)
        memset(&buffer, 0, buffer_size);
        bytes_read = read(fd, buffer, buffer_size);

        if(bytes_read != 0){
            if(*text_ptr == NULL){
                *text_ptr = calloc(bytes_read + 1, sizeof(char));
                strncpy(*text_ptr, buffer, bytes_read);
                *text_length_ptr = bytes_read;
            } else {
                *text_ptr = reallocarray(*text_ptr, strlen(*text_ptr) + bytes_read + 1, sizeof(char));
                strncat(*text_ptr, buffer, bytes_read);
                *text_length_ptr += bytes_read;
            }
        }
    }
}


char* getLastModifiedTime(char* rootDirectory, char* path) {
    u_int16_t length = strlen(rootDirectory) + strlen(path) + (endsWithSlash(rootDirectory) ? 1 : 2);
    char * fullPath = calloc(length, sizeof(char));

    // Construct path using values and add a slash if necessary
    strcat(fullPath, rootDirectory);
    if(!endsWithSlash(rootDirectory)) {
        strcat(fullPath, "/");
    }
    strcat(fullPath, path);


    struct stat attrib;
    stat(fullPath, &attrib);
    char * date = calloc(30, sizeof(char));
    strftime(date, 30, "%a, %d %b %Y %H:%M:%S %Z", gmtime(&(attrib.st_mtim.tv_sec)));
    return date;
}

void closeFile(int fd) {
    server_log(INFO, "Closed file w/ fd: %i", fd);
    close(fd);
}


char* contentTypeToString(MIMEContentType type) {
    switch(type) {
        case CONTENT_TYPE_UNKNOWN:
            return "application/octet-stream";
        case CONTENT_TYPE_TEXT_PLAIN:
            return "text/plain";
        case CONTENT_TYPE_TEXT_HTML:
            return "text/html";
        case CONTENT_TYPE_TEXT_CSS:
            return "text/css";
        case CONTENT_TYPE_TEXT_CSV:
            return "text/csv";
        case CONTENT_TYPE_TEXT_CALENDAR:
            return "text/calendar";
        case CONTENT_TYPE_FONT_OTF:
            return "font/otf";
        case CONTENT_TYPE_FONT_TTF:
            return "font/ttf";
        case CONTENT_TYPE_FONT_WOFF:
            return "font/woff";
        case CONTENT_TYPE_FONT_WOFF2:
            return "font/woff2";
        case CONTENT_TYPE_IMAGE_BMP:
            return "image/bmp";
        case CONTENT_TYPE_IMAGE_GIF:
            return "image/gif";
        case CONTENT_TYPE_IMAGE_X_ICON:
            return "image/x-icon";
        case CONTENT_TYPE_IMAGE_JPEG:
            return "image/jpeg";
        case CONTENT_TYPE_IMAGE_PNG:
            return "image/png";
        case CONTENT_TYPE_IMAGE_SVG_XML:
            return "image/svg+xml";
        case CONTENT_TYPE_IMAGE_TIFF:
            return "image/tiff";
        case CONTENT_TYPE_IMAGE_WEBP:
            return "image/webp";
        case CONTENT_TYPE_AUDIO_AAC:
            return "audio/aac";
        case CONTENT_TYPE_AUDIO_MIDI:
            return "audio/midi";
        case CONTENT_TYPE_AUDIO_OGG:
            return "audio/ogg";
        case CONTENT_TYPE_AUDIO_X_WAV:
            return "audio/x-wav";
        case CONTENT_TYPE_AUDIO_WEBM:
            return "audio/webm";
        case CONTENT_TYPE_VIDEO_X_MSVIDEO:
            return "video/x-msvideo";
        case CONTENT_TYPE_VIDEO_MPEG:
            return "video/mpeg";
        case CONTENT_TYPE_VIDEO_OGG:
            return "video/ogg";
        case CONTENT_TYPE_VIDEO_WEBM:
            return "video/webm";
        case CONTENT_TYPE_VIDEO_3GPP:
            return "video/3gpp";
        case CONTENT_TYPE_VIDEO_3GPP2:
            return "video/3gpp2";
        case CONTENT_TYPE_APPLICATION_ABIWORD:
            return "application/abiword";
        case CONTENT_TYPE_APPLICATION_OCTET_STREAM:
            return "application/octet-stream";
        case CONTENT_TYPE_APPLICATION_VND_AMAZON_EBOOK:
            return "application/vnd.amazon.ebook";
        case CONTENT_TYPE_APPLICATION_X_BZIP:
            return "application/x-bzip";
        case CONTENT_TYPE_APPLICATION_X_BZIP2:
            return "application/x-bzip2";
        case CONTENT_TYPE_APPLICATION_X_CSH:
            return "application/x-csh";
        case CONTENT_TYPE_APPLICATION_MSWORD:
            return "application/msword";
        case CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT:
            return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        case CONTENT_TYPE_APPLICATION_VND_MS_FONT_OBJECT:
            return "application/vnd.ms-fontobject";
        case CONTENT_TYPE_APPLICATION_EPUB_ZIP:
            return "application/epub+zip";
        case CONTENT_TYPE_APPLICATION_JAVA_ARCHIVE:
            return "application/java-archive";
        case CONTENT_TYPE_APPLICATION_JAVASCRIPT:
            return "application/javascript";
        case CONTENT_TYPE_APPLICATION_TYPESCRIPT:
            return "application/typescript";
        case CONTENT_TYPE_APPLICATION_JSON:
            return "application/json";
        case CONTENT_TYPE_APPLICATION_VND_APPLE_INSTALLER_XML:
            return "application/vnd.apple.installer+xml";
        case CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION:
            return "application/vnd.oasis.opendocument.presentation";
        case CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET:
            return "application/vnd.oasis.opendocument.spreadsheet";
        case CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT:
            return "application/vnd.oasis.opendocument.text";
        case CONTENT_TYPE_APPLICATION_OGG:
            return "application/ogg";
        case CONTENT_TYPE_APPLICATION_PDF:
            return "application/pdf";
        case CONTENT_TYPE_APPLICATION_VND_MS_POWERPOINT:
            return "application/vnd.ms-powerpoint";
        case CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION:
            return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        case CONTENT_TYPE_APPLICATION_X_RAR_COMPRESSED:
            return "application/x-rar-compressed";
        case CONTENT_TYPE_APPLICATION_RTF:
            return "application/rtf";
        case CONTENT_TYPE_APPLICATION_X_SH:
            return "application/x-sh";
        case CONTENT_TYPE_APPLICATION_X_SHOCKWAVE_FLASH:
            return "application/x-shockwave-flash";
        case CONTENT_TYPE_APPLICATION_X_TAR:
            return "application/x-tar";
        case CONTENT_TYPE_APPLICATION_XHTML_XML:
            return "application/xhtml+xml";
        case CONTENT_TYPE_APPLICATION_VND_VISIO:
            return "application/vnd.visio";
        case CONTENT_TYPE_APPLICATION_VND_MS_EXCEL:
            return "application/vnd.ms-excel";
        case CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET:
            return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        case CONTENT_TYPE_APPLICATION_XML:
            return "application/xml";
        case CONTENT_TYPE_APPLICATION_VND_MOZILLA_XUL_XML:
            return "application/vnd.mozilla.xul+xml";
        case CONTENT_TYPE_APPLICATION_ZIP:
            return "application/zip";
        case CONTENT_TYPE_APPLICATION_X_7Z_COMPRESSED:
            return "application/x-7z-compressed";
        default:
            return "application/octet-stream";
    }
}

struct hashmap * mimeContentTypes = NULL;

void initMimeContentTypes() {
    if(mimeContentTypes == NULL){
        mimeContentTypes = new_map(CONTENT_TYPE_MAX_LENGTH);

        //       map                    extension          content type
        map_set(mimeContentTypes, "unknown", CONTENT_TYPE_UNKNOWN);

        map_set(mimeContentTypes, "txt", CONTENT_TYPE_TEXT_PLAIN);
        map_set(mimeContentTypes, "aac", CONTENT_TYPE_AUDIO_AAC);
        map_set(mimeContentTypes, "abw", CONTENT_TYPE_APPLICATION_ABIWORD);
        map_set(mimeContentTypes, "arc", CONTENT_TYPE_APPLICATION_OCTET_STREAM);
        map_set(mimeContentTypes, "avi", CONTENT_TYPE_VIDEO_X_MSVIDEO);
        map_set(mimeContentTypes, "azw", CONTENT_TYPE_APPLICATION_VND_AMAZON_EBOOK);
        map_set(mimeContentTypes, "bin", CONTENT_TYPE_APPLICATION_OCTET_STREAM);
        map_set(mimeContentTypes, "bmp", CONTENT_TYPE_IMAGE_BMP);
        map_set(mimeContentTypes, "bz", CONTENT_TYPE_APPLICATION_X_BZIP);
        map_set(mimeContentTypes, "bz2", CONTENT_TYPE_APPLICATION_X_BZIP2);
        map_set(mimeContentTypes, "csh", CONTENT_TYPE_APPLICATION_X_CSH);
        map_set(mimeContentTypes, "css", CONTENT_TYPE_TEXT_CSS);
        map_set(mimeContentTypes, "csv", CONTENT_TYPE_TEXT_CSV);
        map_set(mimeContentTypes, "doc", CONTENT_TYPE_APPLICATION_MSWORD);
        map_set(mimeContentTypes, "docx", CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT);
        map_set(mimeContentTypes, "eot", CONTENT_TYPE_APPLICATION_VND_MS_FONT_OBJECT);
        map_set(mimeContentTypes, "epub", CONTENT_TYPE_APPLICATION_EPUB_ZIP);
        map_set(mimeContentTypes, "gif", CONTENT_TYPE_IMAGE_GIF);
        map_set(mimeContentTypes, "htm", CONTENT_TYPE_TEXT_HTML);
        map_set(mimeContentTypes, "html", CONTENT_TYPE_TEXT_HTML);
        map_set(mimeContentTypes, "ico", CONTENT_TYPE_IMAGE_X_ICON);
        map_set(mimeContentTypes, "ics", CONTENT_TYPE_TEXT_CALENDAR);
        map_set(mimeContentTypes, "jar", CONTENT_TYPE_APPLICATION_JAVA_ARCHIVE);
        map_set(mimeContentTypes, "jpeg", CONTENT_TYPE_IMAGE_JPEG);
        map_set(mimeContentTypes, "jpg", CONTENT_TYPE_IMAGE_JPEG);
        map_set(mimeContentTypes, "js", CONTENT_TYPE_APPLICATION_JAVASCRIPT);
        map_set(mimeContentTypes, "json", CONTENT_TYPE_APPLICATION_JSON);
        map_set(mimeContentTypes, "mid", CONTENT_TYPE_AUDIO_MIDI);
        map_set(mimeContentTypes, "midi", CONTENT_TYPE_AUDIO_MIDI);
        map_set(mimeContentTypes, "mpeg", CONTENT_TYPE_VIDEO_MPEG);
        map_set(mimeContentTypes, "mpkg", CONTENT_TYPE_APPLICATION_VND_APPLE_INSTALLER_XML);
        map_set(mimeContentTypes, "odp", CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION);
        map_set(mimeContentTypes, "ods", CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET);
        map_set(mimeContentTypes, "odt", CONTENT_TYPE_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT);
        map_set(mimeContentTypes, "oga", CONTENT_TYPE_AUDIO_OGG);
        map_set(mimeContentTypes, "ogv", CONTENT_TYPE_VIDEO_OGG);
        map_set(mimeContentTypes, "ogx", CONTENT_TYPE_APPLICATION_OGG);
        map_set(mimeContentTypes, "otf", CONTENT_TYPE_FONT_OTF);
        map_set(mimeContentTypes, "png", CONTENT_TYPE_IMAGE_PNG);
        map_set(mimeContentTypes, "pdf", CONTENT_TYPE_APPLICATION_PDF);
        map_set(mimeContentTypes, "ppt", CONTENT_TYPE_APPLICATION_VND_MS_POWERPOINT);
        map_set(mimeContentTypes, "pptx", CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION);
        map_set(mimeContentTypes, "rar", CONTENT_TYPE_APPLICATION_X_RAR_COMPRESSED);
        map_set(mimeContentTypes, "rtf", CONTENT_TYPE_APPLICATION_RTF);
        map_set(mimeContentTypes, "sh", CONTENT_TYPE_APPLICATION_X_SH);
        map_set(mimeContentTypes, "svg", CONTENT_TYPE_IMAGE_SVG_XML);
        map_set(mimeContentTypes, "swf", CONTENT_TYPE_APPLICATION_X_SHOCKWAVE_FLASH);
        map_set(mimeContentTypes, "tar", CONTENT_TYPE_APPLICATION_X_TAR);
        map_set(mimeContentTypes, "tif", CONTENT_TYPE_IMAGE_TIFF);
        map_set(mimeContentTypes, "tiff", CONTENT_TYPE_IMAGE_TIFF);
        map_set(mimeContentTypes, "ts", CONTENT_TYPE_APPLICATION_TYPESCRIPT);
        map_set(mimeContentTypes, "ttf", CONTENT_TYPE_FONT_TTF);
        map_set(mimeContentTypes, "vsd", CONTENT_TYPE_APPLICATION_VND_VISIO);
        map_set(mimeContentTypes, "wav", CONTENT_TYPE_AUDIO_X_WAV);
        map_set(mimeContentTypes, "weba", CONTENT_TYPE_AUDIO_WEBM);
        map_set(mimeContentTypes, "webm", CONTENT_TYPE_VIDEO_WEBM);
        map_set(mimeContentTypes, "webp", CONTENT_TYPE_IMAGE_WEBP);
        map_set(mimeContentTypes, "woff", CONTENT_TYPE_FONT_WOFF);
        map_set(mimeContentTypes, "woff2", CONTENT_TYPE_FONT_WOFF2);
        map_set(mimeContentTypes, "xhtml", CONTENT_TYPE_APPLICATION_XHTML_XML);
        map_set(mimeContentTypes, "xls", CONTENT_TYPE_APPLICATION_VND_MS_EXCEL);
        map_set(mimeContentTypes, "xlsx", CONTENT_TYPE_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET);
        map_set(mimeContentTypes, "xml", CONTENT_TYPE_APPLICATION_XML);
        map_set(mimeContentTypes, "xul", CONTENT_TYPE_APPLICATION_VND_MOZILLA_XUL_XML);
        map_set(mimeContentTypes, "zip", CONTENT_TYPE_APPLICATION_ZIP);
        map_set(mimeContentTypes, "3gp", CONTENT_TYPE_VIDEO_3GPP);
        map_set(mimeContentTypes, "3g2", CONTENT_TYPE_VIDEO_3GPP2);
        map_set(mimeContentTypes, "7z", CONTENT_TYPE_APPLICATION_X_7Z_COMPRESSED);
    }
}

MIMEContentType contentTypeFromExtension(char* extension) {
    return (MIMEContentType) map_get(mimeContentTypes, extension);
}

MIMEContentType detectMimeContentTypes(char* filename){
    char* extension = strrchr(filename, '.');
    if(extension == NULL) return CONTENT_TYPE_UNKNOWN;
    extension++;
    return contentTypeFromExtension(extension);
}
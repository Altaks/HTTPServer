cmake_minimum_required(VERSION 3.27)
project(HTTPServer C)

set(CMAKE_C_STANDARD 23)

add_executable(HTTPServer main.c
        logging/logging.c
        logging/logging.h
        network/dialog/requests.h
        network/dialog/requests.c
        network/address.h
        network/address.c
        files/files.h
        files/files.c
        network/dialog/response.c
        util/date.c
        util/date.h
        util/date/hashmap.c
        util/date/hashmap.h
)

cmake_minimum_required(VERSION 3.27)
project(HTTPServer C)

set(CMAKE_C_STANDARD 23)

add_executable(HTTPServer main.c
        logging/logging.c
        logging/logging.h)

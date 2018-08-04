cmake_minimum_required(VERSION 3.10)
project(web-server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -Wextra -O2")
add_executable(web-server server/server.c server/server.h utils.h http_parsing/request_parser.h http_parsing/response_setter.h)
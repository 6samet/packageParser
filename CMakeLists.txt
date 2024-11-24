cmake_minimum_required(VERSION 3.29)
project(packageParser C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        src/main.c
        src/file_reader.c
        src/file_selector.c
        src/packet_parser.c
        src/data_recorder.c
)

add_executable(packageParser ${SOURCE_FILES})

target_compile_options(packageParser PRIVATE -Wall -Wextra -Wpedantic -O2)

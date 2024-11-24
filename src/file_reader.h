#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>

#define BUFFER_SIZE 1024

typedef struct {
    FILE *file;
    unsigned char *buffer;
    size_t buffer_pos;
    size_t buffer_end;
} FileReader;

int file_reader_open(FileReader *reader, const char *filename);

void file_reader_close(FileReader *reader);

int file_reader_read_byte(FileReader *reader, unsigned char *byte);

int file_reader_fill_buffer(FileReader *reader);

#endif

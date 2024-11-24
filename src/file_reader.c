#include "file_reader.h"
#include <stdio.h>
#include <stdlib.h>

int file_reader_open(FileReader *reader, const char *filename) {
    reader->file = fopen(filename, "rb");
    if (!reader->file) {
        fprintf(stderr, "Failed to open the file!\n");
        return -1;
    }
    reader->buffer = (unsigned char *) malloc(BUFFER_SIZE);
    if (!reader->buffer) {
        fprintf(stderr, "Memory allocation failed!\n");
        fclose(reader->file);
        reader->file = NULL;
        return -1;
    }
    reader->buffer_pos = 0;
    reader->buffer_end = 0;
    return 0;
}

void file_reader_close(FileReader *reader) {
    if (reader->buffer) {
        free(reader->buffer);
        reader->buffer = NULL;
    }
    if (reader->file) {
        fclose(reader->file);
        reader->file = NULL;
    }
}

int file_reader_fill_buffer(FileReader *reader) {
    size_t const bytes_read = fread(reader->buffer, 1, BUFFER_SIZE, reader->file);
    if (bytes_read == 0) {
        if (ferror(reader->file)) {
            fprintf(stderr, "File reading error!\n");
            return -1;
        }
        if (feof(reader->file)) {
            return 0;
        }
    }
    reader->buffer_pos = 0;
    reader->buffer_end = bytes_read;
    return 1;
}

int file_reader_read_byte(FileReader *reader, unsigned char *byte) {
    if (reader->buffer_pos >= reader->buffer_end) {
        int const fill_status = file_reader_fill_buffer(reader);
        if (fill_status == -1) {
            return -1;
        } else if (fill_status == 0) {
            return 0;
        }
    }
    if (reader->buffer_pos < reader->buffer_end) {
        *byte = reader->buffer[reader->buffer_pos++];
        return 1;
    } else {
        return 0;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include "file_selector.h"
#include "file_reader.h"
#include "packet_parser.h"
#include "data_recorder.h"

char *select_and_open_file(FileReader *reader) {
    char *filename = select_file();
    if (!filename) {
        fprintf(stderr, "No file selected.\n");
        return NULL;
    }

    printf("Selected file: %s\n", filename);

    if (file_reader_open(reader, filename) != 0) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        free(filename);
        return NULL;
    }

    return filename;
}

int process_file(FileReader *reader, PacketParser *parser) {
    unsigned char byte;
    int status;
    while ((status = file_reader_read_byte(reader, &byte)) == 1) {
        packet_parser_process_byte(parser, byte);
    }
    if (status == -1) {
        fprintf(stderr, "An error occurred while reading from the file.\n");


        return -1;
    }
    return 0;
}

int main() {
    FileReader reader = {0};
    PacketParser parser = {0};
    DataRecorder data_recorder = {0};

    uint8_t const DEBUG_MODE = 0; // For Debugging purposes only (0: OFF, 1: ON)

    data_recorder_init(&data_recorder);

    packet_parser_init(&parser, DEBUG_MODE, &data_recorder);

    char *filename = select_and_open_file(&reader);
    if (!filename) {
        data_recorder_destroy(&data_recorder);
        return -1;
    }

    if (process_file(&reader, &parser) == -1) {
        file_reader_close(&reader);
        free(filename);
        data_recorder_destroy(&data_recorder);
        return -1;
    }

    file_reader_close(&reader);
    free(filename);

    fprintf(stdout, "\nPackage Summary:\n");
    fprintf(stdout, "Type-1 (0x12 Commands): %u Packet\n", parser.type1_count);
    fprintf(stdout, "Type-2 (0x16 Commands): %u Packet\n", parser.type2_count);

    data_recorder_save_to_files(&data_recorder, "type1_data.txt", "type2_data.txt");

    data_recorder_destroy(&data_recorder);

    fprintf(stdout, "\nFloat data has been saved to 'type1_data.txt' and 'type2_data.txt' files.\n");

    fprintf(stdout, "\nPress Enter to exit...");


    getchar();

    return 0;
}

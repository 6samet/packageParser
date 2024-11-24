// packet_parser.h
#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <stdint.h>
#include "data_recorder.h"

#define PACKET_MAX_SIZE 1024

typedef enum {
    PARSER_STATE_IDLE,
    PARSER_STATE_HEADER,
    PARSER_STATE_DATA
} ParserState;

typedef struct {
    uint8_t buffer[PACKET_MAX_SIZE];
    uint16_t index;
    uint16_t expected_length;
    ParserState state;

    uint32_t type1_count; // For command 0x12
    uint32_t type2_count; // For command 0x16

    uint8_t debug_mode;

    DataRecorder *data_recorder;
} PacketParser;

void packet_parser_init(PacketParser *parser, uint8_t debug_mode, DataRecorder *data_recorder);

void packet_parser_process_byte(PacketParser *parser, uint8_t byte);

uint16_t crc16_xmodem(const uint8_t *data, uint16_t length);

float extract_float_be(const uint8_t *data);

#endif // PACKET_PARSER_H

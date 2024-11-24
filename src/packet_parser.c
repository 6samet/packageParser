// packet_parser.c
#include "packet_parser.h"
#include <stdio.h>
#include <string.h>

void packet_parser_init(PacketParser *parser, uint8_t const debug_mode, DataRecorder *data_recorder) {
    memset(parser, 0, sizeof(PacketParser));
    parser->debug_mode = debug_mode;
    parser->data_recorder = data_recorder;
}

void packet_parser_process_byte(PacketParser *parser, const uint8_t byte) {
    switch (parser->state) {
        case PARSER_STATE_IDLE:
            if (byte == 0xE3) {
                parser->buffer[0] = byte;
                parser->index = 1;
                parser->state = PARSER_STATE_HEADER;
                if (parser->debug_mode) {
                    fprintf(stdout, "\nPacket start detected\n");
                }
            }
            break;

        case PARSER_STATE_HEADER:
            if (parser->index < PACKET_MAX_SIZE) {
                parser->buffer[parser->index++] = byte;
                parser->expected_length = byte;
                if (parser->expected_length < 5) {
                    if (parser->debug_mode) {
                        fprintf(stdout, "Invalid packet length: %d. Searching for new packet...\n",
                                parser->expected_length);
                    }
                    parser->state = PARSER_STATE_IDLE;
                    parser->index = 0;
                } else if (parser->expected_length > PACKET_MAX_SIZE) {
                    if (parser->debug_mode) {
                        fprintf(stdout, "Packet length exceeds maximum size: %d. Searching for new packet...\n",
                                parser->expected_length);
                    }
                    parser->state = PARSER_STATE_IDLE;
                    parser->index = 0;
                } else {
                    parser->state = PARSER_STATE_DATA;
                }
            } else {
                if (parser->debug_mode) {
                    fprintf(stdout, "Buffer overflow detected in HEADER state. Resetting parser.\n");
                }
                parser->state = PARSER_STATE_IDLE;
                parser->index = 0;
            }
            break;

        case PARSER_STATE_DATA:
            if (parser->index < PACKET_MAX_SIZE) {
                parser->buffer[parser->index++] = byte;
            } else {
                if (parser->debug_mode) {
                    fprintf(stdout, "Buffer overflow detected in DATA state. Resetting parser.\n");
                }
                parser->state = PARSER_STATE_IDLE;
                parser->index = 0;
                break;
            }

            if (parser->index == parser->expected_length) {
                uint16_t const received_crc = (parser->buffer[parser->index - 2] << 8) | parser->buffer[
                                                  parser->index - 1];
                uint16_t const calculated_crc = crc16_xmodem(parser->buffer, parser->index - 2);

                if (parser->debug_mode) {
                    fprintf(stdout, "\n---------- Packet Received ----------\n");
                    fprintf(stdout, "Header: %02X\n", parser->buffer[0]);
                    fprintf(stdout, "Length: %02X\n", parser->buffer[1]);
                    fprintf(stdout, "Command: %02X\n", parser->buffer[2]);
                    fprintf(stdout, "Payload:");
                    for (int i = 3; i < parser->index - 2; i++) {
                        fprintf(stdout, " %02X", parser->buffer[i]);
                    }
                    fprintf(stdout, "\n");
                    fprintf(stdout, "Received CRC: %04X\n", received_crc);
                    fprintf(stdout, "Calculated CRC: %04X\n", calculated_crc);
                }

                if (received_crc == calculated_crc) {
                    if (parser->debug_mode) {
                        fprintf(stdout, "CRC is valid\n");
                    }

                    if (parser->buffer[2] == 0x12) {
                        parser->type1_count++;

                        if (parser->expected_length >= 3 + 6 && parser->index >= 3 + 6) {
                            uint8_t const *float_bytes = &parser->buffer[3 + 2];
                            // Payload start from byte 3, float data start from byte 5
                            float const value = extract_float_be(float_bytes);

                            data_recorder_record_type1(parser->data_recorder, value);

                            if (parser->debug_mode) {
                                fprintf(stdout, "Float Bytes: ");
                                for (int i = 0; i < 4; i++) {
                                    fprintf(stdout, "%02X ", float_bytes[i]);
                                }
                                fprintf(stdout, "\n");
                                fprintf(stdout, "Float Data: %f\n", value);
                            }
                        } else {
                            if (parser->debug_mode) {
                                fprintf(stdout, "Payload too short to extract float data.\n");
                            }
                        }
                    } else if (parser->buffer[2] == 0x16) {
                        parser->type2_count++;

                        if (parser->expected_length >= 3 + 68 && parser->index >= 3 + 68) {
                            uint8_t const *float_bytes = &parser->buffer[3 + 64];
                            // Payload start from byte 65, float data start from byte 67
                            float const value = extract_float_be(float_bytes);

                            data_recorder_record_type2(parser->data_recorder, value);

                            if (parser->debug_mode) {
                                fprintf(stdout, "Float Bytes: ");

                                for (int i = 0; i < 4; i++) {
                                    fprintf(stdout, "%02X ", float_bytes[i]);
                                }
                                fprintf(stdout, "\n");
                                fprintf(stdout, "Float Data: %f\n", value);
                            }
                        } else {
                            if (parser->debug_mode) {
                                fprintf(stdout, "Payload too short to extract float data.\n");
                            }
                        }
                    }
                } else {
                    if (parser->debug_mode) {
                        fprintf(stdout, "CRC is invalid. Searching for new packet...\n");
                    }
                }

                parser->state = PARSER_STATE_IDLE;
                parser->index = 0;
                parser->expected_length = 0;
                if (parser->debug_mode) {
                    fprintf(stdout, "-------------------------------------\n");
                }
            } else if (parser->index > parser->expected_length) {
                if (parser->debug_mode) {
                    fprintf(stdout, "Received more data than expected. Resetting parser.\n");
                }
                parser->state = PARSER_STATE_IDLE;
                parser->index = 0;
                parser->expected_length = 0;
            }
            break;

        default:
            parser->state = PARSER_STATE_IDLE;
            parser->index = 0;
            break;
    }
}

uint16_t crc16_xmodem(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0x0000;

    while (length--) {
        crc ^= (uint16_t) (*data++) << 8;
        for (uint16_t i = 0; i < 8; i++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021; // Polynomial 0x1021
            else
                crc <<= 1;
        }
    }
    return crc;
}

float extract_float_be(const uint8_t *data) {
    uint32_t i = ((uint32_t) data[0] << 24) |
                 ((uint32_t) data[1] << 16) |
                 ((uint32_t) data[2] << 8) |
                 ((uint32_t) data[3]);
    float f;
    memcpy(&f, &i, sizeof(f));
    return f;
}

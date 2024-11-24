This Packet Parser is designed to process binary data files containing structured packets with the following specifications:

Packet Types Supported:

Type-1 Packets (Command: 0x12): Extracts a 4-byte float starting from the 3rd byte of the payload.
Type-2 Packets (Command: 0x16): Extracts a 4-byte float starting from the 65th byte of the payload.
Error Handling:

Detects and discards faulty packets based on CRC16 (XModem) validation.

#ifndef _TSPACKET_
#define _TSPACKET_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TS_PACKET_LENGTH 188
#define TS_PACKET_HEADER_LENGTH 4
#define CUSTOM_HEADERR_LENGTH 2
#define TS_PACKET_FULL_HEADER_LENGTH (TS_PACKET_HEADER_LENGTH + CUSTOM_HEADERR_LENGTH)
#define TS_PACKET_DATA_LENGTH (TS_PACKET_LENGTH - TS_PACKET_FULL_HEADER_LENGTH)

#define SYNC_BYTE 0x47
#define PUSI 0x40

typedef struct
{
    // Raw header bytes
    unsigned char header_bytes[TS_PACKET_FULL_HEADER_LENGTH];

    // Payload unit start indicator
    bool pusi;
    // Packet Identifier
    unsigned int pid;
    // Sequence number (or continuity counter)
    unsigned char seq;
    // Custom frame length field, refers to the whole frame formed by multiple TS packet
    unsigned int frame_length;
    // Expected number of TS packet to complete the frame
    unsigned char expected_number_of_packets;

    // Payload of this TS packet
    unsigned char data[TS_PACKET_DATA_LENGTH];
    // Length of usefull data in the payload data array
    unsigned char data_length;

} TsPacket;

bool ts_packet_from_buffer(TsPacket *ts_packet, unsigned char *buffer);

// Buffer must be > 30 bytes
void ts_packet_header_string(TsPacket *ts_packet, char *buffer);

int ts_packet_compare(void const *a, void const *b);

// void ts_packet_copy(TsPacket *from, TsPacket *to);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "tspacket.h"
#include "utils.h"

#define MAX_NUMBER_OF_TS_PACKET 8
#define ASMAN_PID 0x11aa
#define THIRD_BYTE_BASE_VALUE 0x10

#define FRAME_LEN 900

static int rx_buffer_index = 0;
static bool reorder_needed = false;

void do_on_ts_reception(TsPacket ts_packet, TsPacket *rx_buffer)
{
    int i = 0;
    char buffer[30] = {0};
    // ts_packet_header_string(&ts_packet, buffer);
    // printf("   do_on_ts_reception - seq: %d - %s\n", ts_packet.seq, buffer);

    // add to rx_buffer
    rx_buffer[rx_buffer_index] = ts_packet;

    // if incorrect seq, set reorder_needed flag
    if (ts_packet.seq != rx_buffer_index)
        reorder_needed = true;

    ts_packet_header_string(&rx_buffer[rx_buffer_index], buffer);
    printf("   do_on_ts_reception - seq: %d - %s\n", rx_buffer[rx_buffer_index].seq, buffer);

    // if rx_buffer is complete
    if (rx_buffer_index + 1 == ts_packet.expected_number_of_packets)
    {
        // reorder if needed
        if (reorder_needed)
        {
            printf("Reordering..\n");
            qsort(rx_buffer, ts_packet.expected_number_of_packets, sizeof(TsPacket), ts_packet_compare);
        }

        // send frame
        printf("Frame can be sent!\n");

        for (i = 0; i < ts_packet.expected_number_of_packets; i++)
        {
            char buffer[30] = {0};
            ts_packet_header_string(&rx_buffer[i], buffer);
            printf("   [Index %d] Seq: %d - %s\n", i, rx_buffer[i].seq, buffer);
        }
    }

    rx_buffer_index++;
}

int main()
{
    const unsigned char input_packet_number = ceil((double)FRAME_LEN / (double)TS_PACKET_DATA_LENGTH);

    unsigned char inputs[input_packet_number][TS_PACKET_LENGTH];
    memset(inputs, 0, input_packet_number * TS_PACKET_LENGTH * sizeof(unsigned char));

    int i = 0, j = 0, seq = 0;
    for (i = 0; i < input_packet_number; i++)
    {
        unsigned char ts_packet_buffer[TS_PACKET_LENGTH] = {0};
        ts_packet_buffer[0] = SYNC_BYTE;
        ts_packet_buffer[1] = (ASMAN_PID >> 8) & 0x1f;
        ts_packet_buffer[2] = ASMAN_PID & 0xFF;
        ts_packet_buffer[3] = THIRD_BYTE_BASE_VALUE + seq;
        ts_packet_buffer[4] = (FRAME_LEN >> 8);
        ts_packet_buffer[5] = FRAME_LEN & 0xFF;

        if (seq == 0)
            ts_packet_buffer[1] += PUSI;

        for (j = 0; j < TS_PACKET_LENGTH; j++)
            inputs[i][j] = ts_packet_buffer[j];

        seq++;
    }

    printf("Generated input with %d ts buffer for a %d bytes frame\n", input_packet_number, FRAME_LEN);

    // Create all TsPacket structs from buffers
    TsPacket input_ts_packets[input_packet_number];
    for (i = 0; i < input_packet_number; i++)
        ts_packet_from_buffer(&input_ts_packets[i], inputs[i]);

    // Shuffle to similate discontinuity
    shuffle(input_ts_packets, input_packet_number);

    for (i = 0; i < input_packet_number; i++)
    {
        char buffer[30] = {0};
        ts_packet_header_string(&input_ts_packets[i], buffer);
        printf("   [Index %d] Seq: %d - %s\n", i, input_ts_packets[i].seq, buffer);
    }

    /** Sort the whole ts packets array **/
    // printf("Sorting input ts packets\n");

    // qsort(input_ts_packets, input_packet_number, sizeof(TsPacket), ts_packet_compare);

    // for (i = 0; i < input_packet_number; i++)
    // {
    //     char buffer[30] = {0};
    //     ts_packet_header_string(&input_ts_packets[i], buffer);
    //     printf("   [Index %d] Seq: %d - %s\n", i, input_ts_packets[i].seq, buffer);
    // }

    TsPacket rx_buffer[MAX_NUMBER_OF_TS_PACKET];

    for (i = 0; i < input_packet_number; i++)
    {
        do_on_ts_reception(input_ts_packets[i], rx_buffer);
        usleep(500000);
    }

    return EXIT_SUCCESS;
}
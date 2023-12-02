#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "tspacket.h"
#include "simulation.h"

#define MAX_NUMBER_OF_TS_PACKET 8

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
    printf("do_on_ts_reception - [pid: 0x%04X] - seq: %d - %s\n", rx_buffer[rx_buffer_index].pid, rx_buffer[rx_buffer_index].seq, buffer);

    // // Some packets are missing
    // if (rx_buffer_index + 1 < ts_packet.expected_number_of_packets)
    // {
    // }

    // if rx_buffer is complete
    if (rx_buffer_index + 1 == ts_packet.expected_number_of_packets)
    {
        // reorder if needed
        if (reorder_needed)
        {
            printf("do_on_ts_reception - [pid: 0x%04X] - Reordering..\n", rx_buffer[rx_buffer_index].pid);
            qsort(rx_buffer, ts_packet.expected_number_of_packets, sizeof(TsPacket), ts_packet_compare);
        }

        // show
        for (i = 0; i < ts_packet.expected_number_of_packets; i++)
        {
            char buffer[30] = {0};
            ts_packet_header_string(&rx_buffer[i], buffer);
            printf("   [pid: 0x%04X] Index: %d - Seq: %d - %s\n", rx_buffer[rx_buffer_index].pid, i, rx_buffer[i].seq, buffer);
        }

        // send frame
        printf("do_on_ts_reception - [pid: 0x%04X] - Frame can be sent!\n", rx_buffer[rx_buffer_index].pid);
        rx_buffer_index = 0;
        reorder_needed = false;
        return;
    }

    rx_buffer_index++;
}

int main()
{
    TsPacket input_ts_packets[255];
    unsigned char input_packet_number = generate_frames(input_ts_packets);

    TsPacket rx_buffer[PID_RANGE][MAX_NUMBER_OF_TS_PACKET];
    int i;
    for (i = 0; i < input_packet_number; i++)
    {
        int frame_count = input_ts_packets[i].pid - ASMAN_PID;
        do_on_ts_reception(input_ts_packets[i], rx_buffer[frame_count]);
        usleep(500000);
    }

    return EXIT_SUCCESS;
}
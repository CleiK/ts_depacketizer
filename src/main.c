#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "tspacket.h"
#include "simulation.h"

#define MAX_NUMBER_OF_TS_PACKET 8
#define MAX_TS_PACKET_BEFORE_FRAME_LOSS 5

typedef enum
{
    Ready,
    Incomplete,
} FrameStatus;

typedef struct
{
    // PID number (PID - ASMAN_PID)
    unsigned char pid_count;
    // Number of TS packets received since the last time we got a packet for this frame
    unsigned char lateness_count;
    // The current number of read that happened for this frame (given the pid)
    unsigned char read_index;
    // Status of the frame for this pid_count
    FrameStatus status;
} FrameInfo;

bool reorder_needed = false;
FrameInfo rx_frame_status[PID_RANGE];

void do_on_ts_reception(TsPacket ts_packet, TsPacket rx_buffer[MAX_NUMBER_OF_TS_PACKET])
{
    int i = 0;
    char buffer[30] = {0};
    unsigned char pid_count = ts_packet.pid - ASMAN_PID;

    // add to rx_buffer
    rx_buffer[ts_packet.seq] = ts_packet;

    // if incorrect seq, set reorder_needed flag
    if (ts_packet.seq != rx_frame_status[pid_count].read_index)
        reorder_needed = true;

    ts_packet_header_string(&rx_buffer[ts_packet.seq], buffer);
    printf("do_on_ts_reception - [pid: 0x%04X] - seq: %d - %s\n", ts_packet.pid, ts_packet.seq, buffer);

    // // Some packets are missing
    // if (rx_buffer_index + 1 < ts_packet.expected_number_of_packets)
    // {
    // }

    // if frame is complete
    if (rx_frame_status[pid_count].read_index + 1 == ts_packet.expected_number_of_packets)
    {
        // reorder if needed
        if (reorder_needed)
        {
            printf("do_on_ts_reception - [pid: 0x%04X] - Reordering..\n", ts_packet.pid);
            qsort(rx_buffer, ts_packet.expected_number_of_packets, sizeof(TsPacket), ts_packet_compare);
        }

        // show
        // for (i = 0; i < ts_packet.expected_number_of_packets; i++)
        // {
        //     char buffer[30] = {0};
        //     ts_packet_header_string(&rx_buffer[i], buffer);
        //     printf("   [pid: 0x%04X] Index: %d - Seq: %d - %s\n", rx_buffer[rx_buffer_index].pid, i, rx_buffer[i].seq, buffer);
        // }

        // send frame
        printf("do_on_ts_reception - [pid: 0x%04X] - Frame can be sent!\n", ts_packet.pid);
        rx_frame_status[pid_count].read_index = 0;
        rx_frame_status[pid_count].lateness_count = 0;
        rx_frame_status[pid_count].status = Ready;

        reorder_needed = false;

        return;
    }

    rx_frame_status[pid_count].read_index++;
    rx_frame_status[pid_count].lateness_count = 0;
    rx_frame_status[pid_count].status = Incomplete;
}

int main()
{
    TsPacket input_ts_packets[255];
    TsPacket rx_buffer[PID_RANGE][MAX_NUMBER_OF_TS_PACKET];

    printf("[Sizes] TsPacket: %ld  Input TS packets: %ld  RX buffer: %ld\n", sizeof(TsPacket), sizeof(input_ts_packets), sizeof(rx_buffer));

    unsigned char input_packet_number = generate_frames(input_ts_packets);
    printf("Total: %d packets\n", input_packet_number);
    int packet_index, pid_index;

    // Initialize rx_frame_status
    for (pid_index = 0; pid_index < PID_RANGE; pid_index++)
    {
        rx_frame_status[pid_index].pid_count = pid_index;
        rx_frame_status[pid_index].lateness_count = 0;
        rx_frame_status[pid_index].status = Ready;
    }

    for (packet_index = 0; packet_index < input_packet_number; packet_index++)
    {
        int frame_count = 0;

        // Skip the packets removed from the frame during the simulation
        if (input_ts_packets[packet_index].pid != 0)
        {
            frame_count = input_ts_packets[packet_index].pid - ASMAN_PID;

            do_on_ts_reception(input_ts_packets[packet_index], rx_buffer[frame_count]);
            usleep(100000);
        }
        else
        {
            printf("Skiped empty packet\n");
        }

        // Iterate on incomplete frames except the current one
        for (pid_index = 0; pid_index < PID_RANGE; pid_index++)
        {
            if (pid_index == frame_count)
            {
                // printf("Skip %d 0x%04X\n", frame_count, input_ts_packets[i].pid);
                continue;
            }

            if (rx_frame_status[pid_index].status == Incomplete)
            {
                // Increment lateness
                rx_frame_status[pid_index].lateness_count++;
                printf("0x%04X increment lateness to %d!\n", rx_frame_status[pid_index].pid_count + ASMAN_PID, rx_frame_status[pid_index].lateness_count);
            }

            if (rx_frame_status[pid_index].lateness_count >= MAX_TS_PACKET_BEFORE_FRAME_LOSS)
            {
                printf("0x%04X frame loss!\n", frame_count + ASMAN_PID);

                // Consider the frame lost
                rx_frame_status[pid_index].lateness_count = 0;
                rx_frame_status[pid_index].status = Ready;
                memset(rx_buffer[frame_count], 0, MAX_NUMBER_OF_TS_PACKET);
            }
        }
    }

    return EXIT_SUCCESS;
}
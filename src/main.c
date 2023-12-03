#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "tspacket.h"
#include "simulation.h"
#include "frame.h"
#include "common.h"

#define MAX_TS_PACKET_BEFORE_FRAME_LOSS 5

FrameInfo rx_frames[PID_RANGE];

void do_on_ts_reception(TsPacket ts_packet)
{
    int i = 0;
    char buffer[30] = {0};
    unsigned char pid_count = ts_packet.pid - BASE_PID;

    FrameInfo *frame = &rx_frames[pid_count];

    // add to rx_buffer
    frame->rx_buffer[ts_packet.seq] = ts_packet;

    // if incorrect seq, set reorder_needed flag
    if (ts_packet.seq != frame->read_index)
        frame->reorder_needed = true;

    if (DEBUG)
    {
        ts_packet_header_string(&frame->rx_buffer[ts_packet.seq], buffer);
        printf("do_on_ts_reception - [pid: 0x%04X] - seq: %d - %s\n", ts_packet.pid, ts_packet.seq, buffer);
    }

    // if frame is complete
    if (frame->read_index + 1 == ts_packet.expected_number_of_packets)
    {
        // reorder if needed
        if (frame->reorder_needed)
        {
            printf("do_on_ts_reception - [pid: 0x%04X] - Reordering..\n", ts_packet.pid);
            qsort(frame->rx_buffer, ts_packet.expected_number_of_packets, sizeof(TsPacket), ts_packet_compare);

            if (DEBUG)
            {
                // Show after reordering
                for (i = 0; i < ts_packet.expected_number_of_packets; i++)
                {
                    char buffer[30] = {0};
                    ts_packet_header_string(&frame->rx_buffer[i], buffer);
                    printf("   [pid: 0x%04X] Index: %d - Seq: %d - %s\n", frame->rx_buffer[i].pid, i, frame->rx_buffer[i].seq, buffer);
                }
            }
        }

        // send frame
        printf("do_on_ts_reception - [pid: 0x%04X] - Frame can be sent!\n", ts_packet.pid);

        frame_info_reset(frame);

        return;
    }

    frame->read_index++;
    frame->lateness_count = 0;
    frame->status = Incomplete;
}

int main()
{
    TsPacket input_ts_packets[255];
    TsPacket rx_buffer[PID_RANGE][MAX_NUMBER_OF_TS_PACKET_PER_FRAME];

    printf("[Sizes] TsPacket: %ld  Input TS packets: %ld  RX buffer: %ld\n", sizeof(TsPacket), sizeof(input_ts_packets), sizeof(rx_buffer));

    unsigned char input_packet_number = generate_frames(input_ts_packets);
    printf("Total: %d packets\n", input_packet_number);
    int packet_index, pid_index;

    // Initialize rx_frames
    for (pid_index = 0; pid_index < PID_RANGE; pid_index++)
        frame_info_init(&rx_frames[pid_index], pid_index, rx_buffer[pid_index]);

    // Simulate the reception of packets
    for (packet_index = 0; packet_index < input_packet_number; packet_index++)
    {
        int frame_count = 0;

        // Skip the packets removed from the frame during the simulation
        if (input_ts_packets[packet_index].pid != 0)
        {
            frame_count = input_ts_packets[packet_index].pid - BASE_PID;

            do_on_ts_reception(input_ts_packets[packet_index]);
            usleep(50000);
        }

        // Iterate on incomplete frames except the current one
        for (pid_index = 0; pid_index < PID_RANGE; pid_index++)
        {
            if (pid_index == frame_count)
            {
                // printf("Skip %d 0x%04X\n", frame_count, input_ts_packets[i].pid);
                continue;
            }

            if (rx_frames[pid_index].status == Incomplete)
            {
                // Increment lateness
                rx_frames[pid_index].lateness_count++;
                // printf("0x%04X increment lateness to %d!\n", rx_frames[pid_index].pid_count + BASE_PID, rx_frames[pid_index].lateness_count);
            }

            if (rx_frames[pid_index].lateness_count >= MAX_TS_PACKET_BEFORE_FRAME_LOSS)
            {
                printf("[pid: 0x%04X] - Frame lost!\n", pid_index + BASE_PID);

                // Consider the frame lost
                frame_info_reset(&rx_frames[pid_index]);
            }
        }
    }

    return EXIT_SUCCESS;
}
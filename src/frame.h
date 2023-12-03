#ifndef _FRAME_
#define _FRAME_

#include "tspacket.h"

#define MAX_NUMBER_OF_TS_PACKET_PER_FRAME 8

typedef enum
{
    Ready,
    Incomplete,
} FrameStatus;

typedef struct
{
    // PID number (PID - BASE_PID)
    unsigned char pid_count;
    // Number of TS packets received since the last time we got a packet for this frame
    unsigned char lateness_count;
    // The current number of read that happened for this frame (given the pid)
    unsigned char read_index;
    // Status of the frame for this pid_count
    FrameStatus status;

    bool reorder_needed;

    TsPacket rx_buffer[MAX_NUMBER_OF_TS_PACKET_PER_FRAME];
} FrameInfo;

void frame_info_init(FrameInfo *frame_info, unsigned char pid_count, TsPacket rx_buffer[MAX_NUMBER_OF_TS_PACKET_PER_FRAME]);
void frame_info_reset(FrameInfo *frame_info);

#endif
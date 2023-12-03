#include "frame.h"

void frame_info_init(FrameInfo *frame_info, unsigned char pid_count, TsPacket rx_buffer[MAX_NUMBER_OF_TS_PACKET_PER_FRAME])
{
    frame_info->pid_count = pid_count;
    frame_info->lateness_count = 0;
    frame_info->read_index = 0;
    frame_info->status = Ready;
    frame_info->reorder_needed = false;
    memcpy(frame_info->rx_buffer, rx_buffer, MAX_NUMBER_OF_TS_PACKET_PER_FRAME);
}

void frame_info_reset(FrameInfo *frame_info)
{
    frame_info->pid_count = 0;
    frame_info->lateness_count = 0;
    frame_info->read_index = 0;
    frame_info->status = Ready;
    frame_info->reorder_needed = false;
    memset(frame_info->rx_buffer, 0, MAX_NUMBER_OF_TS_PACKET_PER_FRAME);
}
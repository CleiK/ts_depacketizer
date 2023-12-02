#include "tspacket.h"

bool ts_packet_from_buffer(TsPacket *ts_packet, unsigned char *buffer)
{
    if (buffer == NULL || ts_packet == NULL)
        return false;

    if (buffer[0] != SYNC_BYTE)
    {
        printf("ts_packet_from_buffer - no sync byte\n");
        return false;
    }

    ts_packet->pusi = (buffer[1] & PUSI) == PUSI;
    ts_packet->pid = (((unsigned int)buffer[1] & 0x1F) << 8) + buffer[2];
    ts_packet->seq = buffer[3] & 0xf;
    ts_packet->frame_length = ((unsigned int)buffer[4] << 8) + buffer[5];
    ts_packet->expected_number_of_packets = ceil((double)ts_packet->frame_length / (double)TS_PACKET_DATA_LENGTH);

    // Last packet has some data bytes + padding bytes that we don't want to count
    if (ts_packet->seq == ts_packet->expected_number_of_packets - 1)
        ts_packet->data_length = TS_PACKET_DATA_LENGTH - ((ts_packet->expected_number_of_packets * TS_PACKET_DATA_LENGTH) - ts_packet->frame_length);
    else
        ts_packet->data_length = TS_PACKET_DATA_LENGTH;

    int i;
    for (i = 0; i < TS_PACKET_FULL_HEADER_LENGTH; i++)
        ts_packet->header_bytes[i] = buffer[i];

    for (i = 0; i < TS_PACKET_DATA_LENGTH; i++)
        ts_packet->data[i] = buffer[TS_PACKET_FULL_HEADER_LENGTH + i];

    return true;
}

void ts_packet_header_string(TsPacket *ts_packet, char *buffer)
{
    if (ts_packet == NULL || buffer == NULL)
        return;
    sprintf(buffer, "0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", ts_packet->header_bytes[0], ts_packet->header_bytes[1], ts_packet->header_bytes[2], ts_packet->header_bytes[3], ts_packet->header_bytes[4], ts_packet->header_bytes[5]);
}

int ts_packet_compare(void const *a, void const *b)
{
    TsPacket *tsp_a = (TsPacket *)a;
    TsPacket *tsp_b = (TsPacket *)b;

    // printf("!! %d - %d = %d\n", tsp_a->seq, tsp_b->seq, tsp_a->seq - tsp_b->seq);

    return tsp_a->seq - tsp_b->seq;
}

// void ts_packet_copy(TsPacket *from, TsPacket *to)
// {
//     if (from == NULL || to == NULL)
//         return;

//     memcpy(to->header_bytes, from->header_bytes, TS_PACKET_FULL_HEADER_LENGTH);
//     to->pusi = from->pusi;
//     to->pid = from->pid;
//     to->seq = from->seq;
//     to->frame_length = from->frame_length;
//     to->expected_number_of_packets = from->expected_number_of_packets;
//     memcpy(to->data, from->data, TS_PACKET_DATA_LENGTH);
//     to->data_length = from->data_length;
// }
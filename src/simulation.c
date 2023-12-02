#include "simulation.h"

unsigned int generate_frames(TsPacket *ts_packets)
{
    unsigned int total_ts_packets = 0;
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled);

    return total_ts_packets;
}

static unsigned char frame_count = 0;

static unsigned char generate_frame(TsPacket *ts_packets, SimulatedFrameStyle style)
{
    const unsigned char number_of_ts_packets = ceil((double)FRAME_LEN / (double)TS_PACKET_DATA_LENGTH);

    unsigned char inputs[number_of_ts_packets][TS_PACKET_LENGTH];
    memset(inputs, 0, number_of_ts_packets * TS_PACKET_LENGTH * sizeof(unsigned char));

    unsigned int pid = ASMAN_PID + frame_count;

    int i = 0, j = 0, seq = 0;
    for (i = 0; i < number_of_ts_packets; i++)
    {
        unsigned char ts_packet_buffer[TS_PACKET_LENGTH] = {0};
        ts_packet_buffer[0] = SYNC_BYTE;
        ts_packet_buffer[1] = (pid >> 8) & 0x1f;
        ts_packet_buffer[2] = pid & 0xFF;
        ts_packet_buffer[3] = THIRD_BYTE_BASE_VALUE + seq;
        ts_packet_buffer[4] = (FRAME_LEN >> 8);
        ts_packet_buffer[5] = FRAME_LEN & 0xFF;

        if (seq == 0)
            ts_packet_buffer[1] += PUSI;

        for (j = 0; j < TS_PACKET_LENGTH; j++)
            inputs[i][j] = ts_packet_buffer[j];

        seq++;
    }

    // Create all TsPacket structs from buffers

    for (i = 0; i < number_of_ts_packets; i++)
        ts_packet_from_buffer(&ts_packets[i], inputs[i]);

    if (style == Shuffled)
        shuffle(ts_packets, number_of_ts_packets);

    printf("generate_frame - [pid: 0x%04X] - Generated a frame with %d ts packets\n", pid, number_of_ts_packets);

    for (i = 0; i < number_of_ts_packets; i++)
    {
        char buffer[30] = {0};
        ts_packet_header_string(&ts_packets[i], buffer);
        printf("   [pid: 0x%04X] Index: %d - Seq: %d - %s\n", pid, i, ts_packets[i].seq, buffer);
    }

    frame_count++;

    return number_of_ts_packets;
}

static void swap(TsPacket *a, TsPacket *b)
{
    TsPacket temp = *a;
    *a = *b;
    *b = temp;
}

static void shuffle(TsPacket arr[], int n)
{
    // Seed for random number generation
    srand(time(NULL));

    // Start from the last element and swap one by one
    for (int i = n - 1; i > 0; i--)
    {
        // Generate a random index between 0 and i
        int j = rand() % (i + 1);

        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}
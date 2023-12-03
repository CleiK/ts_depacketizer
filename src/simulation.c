#include "simulation.h"

unsigned int generate_frames(TsPacket ts_packets[])
{
    unsigned int total_ts_packets = 0;
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Ordered);
    // total_ts_packets += generate_frame(ts_packets + total_ts_packets, IncompleteAtStart);
    // total_ts_packets += generate_frame(ts_packets + total_ts_packets, IncompleteAtMid);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled + IncompleteAtStart);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled + IncompleteAtMid);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled + IncompleteAtEnd);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Shuffled);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Ordered);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Ordered);
    total_ts_packets += generate_frame(ts_packets + total_ts_packets, Ordered);

    return total_ts_packets;
}

static unsigned char frame_count = 0;

static unsigned char generate_frame(TsPacket ts_packets[], SimulatedFrameStyle style)
{
    const unsigned char number_of_ts_packets = ceil((double)FRAME_LEN / (double)TS_PACKET_DATA_LENGTH);

    unsigned char inputs[number_of_ts_packets][TS_PACKET_LENGTH];
    memset(inputs, 0, number_of_ts_packets * TS_PACKET_LENGTH * sizeof(unsigned char));

    unsigned int pid = BASE_PID + frame_count;

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
    unsigned char created_packets_count = 0;
    bool skipped = false;
    for (i = 0; i < number_of_ts_packets; i++)
    {

        if (has_style(style, IncompleteAtStart) && i == 0)
        {
            skipped = true;
            continue;
        }

        if (has_style(style, IncompleteAtMid) && i == number_of_ts_packets / 2)
        {
            skipped = true;
            continue;
        }

        if (has_style(style, IncompleteAtEnd) && i == (number_of_ts_packets - 1))
        {
            skipped = true;
            continue;
        }

        unsigned char index = skipped ? i - 1 : i;

        ts_packet_from_buffer(&ts_packets[index], inputs[i]);
        created_packets_count++;
    }

    if (has_style(style, Shuffled))
        shuffle(ts_packets, created_packets_count);

    char style_string[255] = {0};
    frame_style_string(style, style_string);
    printf("generate_frame - [pid: 0x%04X] - Generated a frame with %d ts packets and style %s\n", pid, created_packets_count, style_string);

    if (DEBUG)
    {
        for (i = 0; i < created_packets_count; i++)
        {
            char buffer[30] = {0};
            ts_packet_header_string(&ts_packets[i], buffer);
            printf("   [pid: 0x%04X] Index: %d - Seq: %d - %s\n", ts_packets[i].pid, i, ts_packets[i].seq, buffer);
        }
    }

    frame_count++;

    return created_packets_count;
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

static bool has_style(SimulatedFrameStyle value, SimulatedFrameStyle flag)
{
    return (value & flag) == flag;
}

static void frame_style_string(SimulatedFrameStyle value, char *buffer)
{
    if (buffer == NULL)
        return;

    strcat(buffer, "[");

    if (has_style(value, Shuffled))
        strcat(buffer, "Shuffled ");
    else
        strcat(buffer, "Ordered ");

    if (has_style(value, IncompleteAtStart))
        strcat(buffer, "IncompleteAtStart ");

    if (has_style(value, IncompleteAtMid))
        strcat(buffer, "IncompleteAtMid ");

    if (has_style(value, IncompleteAtEnd))
        strcat(buffer, "IncompleteAtEnd ");

    buffer[strlen(buffer) - 1] = ']';
    buffer[strlen(buffer)] = '\0';
}
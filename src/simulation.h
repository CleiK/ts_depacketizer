#ifndef _SIMULATION_
#define _SIMULATION_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tspacket.h"

#define ASMAN_PID 0x11aa
#define THIRD_BYTE_BASE_VALUE 0x10
#define FRAME_LEN 900

typedef enum
{
    // No discontinuity error, complete frame
    Ordered,
    // Shuffle to similate discontinuity
    Shuffled,
    // No discontinuity error, missing start packet
    IncompleteAtStart,
    // No discontinuity error, missing mid packet
    IncompleteAtMid,
    // No discontinuity error, missing end packet
    IncompleteAtEnd,
} SimulatedFrameStyle;

unsigned int generate_frames(TsPacket *ts_packets);

static unsigned char generate_frame(TsPacket *ts_packets, SimulatedFrameStyle style);

// Swap two elements in an array
static void swap(TsPacket *a, TsPacket *b);

// Shuffle an array using Fisher-Yates algorithm
static void shuffle(TsPacket arr[], int n);

#endif
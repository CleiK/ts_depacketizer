#ifndef _SIMULATION_
#define _SIMULATION_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "tspacket.h"

#define ASMAN_PID 0x11aa
#define PID_RANGE UCHAR_MAX
#define THIRD_BYTE_BASE_VALUE 0x10
#define FRAME_LEN 900

typedef enum
{
    // No discontinuity error, complete frame
    Ordered = 0b0000,
    // Shuffle to simulate discontinuity
    Shuffled = 0b0001,
    // No discontinuity error, missing start packet
    IncompleteAtStart = 0b0010,
    // No discontinuity error, missing mid packet
    IncompleteAtMid = 0b0100,
    // No discontinuity error, missing end packet
    IncompleteAtEnd = 0b1000,
} SimulatedFrameStyle;

unsigned int generate_frames(TsPacket ts_packets[]);

static unsigned char generate_frame(TsPacket ts_packets[], SimulatedFrameStyle style);

// Swap two elements in an array
static void swap(TsPacket *a, TsPacket *b);

// Shuffle an array using Fisher-Yates algorithm
static void shuffle(TsPacket arr[], int n);

static bool has_style(SimulatedFrameStyle value, SimulatedFrameStyle flag);

static void frame_style_string(SimulatedFrameStyle value, char *buffer);

#endif
#ifndef _UTILS_
#define _UTILS_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tspacket.h"

// Swap two elements in an array
void swap(TsPacket *a, TsPacket *b);

// Shuffle an array using Fisher-Yates algorithm
void shuffle(TsPacket arr[], int n);

#endif
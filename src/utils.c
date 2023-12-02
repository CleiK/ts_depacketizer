#include "utils.h"

void swap(TsPacket *a, TsPacket *b)
{
    TsPacket temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(TsPacket arr[], int n)
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

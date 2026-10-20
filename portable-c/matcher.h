#ifndef MASTERMIND_MATCHER_H
#define MASTERMIND_MATCHER_H

/* Standalone, platform-independent MasterMind rules for 3 slots / 3 colours.
   Values: 1=green, 2=red, 3=blue. Repeated colours are allowed. */
#define MM_SLOTS 3
#define MM_COLOURS 3

typedef struct {
    int exact;  /* right colour in the right slot */
    int approx; /* right colour in a different slot; no double counting */
} MmScore;

/* Return 0 on success, -1 for a null pointer or invalid colour. */
int mm_score(const int secret[MM_SLOTS], const int guess[MM_SLOTS], MmScore *result);

#endif

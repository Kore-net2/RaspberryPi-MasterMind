#include "matcher.h"

int mm_score(const int secret[MM_SLOTS], const int guess[MM_SLOTS], MmScore *result)
{
    int remaining_secret[MM_COLOURS + 1] = {0};
    int remaining_guess[MM_COLOURS + 1] = {0};
    int exact = 0;
    int approx = 0;
    int i;

    if (!secret || !guess || !result)
        return -1;

    /* Validate all inputs before modifying the caller's output. */
    for (i = 0; i < MM_SLOTS; ++i) {
        if (secret[i] < 1 || secret[i] > MM_COLOURS ||
            guess[i] < 1 || guess[i] > MM_COLOURS)
            return -1;
    }

    for (i = 0; i < MM_SLOTS; ++i) {
        if (secret[i] == guess[i]) {
            ++exact;
        } else {
            /* Only non-exact positions can become approximate matches. */
            ++remaining_secret[secret[i]];
            ++remaining_guess[guess[i]];
        }
    }

    for (i = 1; i <= MM_COLOURS; ++i)
        approx += remaining_secret[i] < remaining_guess[i]
            ? remaining_secret[i] : remaining_guess[i];

    result->exact = exact;
    result->approx = approx;
    return 0;
}

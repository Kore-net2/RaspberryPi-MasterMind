#include "matcher.h"
#include <assert.h>
#include <stdio.h>

/* Independent reference implementation: mark exact positions, then greedily
   pair each remaining guess with one remaining unmatched secret slot. */
static MmScore reference_score(const int secret[MM_SLOTS], const int guess[MM_SLOTS])
{
    int used_secret[MM_SLOTS] = {0};
    int used_guess[MM_SLOTS] = {0};
    MmScore score = {0, 0};
    int i, j;
    for (i = 0; i < MM_SLOTS; ++i) {
        if (secret[i] == guess[i]) {
            used_secret[i] = 1;
            used_guess[i] = 1;
            ++score.exact;
        }
    }
    for (i = 0; i < MM_SLOTS; ++i) {
        if (used_guess[i]) continue;
        for (j = 0; j < MM_SLOTS; ++j) {
            if (!used_secret[j] && guess[i] == secret[j]) {
                used_secret[j] = 1;
                ++score.approx;
                break;
            }
        }
    }
    return score;
}

static void decode(int value, int digits[MM_SLOTS])
{
    int i;
    for (i = 0; i < MM_SLOTS; ++i) {
        digits[i] = value % MM_COLOURS + 1;
        value /= MM_COLOURS;
    }
}

int main(void)
{
    int a, b;
    int cases = 0;
    const int s1[3] = {1, 2, 3};
    const int g1[3] = {1, 3, 2};
    const int s2[3] = {1, 1, 2};
    const int g2[3] = {2, 2, 1};
    const int bad[3] = {1, 0, 3};
    MmScore scored = {-1, -1};

    assert(mm_score(s1, g1, &scored) == 0 && scored.exact == 1 && scored.approx == 2);
    assert(mm_score(s2, g2, &scored) == 0 && scored.exact == 0 && scored.approx == 2);
    assert(mm_score(s1, bad, &scored) == -1);
    assert(mm_score(s1, g1, NULL) == -1);

    for (a = 0; a < 27; ++a) {
        for (b = 0; b < 27; ++b) {
            int secret[MM_SLOTS];
            int guess[MM_SLOTS];
            MmScore actual;
            MmScore expected;
            decode(a, secret);
            decode(b, guess);
            assert(mm_score(secret, guess, &actual) == 0);
            expected = reference_score(secret, guess);
            assert(actual.exact == expected.exact);
            assert(actual.approx == expected.approx);
            assert(actual.exact + actual.approx <= MM_SLOTS);
            ++cases;
        }
    }
    printf("PASS: %d exhaustive secret/guess pairs + edge cases.\n", cases);
    return 0;
}

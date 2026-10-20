#include "matcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int parse_code(const char *s, int code[MM_SLOTS])
{
    int i;
    if (!s || strlen(s) != MM_SLOTS)
        return -1;
    for (i = 0; i < MM_SLOTS; ++i) {
        if (s[i] < '1' || s[i] > '3')
            return -1;
        code[i] = s[i] - '0';
    }
    return 0;
}

static void print_help(const char *program)
{
    printf("MasterMind — three colours, three slots\n");
    printf("1=green, 2=red, 3=blue; repeated colours are allowed.\n");
    printf("Usage: %s [--secret 123] [--help]\n", program);
    printf("Enter 3 digits (e.g. 123), or q to quit.\n");
    printf("Exact = correct colour + position. Approx = correct colour, wrong position.\n");
}

int main(int argc, char **argv)
{
    int secret[MM_SLOTS];
    int guess[MM_SLOTS];
    int attempt = 0;
    char line[128];
    int i;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "--secret") == 0) {
        if (parse_code(argv[2], secret) != 0) {
            fprintf(stderr, "Error: secret must be exactly 3 digits from 1 to 3.\n");
            return 2;
        }
        puts("Demo mode: fixed secret is enabled.");
    } else if (argc == 1) {
        srand((unsigned int)time(NULL));
        for (i = 0; i < MM_SLOTS; ++i)
            secret[i] = (rand() % MM_COLOURS) + 1;
    } else {
        print_help(argv[0]);
        return 2;
    }

    print_help(argv[0]);
    for (;;) {
        MmScore score;
        size_t len;
        printf("Attempt %d > ", attempt + 1);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            putchar('\n');
            return 0;
        }
        len = strcspn(line, "\r\n");
        if (len == sizeof(line) - 1 && line[len] != '\n' && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
        }
        line[len] = '\0';
        if (strcmp(line, "q") == 0 || strcmp(line, "Q") == 0) {
            printf("Secret: %d%d%d\n", secret[0], secret[1], secret[2]);
            return 0;
        }
        if (parse_code(line, guess) != 0) {
            puts("Invalid guess. Use exactly three digits, each 1, 2, or 3.");
            continue;
        }
        if (mm_score(secret, guess, &score) != 0) {
            fputs("Unexpected scoring error.\n", stderr);
            return 1;
        }
        ++attempt;
        printf("Exact: %d | Approx: %d\n", score.exact, score.approx);
        if (score.exact == MM_SLOTS) {
            printf("You won in %d attempt%s!\n", attempt, attempt == 1 ? "" : "s");
            return 0;
        }
    }
}

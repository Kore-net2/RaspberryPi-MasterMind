/* MasterMind implementation for F28HS CW2 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

// Prototypes from lcdBinary.c
extern int failure(int fatal, const char *message, ...);
extern void pinMode(uint32_t *gpio, int pin, int mode);
extern void writeLED(uint32_t *gpio, int led, int value);
extern int readButton(uint32_t *gpio, int button);
extern void waitForButton(uint32_t *gpio, int button);
extern void delay(int us);
extern void writeLCD(char *text);

// Prototype from mm-matches.s
extern void matches(int *secret, int *guess, int len, int *exact, int *approx);

#define DEBUG
#define COLS 3
#define SEQL 3
#define LED 26        // Green LED
#define LED2 5        // Red LED
#define BUTTON 19     // Button
#define DELAY 200     // ms
#define TIMEOUT 5000000  // us (5s на ввод одного цвета)
#define COLOR_DELAY 2000000  // us (2s задержка между цветами)

#ifndef TRUE
#  define TRUE  (1==1)
#  define FALSE (1==2)
#endif

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)
#define INPUT  0
#define OUTPUT 1
#define LOW    0
#define HIGH   1

static const int colors = COLS;
static const int seqlen = SEQL;
static char* color_names[] = { "green", "red", "blue" };
static int* theSeq = NULL;
static uint32_t *gpio;
static uint32_t gpiobase = 0xFE200000;

void initSeq() {
    if (theSeq == NULL) theSeq = (int*)malloc(seqlen * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < seqlen; i++) {
        theSeq[i] = (rand() % colors) + 1;
    }
}

void showSeq(int *seq) {
    printf("Seq: %s %s %s\n", color_names[seq[0]-1], color_names[seq[1]-1], color_names[seq[2]-1]);
}

void countMatches(int *seq1, int *seq2, int *exact, int *approx) {
    matches(seq1, seq2, seqlen, exact, approx);
}

void showMatches(int exact, int approx, int *seq1, int *seq2, int lcd_format) {
    printf("%d exact\n%d approximate\n", exact, approx);
    if (lcd_format) {
        char lcdText[16];
        sprintf(lcdText, "%d %d", exact, approx);
        writeLCD(lcdText);
    }
}

void readSeq(int *seq, int val) {
    for (int i = seqlen - 1; i >= 0; i--) {
        seq[i] = val % 10;
        val /= 10;
    }
}

void blinkN(uint32_t *gpio, int led, int c) {
    for (int i = 0; i < c; i++) {
        writeLED(gpio, led, HIGH);
        delay(DELAY * 1000);
        writeLED(gpio, led, LOW);
        delay(DELAY * 1000);
    }
}

int main(int argc, char *argv[]) {
    int verbose = 0, debug = 0, help = 0, opt_s = 0, unit_test = 0;
    int attempts = 0, found = 0;
    int *attSeq;
    int fd;

    int opt;
    while ((opt = getopt(argc, argv, "hvdus:")) != -1) {
        switch (opt) {
        case 'v': verbose = 1; break;
        case 'h': help = 1; break;
        case 'd': debug = 1; break;
        case 'u': unit_test = 1; break;
        case 's': opt_s = atoi(optarg); break;
        default:
            fprintf(stderr, "Usage: %s [-h] [-v] [-d] [-u <seq1> <seq2>] [-s <secret seq>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (help) {
        fprintf(stderr, "MasterMind program for Raspberry Pi\n");
        fprintf(stderr, "Usage: %s [-h] [-v] [-d] [-u <seq1> <seq2>] [-s <secret seq>]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    attSeq = (int*)malloc(seqlen * sizeof(int));
    if (attSeq == NULL) {
        fprintf(stderr, "Failed to allocate memory for attSeq\n");
        exit(EXIT_FAILURE);
    }

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
        return failure(TRUE, "Unable to open /dev/mem: %s\n", strerror(errno));

    gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, gpiobase);
    if ((int32_t)gpio == -1)
        return failure(TRUE, "mmap (GPIO) failed: %s\n", strerror(errno));

    printf("Starting program...\n");
    pinMode(gpio, LED, OUTPUT);
    pinMode(gpio, LED2, OUTPUT);
    pinMode(gpio, BUTTON, INPUT);
    printf("GPIO configured\n");

    // Тест LED
    printf("Testing Green LED...\n");
    writeLED(gpio, LED, HIGH);
    delay(1000000);
    writeLED(gpio, LED, LOW);
    delay(1000000);
    printf("Green LED test complete\n");

    printf("Testing Red LED...\n");
    writeLED(gpio, LED2, HIGH);
    delay(1000000);
    writeLED(gpio, LED2, LOW);
    delay(1000000);
    printf("Red LED test complete\n");

    // Тест кнопки
    printf("Press button to test (5s timeout)...\n");
    int timeout = 5000000 / 10000;
    int presses = 0;
    while (timeout > 0) {
        int state = readButton(gpio, BUTTON);
        if (state) {
            presses++;
            printf("Button pressed %d times\n", presses);
            delay(200000);
            while (readButton(gpio, BUTTON)) delay(10000);
        }
        delay(10000);
        timeout--;
    }
    if (presses == 0) printf("No button press detected\n");

    // Тест LCD
    printf("Testing LCD...\n");
    writeLCD("Test");
    delay(1000000);

    if (opt_s) {
        theSeq = (int*)malloc(seqlen * sizeof(int));
        if (theSeq == NULL) {
            fprintf(stderr, "Failed to allocate memory for theSeq\n");
            free(attSeq);
            close(fd);
            exit(EXIT_FAILURE);
        }
        readSeq(theSeq, opt_s);
    } else {
        initSeq();
    }

    if (debug) showSeq(theSeq);

    const char *surname = "Smith";  // Замените на свою фамилию
    for (int i = 0; i < 5 && surname[i]; i++) {
        char c = surname[i];
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
            blinkN(gpio, LED, 1);
        else
            blinkN(gpio, LED2, 1);
    }

    writeLCD("MasterMind");

    while (!found) {
        attempts++;
        for (int i = 0; i < seqlen; i++) {
            int presses = 0;
            int timeout = TIMEOUT / 10000;
            printf("Enter color %d (1=green, 2=red, 3=blue, 5s timeout)...\n", i+1);
            while (timeout > 0) {
                int state = readButton(gpio, BUTTON);
                if (state && readButton(gpio, BUTTON)) {
                    presses++;
                    printf("Presses: %d\n", presses);
                    if (presses == 1) { writeLED(gpio, LED, HIGH); writeLED(gpio, LED2, LOW); }
                    else if (presses == 2) { writeLED(gpio, LED, LOW); writeLED(gpio, LED2, HIGH); }
                    else if (presses == 3) { writeLED(gpio, LED, LOW); writeLED(gpio, LED2, LOW); }
                    delay(200000);
                    while (readButton(gpio, BUTTON)) delay(10000);
                }
                delay(10000);
                timeout--;
                if (presses >= 3 || (timeout <= 0 && presses > 0)) break;
            }
            attSeq[i] = presses > colors ? colors : presses;
            if (attSeq[i] == 0) attSeq[i] = 1;
            writeLED(gpio, LED, LOW);
            writeLED(gpio, LED2, LOW);
            blinkN(gpio, LED2, 1);
            if (attSeq[i] == 1) blinkN(gpio, LED, 1);
            else if (attSeq[i] == 2) blinkN(gpio, LED2, 1);
            else if (attSeq[i] == 3) {
                writeLED(gpio, LED, HIGH);
                writeLED(gpio, LED2, HIGH);
                delay(DELAY * 1000);
                writeLED(gpio, LED, LOW);
                writeLED(gpio, LED2, LOW);
                delay(DELAY * 1000);
            }
            delay(COLOR_DELAY); // Задержка 2 секунды между вводом цветов
        }
        blinkN(gpio, LED2, 2);
#ifdef DEBUG
        printf("Secret: ");
        showSeq(theSeq);
        printf("Guess: ");
        showSeq(attSeq);
#endif
        int exact, approx;
        countMatches(theSeq, attSeq, &exact, &approx);
        showMatches(exact, approx, theSeq, attSeq, 1);
        if (exact == seqlen) {
            writeLED(gpio, LED2, HIGH);
            blinkN(gpio, LED, 3);
            writeLED(gpio, LED2, LOW);
            char lcdText[16];
            sprintf(lcdText, "%d %d Success", exact, approx); // Показываем exact и approx с Success
            writeLCD(lcdText);
            found = 1;
        } else {
            blinkN(gpio, LED2, 3);
        }
    }

    close(fd);
    free(theSeq);
    free(attSeq);
    return 0;
}

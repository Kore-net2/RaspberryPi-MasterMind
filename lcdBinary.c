/* Low-level hardware control functions for LED, button, and I2C LCD */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <errno.h>

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

#define GPIO_BASE  0xFE200000
#define GPFSEL0    (GPIO_BASE + 0x00)
#define GPSET0     (GPIO_BASE + 0x1C)
#define GPCLR0     (GPIO_BASE + 0x28)
#define GPLEV0     (GPIO_BASE + 0x34)

#define GPIO_RED_LED   5
#define GPIO_GREEN_LED 26
#define GPIO_BUTTON    19

#define I2C_ADDR       0x27  // I2C адрес вашего LCD
#define LCD_BACKLIGHT  0x08  // Подсветка включена
#define LCD_EN         0x04  // Enable бит
#define LCD_RW         0x02  // Read/Write бит (не используется)
#define LCD_RS         0x01  // Register Select бит

// Прототипы
int failure(int fatal, const char *message, ...);
void pinMode(uint32_t *gpio, int pin, int mode);
void writeLED(uint32_t *gpio, int led, int value);
int readButton(uint32_t *gpio, int button);
void waitForButton(uint32_t *gpio, int button);
void delay(int us);
void lcd_init();
void lcd_write_byte(uint8_t byte, uint8_t mode);
void writeLCD(char *text);

// Глобальный файловый дескриптор для I2C
static int i2c_fd = -1;

void delay(int us) {
    volatile int i;
    for (i = 0; i < us * 100; i++);  // Примерная задержка
}

int failure(int fatal, const char *message, ...) {
    va_list argp;
    char buffer[1024];
    va_start(argp, message);
    vsnprintf(buffer, 1024, message, argp);
    va_end(argp);
    fprintf(stderr, "%s\n", buffer);
    if (fatal) exit(-1);
    return -1;
}

void pinMode(uint32_t *gpio, int pin, int mode) {
    asm volatile (
        "mov r2, %0\n"
        "mov r3, r1, lsr #3\n"
        "add r2, r2, r3, lsl #2\n"
        "ldr r3, [r2]\n"
        "and r1, r1, #31\n"
        "mov r4, #3\n"
        "bic r3, r3, r4, lsl r1\n"
        "cmp %1, #1\n"
        "moveq r4, #1\n"
        "orr r3, r3, r4, lsl r1\n"
        "str r3, [r2]\n"
        :
        : "r" (gpio), "r" (mode)
        : "r1", "r2", "r3", "r4"
    );
}

void writeLED(uint32_t *gpio, int led, int value) {
    asm volatile (
        "mov r2, %0\n"
        "add r2, r2, #0x1C\n"
        "cmp %2, #0\n"
        "addeq r2, %0, #0x28\n"
        "mov r3, #1\n"
        "lsl r3, r3, %1\n"
        "str r3, [r2]\n"
        :
        : "r" (gpio), "r" (led), "r" (value)
        : "r2", "r3"
    );
}

int readButton(uint32_t *gpio, int button) {
    int result;
    asm volatile (
        "mov r1, %1\n"
        "add r1, r1, #0x34\n"
        "ldr r1, [r1]\n"
        "mov r2, #1\n"
        "lsl r2, r2, %2\n"
        "and r1, r1, r2\n"
        "cmp r1, #0\n"
        "moveq %0, #0\n"
        "movne %0, #1\n"
        : "=r" (result)
        : "r" (gpio), "r" (button)
        : "r1", "r2"
    );
    return result;
}

void waitForButton(uint32_t *gpio, int button) {
    while (readButton(gpio, button) == LOW) {
        delay(10000);
    }
    while (readButton(gpio, button) == HIGH) {
        delay(10000);
    }
}

void lcd_init() {
    if ((i2c_fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        failure(TRUE, "Failed to open I2C bus: %s", strerror(errno));
    }
    if (ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR) < 0) {
        failure(TRUE, "Failed to set I2C address: %s", strerror(errno));
    }

    // Инициализация LCD в 4-битном режиме
    usleep(50000);  // Задержка после включения
    lcd_write_byte(0x03, 0); usleep(4500);
    lcd_write_byte(0x03, 0); usleep(150);
    lcd_write_byte(0x03, 0); usleep(150);
    lcd_write_byte(0x02, 0); // Установка 4-битного режима

    lcd_write_byte(0x28, 0); // 4-bit, 2 строки, 5x8
    lcd_write_byte(0x08, 0); // Выключить дисплей
    lcd_write_byte(0x01, 0); // Очистить дисплей
    lcd_write_byte(0x06, 0); // Режим ввода
    lcd_write_byte(0x0C, 0); // Включить дисплей, без курсора
    usleep(2000);
}

void lcd_write_byte(uint8_t byte, uint8_t mode) {
    uint8_t high_nibble = (byte & 0xF0) | LCD_BACKLIGHT | (mode ? LCD_RS : 0);
    uint8_t low_nibble = ((byte << 4) & 0xF0) | LCD_BACKLIGHT | (mode ? LCD_RS : 0);

    // Отправка старшего ниббла
    write(i2c_fd, &(uint8_t){high_nibble | LCD_EN}, 1);
    usleep(1);
    write(i2c_fd, &(uint8_t){high_nibble & ~LCD_EN}, 1);
    usleep(50);

    // Отправка младшего ниббла
    write(i2c_fd, &(uint8_t){low_nibble | LCD_EN}, 1);
    usleep(1);
    write(i2c_fd, &(uint8_t){low_nibble & ~LCD_EN}, 1);
    usleep(50);
}

void writeLCD(char *text) {
    if (i2c_fd < 0) lcd_init();
    lcd_write_byte(0x01, 0); // Очистить дисплей
    usleep(2000);
    for (int i = 0; i < strlen(text) && i < 16; i++) {
        lcd_write_byte(text[i], 1);
    }
}

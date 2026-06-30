#include <stdint.h>
#include <stdbool.h>

#define VGA_MEMORY_PHYS 0xB8000
#define WHITE_ON_BLACK 0x0F
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static int cursor = 0;
volatile uint16_t*  videoMemory = (volatile uint16_t*)VGA_MEMORY_PHYS;

void kclear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        videoMemory[i] = ((uint16_t)WHITE_ON_BLACK << 8) | ' ';
    }
    cursor = 0;
}

void kputc(char c) {
    if (cursor == VGA_WIDTH * VGA_HEIGHT) {
        kclear();
    }
    if (c == '\n') {
        cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
        return;
    }

    videoMemory[cursor] = ((uint16_t)WHITE_ON_BLACK << 8) | c;
    cursor++;
}

void kprint(const char* s) {
    // asm volatile("cli");
    // s--;
    while (*s)
        kputc(*s++);

    // asm volatile("sti");
}

void kprint_hex(uint32_t n) {
    // asm volatile("cli");
    char hexChars[] = "0123456789ABCDEF";
    char result[11];

    // result[-1] = '\0';
    result[0] = '0';
    result[1] = 'x';
    result[10] = '\0';

    for (int i = 2; i <= 9; i++) {
        result[11-i] = hexChars[n & 0x0F];
        n >>= 4;
    }

    kprint(result);
    // asm volatile("sti");
}

void kprint_hexl(uint32_t n) {
    // asm volatile("cli");
    char hexChars[] = "0123456789abcdef";
    char result[11];

    // result[-1] = '\0';
    result[0] = '0';
    result[1] = 'x';
    result[10] = '\0';

    for (int i = 2; i <= 9; i++) {
        result[11-i] = hexChars[n & 0x0F];
        n >>= 4;
    }

    kprint(result);
}

void kprint_udec(int n) {

}

void kprint_dec(int n) {

}


void kprint_hex64(uint64_t n) {
    // asm volatile("cli");
    char hexChars[] = "0123456789ABCDEF";
    char result[19];

    result[0] = '0';
    result[1] = 'x';
    result[18] = '\0';

    for (int i = 2; i <= 17; i++) {
        result[19-i] = hexChars[n & 0x0F];
        n >>= 4;
    }

    kprint(result);
    // asm volatile("sti");
}
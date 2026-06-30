// THANKS TO THE OS DEV WIKI FOR THIS
#include <stdint.h>

void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ( "outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

uint8_t inw(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inw %w1, %w0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

void insw(uint16_t port, void* addr, uint32_t count) {
    __asm__ volatile (
        "rep insw"
        : : "d" (port), "D" (addr), "c" (count)
        : "memory"
    );
}

void outsw(uint16_t port, void* addr, uint32_t count) {
    __asm__ volatile (
        "rep outsw"
        : : "d" (port), "S" (addr), "c" (count)
    );
}

void ioWait(void)
{
    outb(0x80, 0);
}
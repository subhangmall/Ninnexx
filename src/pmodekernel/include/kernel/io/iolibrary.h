#include <stdint.h>

#ifndef IOLIBRARY_H
#define IOLIBRARY_H

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void ioWait(void);
void outw(uint16_t port, uint16_t val);
void insw(uint16_t port, void* addr, uint32_t count);
void outsw(uint16_t port, void* addr, uint32_t count);
uint8_t inw(uint16_t port);

#endif
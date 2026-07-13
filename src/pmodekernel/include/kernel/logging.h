#include <stdint.h>
#include <stdbool.h>

#ifndef LOGGING_H
#define LOGGING_H

extern volatile char* videoMemory;
extern int cursor;

void kclear();
void kputc(char c);
void kprint(const char* s);
void kprint_hex(uint32_t n);
void kprint_hexl(uint32_t n);
void kprint_hex64(uint64_t n);
void switchForegroundProcess();

#endif
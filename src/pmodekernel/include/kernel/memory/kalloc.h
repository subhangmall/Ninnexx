#include <stdint.h>
#include <stdbool.h>

#ifndef KALLOC_H
#define KALLOC_H



void* kalloc(uint32_t numBytes);
void kfree(void* ptr);

#endif
#include <stdint.h>
#include <stdbool.h>

#ifndef MEMSETUP_H
#define MEMSETUP_H

void initMemory(void (*functionToJumpToAfterCompletion)(), uint32_t e820LenAddr, uint32_t e820StartAddress);
extern struct PageDirectoryEntry kernelPageDirectory[PAGE_SIZE/sizeof(struct PageDirectoryEntry)];

#endif
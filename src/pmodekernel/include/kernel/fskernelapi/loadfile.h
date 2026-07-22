#ifndef LOADFILE_H
#define LOADFILE_H

#include <kernel/memory/pmm.h>

uint32_t loadFile(char* path, struct PageDirectoryEntry* kPDNew, uint32_t startAddr);

#endif
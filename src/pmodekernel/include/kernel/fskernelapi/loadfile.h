#ifndef LOADFILE_H
#define LOADFILE_H

#include <kernel/memory/pmm.h>

uint32_t loadFile(char* path, uint32_t kPDNew, uint32_t startAddr);

#endif
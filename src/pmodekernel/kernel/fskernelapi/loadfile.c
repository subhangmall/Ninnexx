#include <kernel/memory.h>
#include <kernel/processes/contextSwitch.h>
#include <kernel/filesystem/fat_filelib.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint32_t loadFile(char* path, struct PageDirectoryEntry* kPDNew, uint32_t startAddr) {
    // DO NOT LOAD AT 0X00000000 b/c will be seen as null
    FL_FILE *file;
    void* buffer;
    long size;
    
    uint32_t oldKPD;

    asm volatile (
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %0"
        : "=m" (oldKPD)
        :
        :
    );

    file = fl_fopen(path, "rb");
    if (!file) {
        switchMemoryContext(oldKPD);
        return 1; // file doesnt exist
    }

    fl_fseek(file, 0, SEEK_END);
    size = fl_ftell(file);
    fl_fseek(file, 0, SEEK_SET);

    for (uint32_t i = ALIGN_DOWN(startAddr); i < ALIGN_UP(startAddr + size); i+=4096) {
        if (!vmmAddPage(i, true, VMM_WRITABLE)) {
            switchMemoryContext(oldKPD);
            return 2; // mmeory allocation error
        }
    }


    buffer = (void*) startAddr;

    // printf("file = %X, buffer = %X\n", file, buffer);
    uint32_t bytesRead = fl_fread(buffer, 1, size, file);
    if (bytesRead != size) {
        fl_fclose(file);
        // printf("Expected %X bytes, got %X\n", size, bytesRead);
        switchMemoryContext( oldKPD);
        return 3; // i on know
    }

    fl_fclose(file);

    switchMemoryContext(oldKPD);
    return 0;
}
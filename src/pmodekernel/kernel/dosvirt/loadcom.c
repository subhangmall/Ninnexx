#include <kernel/fskernelapi/loadfile.h>
#include <kernel/processes/process.h>
#include <kernel/processes/contextSwitch.h>
#include <string.h>
#include <stdio.h>

uint32_t loadCom(char* file, uint8_t len, char* args) {
    uint32_t cr3;
    asm volatile (
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %0"
        : "+m" (cr3)
        :
        :
    );
    uint32_t procStackStruct = createProcStackDirectoryStructure();
    switchMemoryContext(virtToPhysAddr((uint32_t)procStackStruct));
    uint32_t result = loadFile(file, virtToPhysAddr((uint32_t)procStackStruct), 0x0000100);

    if (result != 0)  {
        switchMemoryContext(cr3);
        return result;
    }

    *(uint8_t*) 0x0 = 0xCD;
    *(uint8_t*) 0x1 = 0x20; // interrupt 20h (end program)
    *(uint8_t*) 0x80 = len;
    if (len > 126) len = 126;

    memcpy((void*)0x81, args, len);
    *(char*)(0x81 + len) = '\r';

    uint32_t procID = createNewProcess(false, true, procStackStruct, 0x0100, 0xFFFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
    if (procID == 0xFFFFFFFF) {
        switchMemoryContext(cr3);
        return 4;
    }
    switchMemoryContext(cr3);
    return 0;
}
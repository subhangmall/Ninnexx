#include <stdint.h>
#include "./e820.h"
#include "./pmm.h"
#include "./commonMacros.h"

void continuedHigherHalfMemSetup(uint32_t firstKernelPageTableAddr, uint32_t kernelPageDirectoryAddr) {
    parseE820Output();

    *((uint8_t*)0xB8002) = 'p';
    *((uint8_t*)0xB8003) = 0x0F;

    pmmSet(firstKernelPageTableAddr, PMM_UNAVAILABLE);
    pmmSet(kernelPageDirectoryAddr, PMM_UNAVAILABLE);
    for (int i = 0; i < PAGE_SIZE/sizeof(struct PageTableEntry); i+= PAGE_SIZE) {
        pmmSet(i, PMM_UNAVAILABLE);
    }

    *((uint8_t*)0xB8002) = 'E';
    *((uint8_t*)0xB8003) = 0x0F;
}


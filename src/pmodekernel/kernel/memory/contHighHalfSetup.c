#include <stdint.h>
#include <kernel/memory/e820.h>
#include <kernel/memory/pmm.h>
#include <kernel.h>
#include <kernel/memory.h>
#include <kernel/logging.h>
#include <stdio.h>

extern uint32_t _kernel_phys_end;
extern uint32_t _kernel_phys_start;

void continuedHigherHalfMemSetup(uint32_t e820LenAddr, uint32_t e820StartAddress, uint32_t firstKernelPageTableAddr, uint32_t kernelPageDirectoryAddr) {
    parseE820Output(e820LenAddr, e820StartAddress);
    printf("len addr: %X\nstart addr: %X", e820LenAddr, e820StartAddress);
    // kprint_hex(e820LenAddr);
    // kprint("\nstart addr:");
    // kprint_hex(e820StartAddress);

    // *((uint8_t*)0xB8002) = 'p';
    // *((uint8_t*)0xB8003) = 0x0F;

    uint32_t krnlStart = (uint32_t) & _kernel_phys_start;
    
    uint32_t krnlEnd = (uint32_t) & _kernel_phys_end;

    // kprint_hex(krnlEnd);

    pmmSet(firstKernelPageTableAddr, PMM_UNAVAILABLE);
    pmmSet(kernelPageDirectoryAddr, PMM_UNAVAILABLE);
    for (int i = (krnlStart & 0xFFFFF000); i < (krnlEnd | 0x00000FFF); i+= PAGE_SIZE) {
        pmmSet(i, PMM_UNAVAILABLE);
    }

    // *((uint8_t*)0xB8002) = 'E';
    // *((uint8_t*)0xB8003) = 0x0F;
}


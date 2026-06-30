// TODO: FIX WEIRD MEMORY

#include <stdint.h>
#include <stdbool.h>
#include <kernel/memory/memSetup.h>
// #include "./memory/kmemmgt.h"
// #include "./idt.h"
#include <kernel/logging.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/pic.h>
#include <kernel/memory/vmm.h>
// #include <kernel/memory/kalloc.h>
#include <kernel/interrupts/initInterruptHandlers.h>
#include <kernel/time/time.h>
#include <kernel/memory/pmm.h>
#include <kernel/genericDrivers/ps2keyboard.h>
#include <kernel/genericDrivers/atapio.h>
#include <kernel/filesystem/fat_filelib.h>
#include <stdlib.h>

extern void gdtFlush();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress);
void continueInitialization();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress) { // dummy CS because CS Is also pushed onto the stack cause it is called from a far jump
    initMemory(&continueInitialization, e820LenAddr, e820StartAddress);
}

void continueInitialization() {  
    videoMemory = (volatile char*) vmmAllocatePhysicalRange(0xB8000, 4000);
    kclear();

    // remap gdt to higher half
    gdtFlush();

    // struct PageDirectoryEntry* lowHalfPDE = (struct PageDirectoryEntry*) 0xFFFFF000;
    // lowHalfPDE->present=0;
    // asm volatile (
    //     "mov %%cr3, %%eax\n\t"
    //     "mov %%eax, %%cr3"
    //     :
    //     :
    //     : "eax"
    // );

    // kprint("hi");

    // setup interrupts/pic
    setupInterruptStructures();
    initPIC(0x20, 0x28);
    initInterruptHandlers();
    loadIDTR();
    enableInterrupts();
    initKeyboard();
    enablePIC();
    
    // time subsystem to set frequency
    setPitPeriodic(1193); // fire (around) every  10 ms

    identify();
    if (fl_attach_media(fatReadWrapper, fatWriteWrapper) != FAT_INIT_OK) {
        printf("Error mounting FAT filesystem.\n");
    }
    fl_init();

    


    while (1) {}

}
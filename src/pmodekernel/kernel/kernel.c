// TODO: FIX WEIRD MEMORY

#include <stdint.h>
#include <stdbool.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/pic.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/memSetup.h>
#include <kernel/interrupts/initInterruptHandlers.h>
#include <kernel/time/time.h>
#include <kernel/genericDrivers/ps2keyboard.h>
#include <kernel/genericDrivers/atapio.h>
#include <kernel/filesystem/fat_filelib.h>
#include <kernel/processes/process.h>
#include <kernel/logging.h>
#include <kernel.h>
#include <stdlib.h>

extern void gdtFlush();
extern struct PageDirectoryEntry kernelPageDirectory[4096/sizeof(struct PageDirectoryEntry)];

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress);
void continueInitialization();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress) { // dummy CS because CS Is also pushed onto the stack cause it is called from a far jump
    initMemory(&continueInitialization, e820LenAddr, e820StartAddress);
}

extern struct Process procHead;
extern struct Process* current;

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


    // THIS FUNCTION WILL EVENTUALLY WILL BECOME PROCHEAD, ESSENTIALLY WHAT IS SCHEDULED WHEN THERE IS NOTHING ELSE TO RUN
    procHead.procID = 0;
    procHead.next = &procHead;
    procHead.kernel = true;
    procHead.v8086 = false;
    procHead.cr3 = (uint32_t)&kernelPageDirectory;
    current = &procHead;
    asm volatile("int $0xFF");
    kprint("hi");
    asm volatile("int $0xFF");
    kprint("hi again");


    while (1) {}

}
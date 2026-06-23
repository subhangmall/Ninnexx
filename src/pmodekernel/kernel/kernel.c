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
#include <kernel/memory/kalloc.h>
#include <kernel/interrupts/initInterruptHandlers.h>
#include <kernel/time/time.h>
#include <kernel/memory/pmm.h>
// #include "./gdt.asm"

// extern void int0(void);
// #include "./memory/kmemmgt.h"
// #include "./logging.h"
// #include "./idt.h"
// #include "./pic.h"
// #include "./basicInterruptHandlers.h"
extern void gdtFlush();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress);
void continueInitialization();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress) { // dummy CS because CS Is also pushed onto the stack cause it is called from a far jump
    initMemory(&continueInitialization, e820LenAddr, e820StartAddress);
}

void continueInitialization() {  
    // setup screen
    videoMemory = (volatile char*) vmmAllocatePhysicalRange(0xB8000, 4000);
    kclear();

    // remap gdt to higher half
    gdtFlush();

    // setup interrupts/pic
    setupInterruptStructures();
    initPIC(0x20, 0x28);
    initInterruptHandlers();
    loadIDTR();
    enableInterrupts();
    enablePIC();
    
    // time subsystem to set frequency
    setPitPeriodic(1193); // fire (around) every  10 ms

    // a[0]= 'A';
    // kprint_hex(mmioNextFree);
    // struct PageTableEntry* aPTE = (struct PageTableEntry*) (0xFFC00000 + (((uint32_t)a) >> 12));
    // kprint_hex(aPTE->pageAddress << 12);
    // kprint("\n");
    // kprint_hex((uint32_t)a);
    

    // unmap lower half
    struct PageDirectoryEntry* lowHalfPDE = (struct PageDirectoryEntry*) 0xFFFFF000;
    lowHalfPDE->present=0;
    asm volatile (
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %%cr3"
        :
        :
        : "eax"
    );


    // asm volatile (
    //     "int $0x01"
    //     :
    //     :
    //     :
    // );

    // kprint("hi");
    
    sleep(2000);
    // kprint("1seclater");
    // sleep(10000);
    // kprint("10seclater");
    // kprint("esp: ");
    // uint32_t esp;
    // asm volatile (
    //     "mov %%esp, %%eax\n\t"
    //     "mov %%eax, %0\n\t"
    //     : "=m" (esp)
    //     : 
    //     : "eax"
    // );
    // kprint_hex(esp);
    // uint32_t* a = (uint32_t*)kalloc(10000);

    

    while (1) {}

}
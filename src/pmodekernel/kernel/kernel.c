// TODO: FIX WEIRD MEMORY

#include <stdint.h>
#include <stdbool.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/pic.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/memSetup.h>
#include <kernel/memory.h>
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
extern bool shouldTrack;
extern bool procEnabled;

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress);
void continueInitialization();
void hi();

__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress) { // dummy CS because CS Is also pushed onto the stack cause it is called from a far jump
    initMemory(&continueInitialization, e820LenAddr, e820StartAddress);
}

extern struct Process procHead;
extern struct Process* current;

void continueInitialization() {  
    vmmAllocatePage(PARENT_KPD_ADDR, (uint32_t) &kernelPageDirectory, VMM_WRITABLE); // master page table now at PARENT_KPD
    
    videoMemory = (volatile char*) vmmAllocatePhysicalRange(0xB8000, 4000);
    kclear();

    // remap gdt to higher half
    gdtFlush();

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

    struct PageDirectoryEntry* lowHalfPDE = (struct PageDirectoryEntry*) 0xFFFFF000;
    lowHalfPDE->present=0;
    asm volatile (
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %%cr3"
        :
        :
        : "eax"
    );

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
    procHead.zombie = false;
    current = &procHead;
    shouldTrack = true; // enable setting the page directory entry
    procEnabled = true;
    vmmAllocatePage(PARENT_KPD_ADDR, (uint32_t)&kernelPageDirectory, VMM_WRITABLE);
    printf("phys: %X, virt to phys: %X", (uint32_t)&kernelPageDirectory, virtToPhysAddr(PARENT_KPD_ADDR));
    createNewProcess(true, false, createProcStackDirectoryStructure(), (uint32_t)&hi, 0);
    // kprint
    printf("asdfadsfasdf");
    // printf("hiiiiiiiii");
    // uint32_t* a = (uint32_t*)malloc(100);
    // printf("%X", *a);

    while (1) {
        printf("a");
        sleep(1000);
    }
}

void hi() {
    // printf("hiiiiiiiii");
    // uint32_t* a = (uint32_t*)malloc(100);
    // printf("%X", *a);

    while (1) {}
}
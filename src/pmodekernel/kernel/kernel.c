#include <stdint.h>
#include <stdbool.h>
#include "./memory/memSetup.h"
// #include "./memory/kmemmgt.h"
// #include "./idt.h"
#include "./logging.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./memory/vmm.h"
#include "./memory/kalloc.h"
#include "./interrupts/initInterruptHandlers.h"
// #include "./gdt.asm"

// extern void int0(void);
// #include "./memory/kmemmgt.h"
// #include "./logging.h"
// #include "./idt.h"
// #include "./pic.h"
// #include "./basicInterruptHandlers.h"
extern void gdtFlush();

__attribute__((section(".boot"))) void kentry(void);
void continueInitialization();

// volatile char* videoMemory;

// #define VMM_PRESENT    0x1
// #define VMM_WRITABLE   0x2
// #define VMM_USER       0x4
// #define VMM_MMIO       0x8

// #define PAGE_SIZE 4096
// #define RECURSIVE_PD_ADDR 0xFFC00000
// #define RECURSIVE_PD ((struct PageDirectoryEntry*) RECURSIVE_PD_ADDR) // where Page Directory Table appears in memory b/c the last idx of the PDT refers back to the start of the PDT
// #define NULL ((void*)0)
// #define MMIO_VIRTUAL_SPACE_BASE 0xE0000000
// #define MMIO_VIRTUAL_SPACE_SIZE 0x10000000
// #define AVAILABLE_MEM_E820 1
// #define MAX_REGIONS 1024
// #define PMM_UNAVAILABLE true
// #define PMM_AVAILABLE false
// #define ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
// #define ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// #define KERNEL_STACK_VADDR 0xC0100000
// #define KERNEL_STACK_SIZE  0x4000

// void highHalfEntry(void);

// #pragma pack(push, 1)

// struct PageDirectoryEntry {
//     uint32_t present : 1;
//     uint32_t rw : 1;
//     uint32_t user : 1;
//     uint32_t pwt : 1;
//     uint32_t pcd : 1;
//     uint32_t accessed : 1;
//     uint32_t reserved : 1;
//     uint32_t pageSize : 1;
//     uint32_t ignored : 1;
//     uint32_t available : 3;
//     uint32_t pageAddress : 20;
// };

// struct PageTableEntry {
//     uint32_t present : 1;
//     uint32_t rw : 1;
//     uint32_t user : 1;
//     uint32_t pwt : 1;
//     uint32_t pcd : 1;
//     uint32_t accessed : 1;
//     uint32_t dirty : 1;
//     uint32_t pageTableAttrIdx : 1;
//     uint32_t globalPage : 1;
//     uint32_t available : 3;
//     uint32_t pageAddress : 20;
// };

// #pragma pack(pop)


// __attribute__((section(".boot.data")))
// static uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8];

// __attribute__((aligned(PAGE_SIZE)))
// __attribute__((section(".boot.data")))
// struct PageDirectoryEntry kernelPageDirectory[PAGE_SIZE/sizeof(struct PageDirectoryEntry)];

// __attribute__((aligned(PAGE_SIZE)))
// __attribute__((section(".boot.data")))
// struct PageTableEntry firstKernelPageTable[PAGE_SIZE/sizeof(struct PageTableEntry)];

// __attribute__((section(".boot.data")))
// static int cursor = 0;

// __attribute__((section(".boot.data")))
// volatile char* videoMemory = (volatile char*) 0xB8000;

// __attribute__((section(".boot"))) void kentry(void);
// __attribute__((section(".boot"))) void kprint_hex(uint32_t n);
// __attribute__((section(".boot"))) void pmmSet(uint32_t physAddr, bool val);
// __attribute__((section(".boot"))) bool pmmGet(uint32_t physAddr);



__attribute__((section(".boot"))) void kentry(void) {
    // *((uint8_t*)0xB8002) = 'l';
    // *((uint8_t*)0xB8003) = 0x0F;

    // videoMemory = (volatile char*) 0xB8000;

    // asm volatile("hlt");

    // kclear();

    initMemory(&continueInitialization);
}

void continueInitialization() {
    // volatile char* a = (volatile char*) allocatePhysicalRange(0xB8000, 4000);
    // videoMemory = (volatile char*) 0xC00B8000;


    // kprint_hex((uint32_t) &a);
    
    videoMemory = (volatile char*) vmmAllocatePhysicalRange(0xB8000, 4000);

    setupInterruptStructures();
    initPIC(0x20, 0x28);
    // disablePIC();
    enableInterrupts();
    // kprint_hex(1/0);
    // asm volatile (
    //     "int $0x00"
    //     :
    //     :
    //     :
    // );
    asm volatile (
        "int $0x40"
        :
        :
        :
    );
    kprint("hi!!!;D");

    uint8_t* a = kalloc(5000);
    kprint_hex((uint32_t) a);
    // kprint_hex(1/0);
    // setIDTHandler(0, )
    initInterruptHandlers();
    gdtFlush();
    // kprint_hex(1/0);
    // kprint_hex((uint32_t) *(uint8_t*) 0x12345567);

    while (1) {}

}
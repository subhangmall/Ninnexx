#include <stdint.h>
#include <stdbool.h>
// #include "kmemmgt.h"
#include "./contHighHalfSetup.h"
#include "./commonMacros.h"

void highHalfEntry(void);

#pragma pack(push, 1)

struct PageDirectoryEntry {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t accessed : 1;
    uint32_t reserved : 1;
    uint32_t pageSize : 1;
    uint32_t ignored : 1;
    uint32_t available : 3;
    uint32_t pageAddress : 20;
};

struct PageTableEntry {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pageTableAttrIdx : 1;
    uint32_t globalPage : 1;
    uint32_t available : 3;
    uint32_t pageAddress : 20;
};

#pragma pack(pop)


// static uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8];

__attribute__((aligned(PAGE_SIZE)))
__attribute__((section(".boot.data")))
struct PageDirectoryEntry kernelPageDirectory[PAGE_SIZE/sizeof(struct PageDirectoryEntry)];

__attribute__((aligned(PAGE_SIZE)))
__attribute__((section(".boot.data")))
struct PageTableEntry firstKernelPageTable[PAGE_SIZE/sizeof(struct PageTableEntry)];

__attribute__((section(".boot"))) void initMemory(void (*functionToJumpToAfterCompletion)()) {
    // kclear();
    // *((uint8_t*)0xB8000) = 'A';
    // *((uint8_t*)0xB8001) = 0x0F;

    
    struct PageDirectoryEntry pde = {
        .present = 0,
        .rw = 0,
        .user = 0,
        .pwt = 0,
        .pcd = 0,
        .accessed = 0,
        .reserved  = 0,
        .pageSize  = 0,
        .ignored  = 0,
        .available = 0,
        .pageAddress = 0
    };

    struct PageTableEntry pte = { 
        .present = 1,
        .rw = 1,
        .user = 0,
        .pwt = 0,
        .pcd = 0,
        .accessed = 0,
        .dirty = 0,
        .pageTableAttrIdx = 0,
        .globalPage = 0,
        .available = 0,
        .pageAddress = 0
    };

    // *((uint8_t*)0xB8000) = 'B';
    // *((uint8_t*)0xB8001) = 0x0F;

    // *((uint8_t*)0xB8002) = 'B';
    // *((uint8_t*)0xB8003) = 0x0F;

    for (int i = 0; i < PAGE_SIZE/sizeof(struct PageTableEntry); i++) {
        kernelPageDirectory[i] = pde; // zero PDEs
        
        pte.pageAddress = i;
        
        firstKernelPageTable[i] = pte; // setup PTE for first 4 megabytes
    }

    // *((uint8_t*)0xB8000) = 'C';
    // *((uint8_t*)0xB8001) = 0x0F;

    // *((uint8_t*)0xB8004) = 'C';
    // *((uint8_t*)0xB8005) = 0x0F;

    struct PageDirectoryEntry firstPageDirectoryEntry =  {
        .present = 1,
        .rw = 1,
        .user = 0,
        .pwt = 0,
        .pcd = 0,
        .accessed = 0,
        .reserved  = 0,
        .pageSize  = 0,
        .ignored  = 0,
        .available = 1,
        .pageAddress = ((uint32_t) &firstKernelPageTable) >> 12
    };

    // *((uint8_t*)0xB8000) = 'D';
    // *((uint8_t*)0xB8001) = 0x0F;

    // // pmmSet((uint32_t) &firstKernelPageTable, PMM_UNAVAILABLE);

    // *((uint8_t*)0xB8006) = 'D';
    // *((uint8_t*)0xB8007) = 0x0F;

    // points to page directory itself
    struct PageDirectoryEntry loopBack =  {
        .present = 1,
        .rw = 1,
        .user = 0,
        .pwt = 0,
        .pcd = 0,
        .accessed = 0,
        .reserved  = 0,
        .pageSize  = 0,
        .ignored  = 0,
        .available = 1,
        .pageAddress = ((uint32_t) &kernelPageDirectory) >> 12
        // .pageAddress = 0
    };

    // *((uint8_t*)0xB8008) = 'E';
    // *((uint8_t*)0xB8009) = 0x0F;

    // // pmmSet((uint32_t) &kernelPageDirectory, PMM_UNAVAILABLE);

    // // points to first page
    kernelPageDirectory[0] = firstPageDirectoryEntry;
    kernelPageDirectory[0xC0000000 >> 22] = firstPageDirectoryEntry; // address 0xC0000000
    kernelPageDirectory[1023] = loopBack;

    // *((uint8_t*)0xB800A) = 'F';
    // *((uint8_t*)0xB800B) = 0x0F;
    
    // REMEMBER TO FLUSH TLB PER CHANGE
    // *((uint8_t*)0xB8002) = 'B';
    // *((uint8_t*)0xB8003) = 0x0F;

    // move addr into cr3 register and flip the right bit on cr0 to enable paging
    uint32_t kPDAddress = (uint32_t)&kernelPageDirectory;

    // *((uint8_t*)0xB800A) = 'G';
    // *((uint8_t*)0xB800B) = 0x0F;
    
    // void (*highHalfEntryPTR)() = highHalfEntry;
    // kprint_hex((uint32_t)highHalfEntryPTR);
    // highHalfEntryPTR += 0xC0000000;

    asm volatile (
        "mov %0, %%cr3\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0\n\t"
        "jmp 1f\n\t" // to clear prefetched instructions
        "1:\n\t"
        // "mov $0xC0090000, %%esp\n\t"
        "addl $0xC0000000, %%esp\n\t"
        "push %1\n\t" // ADDRESS TO RETURN TO
        "push %2\n\t"
        "push %3\n\t"
        "jmp *%4\n\t"        
        :
        :  "r" (kPDAddress), "r" (&kernelPageDirectory), "r" (&firstPageDirectoryEntry), "r"(functionToJumpToAfterCompletion), "r"(continuedHigherHalfMemSetup)
        // :  "r" (kPDAddress)
        : "eax", "esp"
    );

    // highHalfEntry();
    // while(1) {
    //     __asm volatile ("hlt");
    // }
}

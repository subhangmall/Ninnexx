#include <stdint.h>
#include <stdbool.h>
#include <kernel/memory/contHighHalfSetup.h>
#include <kernel.h>
#include <kernel/memory.h>

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

__attribute__((aligned(PAGE_SIZE)))
__attribute__((section(".boot.data")))
struct PageDirectoryEntry kernelPageDirectory[PAGE_SIZE/sizeof(struct PageDirectoryEntry)];

__attribute__((aligned(PAGE_SIZE)))
__attribute__((section(".boot.data")))
struct PageTableEntry firstKernelPageTable[PAGE_SIZE/sizeof(struct PageTableEntry)];

// __attribute__((aligned(PAGE_SIZE)))
// __attribute__((section(".boot.data")))
// struct PageTableEntry firstKernelPageTableHighHalf[PAGE_SIZE/sizeof(struct PageTableEntry)];

__attribute__((section(".boot"))) void initMemory(void (*functionToJumpToAfterCompletion)()) {
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

    for (int i = 0; i < PAGE_SIZE/sizeof(struct PageTableEntry); i++) {
        kernelPageDirectory[i] = pde; // zero PDEs
        
        pte.pageAddress = i;
        
        firstKernelPageTable[i] = pte; // setup PTE for first 4 megabytes
        
        // firstKernelPageTableHighHalf[i] = pte;
    }

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

    struct PageDirectoryEntry firstPageDirectoryEntryHighHalf =  {
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
        .pageAddress = ((uint32_t) &firstKernelPageTable) >> 12 // COME BAKC TO THIS LATER!!!!!!!!!
    };

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
    };


    // // points to first page
    kernelPageDirectory[0] = firstPageDirectoryEntry;
    kernelPageDirectory[0xC0000000 >> 22] = firstPageDirectoryEntryHighHalf; // address 0xC0000000
    kernelPageDirectory[1023] = loopBack;

    // move addr into cr3 register and flip the right bit on cr0 to enable paging
    uint32_t kPDAddress = (uint32_t)&kernelPageDirectory;

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

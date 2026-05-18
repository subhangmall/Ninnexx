#include <stdint.h>
#include <stdbool.h>
#include "../logging.h"

#define PAGE_SIZE 4096
#define RECURSIVE_PT_ADDR 0xFFC00000
#define KERNEL_PAGE_DIRECTORY_ADDR 0xFFFFF000
#define KERNEL_PAGE_DIRECTORY ((struct PageDirectoryEntry*) KERNEL_PAGE_DIRECTORY_ADDR) 
#define NULL ((void*)0)
#define MMIO_VIRTUAL_SPACE_BASE 0xE0000000
#define MMIO_VIRTUAL_SPACE_SIZE 0x10000000
#define AVAILABLE_MEM_E820 1
#define MAX_REGIONS 1024
#define PMM_UNAVAILABLE true
#define PMM_AVAILABLE false
#define ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

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

struct E280Entry {
    uint64_t baseAddr;
    uint64_t len;
    uint32_t type; 
};

struct UsableRegion {
    uint64_t base;
    uint64_t length;
};

#pragma pack(pop)

uint32_t pmmAllocNextFreePage();

static uint32_t mmioNextFree = MMIO_VIRTUAL_SPACE_BASE;
static struct UsableRegion usableRegions[MAX_REGIONS];
static uint8_t usableRegionsCurrentLength = 0;
static uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8];
static uint32_t nextFreePageFramePhysicalStartingAddress = 4194304 - PAGE_SIZE;
static uint32_t maxMemory = 4294967296 - 1;
static uint32_t currentRegion = 0;

void pmmSet(uint32_t physAddr, bool val) {

    if (val) {
        physicalPageRecord[physAddr >> 15] |= 0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12);
    } else { 
        physicalPageRecord[physAddr >> 15] &= ~(0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12));
    }
}

bool pmmGet(uint32_t physAddr) {
    return (bool)(physicalPageRecord[physAddr >> 15] & (0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12)) >> ((physAddr & 0b00000000000000000111000000000000)>>12));
}

// page allocation from far memory. returns physical first 20 bits of address shifted right 12 bites
uint32_t pmmAllocNextFreePage() {
    // bool found = true;
    // for (int i = 0; i < usableRegionsCurrentLength; i++) {
    //     if (usableRegions[i].base <= nextFreePageFramePhysicalStartingAddress && !(4096<=(usableRegions[i].length + usableRegions[i].base - nextFreePageFramePhysicalStartingAddress))) {
    //         for (int j = i; j < usableRegionsCurrentLength; j++) {
    //             if (usableRegions[j].length >= 4096) {
    //                 nextFreePageFramePhysicalStartingAddress = usableRegions[j].base; 
    //                 break;
    //             } else if (j + 1 == usableRegionsCurrentLength) {
    //                 found = false;
    //             }
    //         }
    //         break;
    //     }
    // }

    // shifted down 12 b/c 2^12 is 4096, the size of a page 
    // shifted down 8 because there are 8 bits per byte, each representing a page table

    for (int i = 0; i < 0xFFFFFFFF; i += 4096) {
        // kprint("Found address!!!!!!!!!!!!");
        if (!pmmGet(i)) {
            pmmSet(i, PMM_UNAVAILABLE);
            return i;
        }
    }
    return 0xFFFFFFFF; // no pages left

    // if (nextFreePageFramePhysicalStartingAddress + PAGE_SIZE > maxMemory) {
    //     return 0xFFFFFFFF;
    // } else {
    //     nextFreePageFramePhysicalStartingAddress += PAGE_SIZE;

    //     return nextFreePageFramePhysicalStartingAddress;
    // }
}

// zero out PTE for an unused page
void premove(uint32_t vAddr) {
    // first 10 bits is Page Directory, next 10 is table index, next 12 is page offset
    // loop back to start of page directory, then set the PTE to zero
    uint32_t *PTE = (uint32_t*)((RECURSIVE_PT_ADDR) + 
        ((0b11111111111111111111000000000000 & vAddr) >> 10));
    *PTE = 0x00000000; // deactivate page in memory


    // refresh tlb
    asm volatile (
            "invlpg (%0)"
            :
            : "r" (vAddr)
            : "memory"
    );
}

bool createNewPageTable(uint32_t vAddr) {
    uint16_t ptdIdx = (uint16_t)((vAddr >> 22)&0b1111111111);

    uint32_t pageStart = pmmAllocNextFreePage();

    if (pageStart == 0xFFFFFFFF) return false; // no mem left

    struct PageDirectoryEntry new  = {
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
        .pageAddress = pageStart >> 12
    };

    KERNEL_PAGE_DIRECTORY[ptdIdx] = new;

    asm volatile (
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n"
        :
        :
        : "eax", "memory"
    );

    uint8_t *newPageTable = (uint8_t*) (RECURSIVE_PT_ADDR + (ptdIdx << 12));

    for (int i = 0; i < 4096; i++) {
        newPageTable[i] = 0x00;
    };

    return true;
}

void* allocatePhysicalRange(uint32_t physAddr, uint32_t len) {
    uint16_t offset = physAddr & 0x00000FFF;
    uint32_t virtualStartAddr = mmioNextFree;

    for (int i = ALIGN_DOWN(physAddr); i < ALIGN_UP(physAddr + len); i += 4096) {
        pmmSet(i, PMM_UNAVAILABLE);
        vmmAllocatePage(mmioNextFree, i, VMM_MMIO);
        mmioNextFree += 4096;
    }

    return (uint32_t*)(virtualStartAddr + offset);
}


void deallocatePhysicalRange(uint32_t vAddr, uint32_t len) {

}
#include <stdint.h>
#include <kernel.h>
#include <kernel/memory.h>
#include <kernel/processes/atlock.h>
#include <kernel/processes/process.h>

uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8];
bool shouldTrack = false;
// static uint32_t mmioNextFree = MMIO_VIRTUAL_SPACE_BASE;

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

void pmmSet(uint32_t physAddr, bool val) {
    // acquireLock(&pprLock);
    if (val) {
        physicalPageRecord[physAddr >> 15] |= 0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12);
    } else { 
        physicalPageRecord[physAddr >> 15] &= ~(0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12));
    }
    // releaseLock(&pprLock);
}

bool pmmGet(uint32_t physAddr) {
    // acquireLock(&pprLock);
    return (bool)(physicalPageRecord[physAddr >> 15] & (0b00000001 << ((physAddr & 0b00000000000000000111000000000000) >> 12)) >> ((physAddr & 0b00000000000000000111000000000000)>>12));
    // releaseLock(&pprLock);
}

// page allocation from far memory. returns physical first 20 bits of address shifted right 12 bites
uint32_t pmmAllocNextFreePage() {
    for (int i = 0; i < 0xFFFFFFFF; i += 4096) {
        // kprint("Found address!!!!!!!!!!!!");
        if (!pmmGet(i)) {
            pmmSet(i, PMM_UNAVAILABLE);
            // releaseLock(&pprLock);
            return i;
        }
    }
    return 0xFFFFFFFF; // no pages left    
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

// FIX FUNCTION
uint32_t virtToPhysAddr(uint32_t vAddr) {
    uint32_t *PTE = (uint32_t*)((RECURSIVE_PT_ADDR) + 
        ((0xFFFFF000 & vAddr) >> 10));
    struct PageTableEntry *pte = (struct PageTableEntry*) PTE;
    // << 12
    // kprint_hex((((1) << 12) + (0x00000FFF & vAddr)));
    // kprint_hex(pte->pageAddress);

    // return (((pte->pageAddress) << 12) + (0x00000FFF & vAddr));

    return 0xFFFFFFFF;
}

bool createNewPageTable(uint32_t vAddr) {
    bool usr = false;

    if ((vAddr & 0xFFC00000) + (4096 * 1024) < 0xC0000000) {
        usr = true;
    }

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

    if (usr) {
        new.user = 1;
    }

    KERNEL_PAGE_DIRECTORY[ptdIdx] = new;
    if (current->procID != 0 && shouldTrack && usr) {
        // update, essentially, swapper_pg_dir
        PARENT_KPD[ptdIdx]=new;
    }

    asm volatile (
        "mov %%cr3, %%eax\n"
        "mov %%eax, %%cr3\n"
        :
        :
        : "eax", "memory"
    );

    volatile uint8_t *newPageTable = (volatile uint8_t*) (RECURSIVE_PT_ADDR + (ptdIdx << 12));

    for (int i = 0; i < 4096; i++) {
        newPageTable[i] = 0x00;
    };
    asm volatile("" ::: "memory");

    return true;
}
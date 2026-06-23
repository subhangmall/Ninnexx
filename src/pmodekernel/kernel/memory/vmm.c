#include <stdint.h>
#include <stdbool.h>
#include <kernel/memory/pmm.h>
#include <kernel/logging.h>
#include <kernel.h>
#include <kernel/memory.h>

uint32_t mmioNextFree = MMIO_VIRTUAL_SPACE_BASE;

void vmmZeroPage(uint32_t vAddr);

void vmmRemovePage(uint32_t vAddr) {
    ((struct PageTableEntry*)(RECURSIVE_PT_ADDR + (vAddr >> 12)))->present=0;
    // REFRESH
    asm volatile (
            "invlpg (%0)"
            :
            : "r" (vAddr)
            : "memory"
    );
}

bool vmmAllocatePage(uint32_t vAddr, uint32_t physAddr, uint8_t flags) {
    // kprint("Vaddr : ");
    // kprint_hex(vAddr);

    uint16_t ptdIdx = (uint16_t)((vAddr >> 22)&0b1111111111);

    if (KERNEL_PAGE_DIRECTORY[ptdIdx].present == 1) {
        // page table present
        struct PageTableEntry* pte = (struct PageTableEntry*)(RECURSIVE_PT_ADDR + ((vAddr >> 12) * 4)); 
        
        if (pte->present == 0) {
            // page not allocated yet
            uint32_t pagePhysicalAddr = physAddr & 0xFFFFF000;
            if (pagePhysicalAddr == 0xFFFFFFFF) return false; // out of memory

            // pmmSet(pagePhysicalAddr, PMM_UNAVAILABLE);

            struct PageTableEntry pte = { 
                .present = 0,
                .rw = 0,
                .user = 0,
                .pwt = 0,
                .pcd = 0,
                .accessed = 0,
                .dirty = 0,
                .pageTableAttrIdx = 0,
                .globalPage = 0,
                .available = 0,
                .pageAddress = (pagePhysicalAddr >> 12)
            };

            if (flags == VMM_PRESENT) {
                pte.present = 1;
            }

            if (flags == VMM_WRITABLE) {
                pte.present = 1;
                pte.rw = 1;
            }

            if (flags == VMM_USER) {
                pte.present = 1;
                pte.rw = 1;
                pte.user = 1;
            }

            if (flags == VMM_MMIO) {
                pte.present = 1;
                pte.rw = 1;
                pte.pcd = 1;
                pte.pwt = 1;
            }
            
            uint32_t pteAddress = RECURSIVE_PT_ADDR + ((vAddr >> 12) * 4);
            struct PageTableEntry* ptePtr = (struct PageTableEntry*)pteAddress;
                
            *ptePtr = pte;

            // FIX LATER
            // pmmSet(pagePhysicalAddr, PMM_AVAILABLE);

            // 0xFFC0000 refers to index 1023, which just refers back to itself
            // vaddr >> 12 * 4 is the page table number + page directory number
                
            asm volatile (
            "invlpg (%0)"
            :
            : "r" (vAddr)
            : "memory"
            );
            
            // if (shouldZero) {
            //     vAddr &= 0b11111111111111111111000000000000;
            //     uint8_t* pageBytes = (uint8_t*) vAddr;
            //     for (int i = 0; i < 4096; i++) {
            //         pageBytes[i] = 0x00;
            //     }
            // }
            kprint("Allocated page at ");
            // *vAddr = 
            kprint_hex(vAddr);
            kprint("\n");
            return true;

        } else {
            kprint("page already allocated\n");
            return true; // page already allocated
        }
        
    } else {
        kprint("allocating page\n");
        // page table not present
       if (!createNewPageTable(vAddr)) return false;

       return vmmAllocatePage(vAddr, physAddr, flags);
    }
}

bool vmmAddPage(uint32_t vAddr, bool shouldZero, uint8_t flags) {
    bool result = vmmAllocatePage(vAddr, pmmAllocNextFreePage(), flags);
    if (shouldZero && result) {
        vmmZeroPage(vAddr);
        return true;
    } else {
        return result;
    }
}

void vmmZeroPage(uint32_t vAddr) {
    vAddr &= 0b11111111111111111111000000000000;
    uint8_t* pageBytes = (uint8_t*) vAddr;
    for (int i = 0; i < 4096; i++) {
        pageBytes[i] = 0x00;
    }
}

void* vmmAllocatePhysicalRange(uint32_t physAddr, uint32_t len) {
    uint16_t offset = physAddr & 0x00000FFF;
    uint32_t virtualStartAddr = mmioNextFree;

    for (int i = ALIGN_DOWN(physAddr); i < ALIGN_UP(physAddr + len); i += 4096) {
        pmmSet(i, PMM_UNAVAILABLE);
        vmmAllocatePage(mmioNextFree, i, VMM_MMIO);
        mmioNextFree += 4096;
    }

    return (uint32_t*)(virtualStartAddr + offset);
}

void vmmDeallocatePhysicalRange(uint32_t vAddr, uint32_t len) {

}
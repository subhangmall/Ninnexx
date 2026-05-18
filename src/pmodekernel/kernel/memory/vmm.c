#include "./pmm.h"

#include <stdint.h>
#include <stdbool.h>
#include "../logging.h"

#pragma pack(push, 1)

struct KallocHeader {
    uint32_t size;
    bool isFree;
    struct KallocHeader* next;
};

#pragma pack(pop)

#define VMM_PRESENT    0x1
#define VMM_WRITABLE   0x2
#define VMM_USER       0x4
#define VMM_MMIO       0x8

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

void* kalloc(uint32_t numBytes);
// void* kcalloc(uint32_t numBytes);
bool vmmAddPage(uint32_t vAddr, bool shouldZero);
void free(void* ptr);
bool vmmAllocatePage(uint32_t vAddr, uint32_t physAddr, uint8_t flags);
void zeroPage(uint32_t vAddr);

struct KallocHeader* heapStart = NULL;
uint32_t nextVAddrToMap = 0xC1000000;

bool vmmAddPage(uint32_t vAddr, bool shouldZero) {
    bool result = vmmAllocatePage(vAddr, pmmAllocNextFreePage(), VMM_WRITABLE);
    if (shouldZero && result) {
        zeroPage(vAddr);
        // kprint_hex(*(uint32_t*) (vAddr + 10));

        return true;
    } else {
        return result;
    }
}

// kernel allocation, for drivers and such
void* kalloc(uint32_t numBytes) {
    if (heapStart == NULL) {
        vmmAddPage(nextVAddrToMap, false);
        struct KallocHeader* head = (struct KallocHeader*)nextVAddrToMap;
        head->size = 4096-sizeof(struct KallocHeader);
        head->isFree = true;
        head->next = NULL;
        heapStart = head;
    } 
    struct KallocHeader* current = heapStart;
    while (current != NULL) {
        if (!current->isFree || current->size < numBytes) {
            // the current segment isn't going to work for the size we're trying to allocate
            if (current->next == NULL) {
                // if there is no next one and we couldn't find a good one before, create a new next one and allocate memory to it
                uint32_t nextAddr = (uint32_t) current;
                nextAddr += sizeof(struct KallocHeader);
                nextAddr += current->size;
                
                // kprint("kalloc addr start: \n");
                // kprint_hex(nextAddr);
                // kprint("\nKalloc addr end; \n");
                // kprint_hex(nextAddr + sizeof(struct KallocHeader) + numBytes);
                // kprint("\nkalloc header size:");
                // kprint_hex(sizeof(struct KallocHeader));
                // kprint("\nsize\n");
                // kprint_hex(numBytes);

                // check if there is enough memory free to allocate pages between nextAddr and the end of this hypothetical kalloc
                for (int i = ALIGN_DOWN(nextAddr); i < ALIGN_UP((nextAddr + sizeof(struct KallocHeader) + numBytes)); i+=PAGE_SIZE) {
                    // kprint("Allocating page (k): ");
                    // kprint_hex(i);
                    kprint("\n");
                    if (!vmmAddPage(i, false)) {
                        kprint("OUT OF MEMORY \n");
                        return NULL;
                    }
                }
                struct KallocHeader* nextHeader = (struct KallocHeader*)nextAddr;
                *nextHeader = (struct KallocHeader) {
                    .size = numBytes,
                    .isFree = false,
                    .next = NULL
                };
                current->next = nextHeader;
                nextAddr += sizeof(struct KallocHeader);
                return (void*)nextAddr;
            }
        } else {
            // we find a segment that can fit
            uint32_t kallocSegmentToReturn = (uint32_t) current;
            current->isFree = false;
            kallocSegmentToReturn += sizeof(struct KallocHeader);
            return (void*) kallocSegmentToReturn;
        }
        current = current->next;
    }
}

void free(void* ptr) {
    if (ptr == NULL) return;

    uint32_t addr = (uint32_t) ptr;
    addr -= sizeof(struct KallocHeader);
    struct KallocHeader* kh = (struct KallocHeader*)addr;
    kh->isFree = true;
    while (kh->next != NULL && kh->next->isFree) {
        kh->size += kh->next->size + sizeof(struct KallocHeader);
        kh->next = kh->next->next;
    }
}

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

void zeroPage(uint32_t vAddr) {
    vAddr &= 0b11111111111111111111000000000000;
    uint8_t* pageBytes = (uint8_t*) vAddr;
    for (int i = 0; i < 4096; i++) {
        pageBytes[i] = 0x00;
    }
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
            // if (shouldZero) {
            //     vAddr &= 0b11111111111111111111000000000000;
            //     uint8_t* pageBytes = (uint8_t*) vAddr;
            //     for (int i = 0; i < 4096; i++) {
            //         pageBytes[i] = 0x00;
            //     }
            // }

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
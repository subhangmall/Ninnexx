// paging

// SETUP E820 MAPPING, FIX FIRST 4MB MAPPING

#include <stdint.h>
#include <stdbool.h>
#include <kernel/logging.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/e820.h>
#include <kernel/memory/vmm.h>
#include <kernel.h>
#include <kernel/memory.h>

void* kalloc(uint32_t numBytes);
void free(void* ptr);

// #pragma pack(push, 1)

struct KallocHeader {
    uint32_t size;
    bool isFree;
    struct KallocHeader* next;
} __attribute__((packed)); 

// #pragma pack(pop)

struct KallocHeader* heapStart = NULL;
uint32_t nextVAddrToMap = 0xC1000000;

// kernel allocation, for drivers and such
void* kalloc(uint32_t numBytes) {
    if (heapStart == NULL) {
        vmmAddPage(nextVAddrToMap, false, VMM_WRITABLE);
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

                // check if there is enough memory free to allocate pages between nextAddr and the end of this hypothetical kalloc
                for (int i = ALIGN_DOWN(nextAddr); i < ALIGN_UP((nextAddr + sizeof(struct KallocHeader) + numBytes)); i+=PAGE_SIZE) {
                    // kprint("Allocating page (k): ");
                    // kprint_hex(i);
                    kprint("\n");
                    if (!vmmAddPage(i, false, VMM_WRITABLE)) {
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
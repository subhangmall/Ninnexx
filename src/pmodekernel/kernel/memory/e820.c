#include "./pmm.h"
#include <stdint.h>
#include "../logging.h"
#include "./commonMacros.h"

#pragma pack(push, 1)

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

extern uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8]; 

static struct UsableRegion usableRegions[MAX_REGIONS];
static uint8_t usableRegionsCurrentLength = 0;

void sortRegions() {
    for (int i = 0; i < usableRegionsCurrentLength; i++) {
        for (int j = 0; j < usableRegionsCurrentLength -i - 1; j++) {
            if (usableRegions[j].base > usableRegions[j+1].base) {
                struct UsableRegion tmp = usableRegions[j];
                usableRegions[j] = usableRegions[j+1];
                usableRegions[j+1] = tmp;
            }
        }
    }
}

void mergeRegions() {
    for (int i = 0; i < usableRegionsCurrentLength - 1; i++) {

        uint64_t currentEnd = usableRegions[i].base + usableRegions[i].length;

        uint64_t nextEnd = usableRegions[i+1].base + usableRegions[i+1].length;

        if (currentEnd >= usableRegions[i+1].base) {
            uint64_t newEnd = currentEnd > nextEnd ? currentEnd : nextEnd;

            usableRegions[i].length = newEnd - usableRegions[i].base;

            for (int j = i + 1; j < usableRegionsCurrentLength - 1; j++) {
                usableRegions[j] = usableRegions[j+1];
            }

            usableRegionsCurrentLength--;
            i--;  // re check merged region
        }
    }
}

void parseE820Output() {
    // setup frame allocator
    for (uint32_t i = 0; i < 0xFFFFFFFF/PAGE_SIZE/8; i++) {
        physicalPageRecord[i] = 0xFF; // means all 8 bits are currently unavailable
    }

    uint16_t* listLength = (uint16_t*) 0x00008000; // where the length of the list is stored by the assembler;
    // 0x5000 is where the entries are stored, each 24 bytes

    // kprint("List length: ");
    // kprint_hex((uint32_t) *listLength);
    // kprint("\n");

    for (uint32_t i = 0; i < *listLength; i++) {
        if (usableRegionsCurrentLength == MAX_REGIONS) {
            break;
        }
        
        struct E280Entry* entry = (struct E280Entry*) (i * 24 + 0x5000);

        if (entry->type == AVAILABLE_MEM_E820) {
            struct UsableRegion region = {
                .base = entry->baseAddr,
                .length = entry->len
            };

            usableRegions[usableRegionsCurrentLength++] = region;
        }
    }

    // for (int i = 0; i < usableRegionsCurrentLength; i++) {
    //     kprint("Start range unsorted: "); 
    //     kprint_hex64(usableRegions[i].base);
    //     kprint("\nEnd range unsorted: ");
    //     kprint_hex64(usableRegions[i].length + usableRegions[i].base);
    //     // kprint("\n");
    // }

    // kprint("\n");

    sortRegions();

    // for (int i = 0; i < usableRegionsCurrentLength; i++) {
    //     kprint("Start range sorted: "); 
    //     kprint_hex64(usableRegions[i].base);
    //     kprint("\nEnd range sorted: ");
    //     kprint_hex64(usableRegions[i].length + usableRegions[i].base);
    //     kprint("\n");
    // }

    // kprint("\n");

    mergeRegions();

    for (int i = 0; i < usableRegionsCurrentLength; i++) {
        kprint("Start range: "); 
        kprint_hex64(usableRegions[i].base);
        kprint("\nEnd range: ");
        kprint_hex64(usableRegions[i].length + usableRegions[i].base);
        kprint("\n");
    }

    // kprint("Usable list length: ");
    // kprint_hex(usableRegionsCurrentLength);
    // kprint("\n");

    for (int i = 0; i < usableRegionsCurrentLength; i++) {
        for (int j = usableRegions[i].base | 0x00000FFF; j < ((usableRegions[i].base + usableRegions[i].length - 1) & 0xFFFFF000); j+=4096) {
            pmmSet(j, PMM_AVAILABLE);
        }
    }
}

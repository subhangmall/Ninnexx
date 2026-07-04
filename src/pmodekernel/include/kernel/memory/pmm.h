#define PAGE_SIZE 4096
#include <stdint.h>

#ifndef PMM_H
#define PMM_H

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

extern uint8_t physicalPageRecord[0xFFFFFFFF/PAGE_SIZE/8];
extern bool shouldTrack;
void pmmSet(uint32_t physAddr, bool val);
bool pmmGet(uint32_t physAddr);
uint32_t pmmAllocNextFreePage();
void premove(uint32_t vAddr);
uint32_t virtToPhysAddr(uint32_t vAddr);
bool addPage(uint32_t vAddr, bool shouldZero);
bool createNewPageTable(uint32_t vAddr);


#endif
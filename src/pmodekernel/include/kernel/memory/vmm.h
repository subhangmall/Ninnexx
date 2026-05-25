#include <stdint.h>
#include <stdbool.h>

#ifndef VMM_H
#define VMM_H

bool vmmAllocatePage(uint32_t vAddr, uint32_t physAddr, uint8_t flags);
bool vmmAddPage(uint32_t vAddr, bool shouldZero, uint8_t flags);
void vmmZeroPage(uint32_t vAddr);
void* vmmAllocatePhysicalRange(uint32_t physAddr, uint32_t len);
void vmmDeallocatePhysicalRange(uint32_t vAddr, uint32_t len);
void vmmRemovePage(uint32_t vAddr);

#endif
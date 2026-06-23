#include <stdint.h>

#ifndef IDT_H
#define IDT_H

#pragma pack(push, 1) 

struct InterruptDescriptor {
    uint16_t offset;
    uint16_t segSelector;
    uint8_t reserved;
    uint8_t gateType : 4;
    uint8_t zero : 1;
    uint8_t dpl : 2;
    uint8_t present : 1; 
    uint16_t offset2;
};

struct IdtrStruct {
    uint16_t size;
    uint32_t idtVAddr;
};

#pragma pack(pop)

void setupInterruptStructures();
void enableInterrupts();
void disableInterrupts();
void loadIDTR();
void setIDTHandler(int isrNum, int handlerAddr);
extern struct IdtrStruct idtr __attribute__((aligned(16)));
extern struct InterruptDescriptor idt[256] __attribute__((aligned(16)));
extern uint32_t dispatcherFunAddrs[256];

#endif
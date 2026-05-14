#include <stdint.h>
#include "../logging.h"

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

struct IdtrStruct idtr __attribute__((aligned(16)));
struct InterruptDescriptor idt[256] __attribute__((aligned(16)));
// void (*interruptRequestHandlers[256])(uint32_t*);
uint32_t dispatcherFunAddrs[256];

extern void int0(void);

void setInterruptDispatcher(uint8_t intNum, uint32_t addr, uint16_t segSelector, uint8_t gateType, uint8_t dpl);

void setupInterruptStructures() {
    dispatcherFunAddrs[0] = (uint32_t) int0;

    for (int i = 0; i < 256; i++) {
        setInterruptDispatcher(i, dispatcherFunAddrs[i], 0x08, 0xE, 0x00);
    }

    idtr = (struct IdtrStruct) {.size = (256 * sizeof(struct InterruptDescriptor) - 1), .idtVAddr = (uint32_t)&idt};
}

void setInterruptDispatcher(uint8_t intNum, uint32_t addr, uint16_t segSelector, uint8_t gateType, uint8_t dpl) {
    struct InterruptDescriptor intDesc = {
        .offset = (uint16_t)(addr & 0xFFFF),
        .segSelector = segSelector,
        .reserved = 0x00,
        .gateType = gateType,
        .zero = 0b0,
        .dpl = dpl,
        .present = 1,
        .offset2 = (uint16_t)((addr & 0xFFFF0000) >> 16)
    };

    idt[intNum] = intDesc;
}

void commonHandler(uint32_t* stack) {
    // interruptRequestHandlers[stack[12]](stack);
    kprint("Hi");
    while(1) {}
}

// void setInterruptHandlerFunction() {

// }

void enableInterrupts() {
    asm volatile (
        "lidt %0\n\t"
        "sti"
        :
        : "m" (idtr)
        : 
    );
}
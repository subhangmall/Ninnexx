#include <stdint.h>
#include <kernel/interrupts/intrStructs.h>
#ifndef PIC_H
#define PIC_H

void initPIC(uint8_t mOffset, uint8_t sOffset);
void disablePIC(void);
void defaultPICHandler(struct InterruptStackFrame* stack);
void enablePIC(void);
bool linkIRQHandler(uint8_t irqNum, uint32_t address);
void sendEOIToPIC(uint8_t irq);
uint8_t intNumToIRQNum(uint8_t intNum);

#endif
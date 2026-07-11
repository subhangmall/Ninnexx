#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/process.h>

#ifndef CONTEXTSWITCH_H
#define CONTEXTSWITCH_H

void contextSwitch(struct InterruptStackFrame* isf);
void zombieContextSwitch();
void switchMemoryContext(uint32_t addr);

#endif
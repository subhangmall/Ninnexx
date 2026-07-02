#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/process.h>

#ifndef CONTEXTSWITCH_H
#define CONTEXTSWITCH_H

void contextSwitch(struct InterruptStackFrame* isf, struct Process* next);

#endif
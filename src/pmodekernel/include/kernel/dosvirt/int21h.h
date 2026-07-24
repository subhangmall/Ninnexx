#ifndef INT21H_H
#define INT21H_H

#include <kernel/interrupts/intrStructs.h>

void intr21(struct InterruptStackFrame* isf);

#endif
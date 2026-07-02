#include <kernel/interrupts/intrStructs.h>


#ifndef YIELD_H
#define YIELD_H

void yield(struct InterruptStackFrame* isf);

#endif
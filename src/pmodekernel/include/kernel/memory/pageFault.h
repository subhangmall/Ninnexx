#include <kernel/interrupts/intrStructs.h>

#ifndef PAGEFAULT_H
#define PAGEFAULT_H

void pageFault(struct InterruptStackFrame* isf);

#endif
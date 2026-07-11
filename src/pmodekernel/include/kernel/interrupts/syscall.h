#include <kernel/interrupts/intrStructs.h>

#ifndef SYSCALL_H
#define SYSCALL_H

void sysIntr(struct InterruptStackFrame* isf);

#endif
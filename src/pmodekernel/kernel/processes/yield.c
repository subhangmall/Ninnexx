// INT 0XFF
#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/process.h>
#include <kernel/processes/contextSwitch.h>
#include <stdio.h>

void yield(struct InterruptStackFrame* isf) {
    contextSwitch(isf);
}
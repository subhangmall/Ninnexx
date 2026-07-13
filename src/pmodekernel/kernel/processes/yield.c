// INT 0XFF
#include <kernel/interrupts/intrStructs.h>
#include <kernel/processes/process.h>
#include <kernel/processes/contextSwitch.h>
#include <stdio.h>

void yield(struct InterruptStackFrame* isf) {
    if (isf->eax != 1) {
        contextSwitch(isf);
    } else {
        struct Process* iter = (struct Process*) &procHead;
        while (iter->procID != isf->ebx) {
            if ((uint32_t) iter->next == (uint32_t) &procHead) {
                // process not found
                isf->eflags |= 1;
                isf->eax = 3;
                return;
            }
            iter = iter->next;
        }
        contextSwitchTo(isf, iter);
    }
    
}
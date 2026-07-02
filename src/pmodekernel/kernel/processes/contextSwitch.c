#include <kernel/processes/process.h>
#include <kernel/interrupts/intrStructs.h>
#include <stdio.h>

extern struct Process* current;
extern struct Process procHead;
extern void context_switch_noret(void);
extern uint32_t tss[];

void contextSwitch(struct InterruptStackFrame* isf, struct Process* next) {
    // printf("yield\n");
    current->kesp = (uint32_t) isf;
    current = next;
    tss[1] = current->krnlStackTop; // esp 0
    context_switch_noret();
}


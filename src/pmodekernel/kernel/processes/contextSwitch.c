#include <kernel/processes/process.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/memory.h>
#include <stdio.h>
#include <kernel/memory/pmm.h>

extern struct Process* current;
extern struct Process procHead;
extern void context_switch_noret(void);
extern uint32_t tss[];


void zombieContextSwitchTo(struct Process* next) {
    current = next;

    if (!current->kernel) {
        tss[1] = current->krnlStackTop; // esp 0
        asm volatile(
            "mov %0, %%eax\n\t"
            "mov %%eax, %%cr3\n\t"
            "jmp 1f\n\t"
            "1:"
            :
            : "r" (current->cr3)
            : "eax"
        );
    }

    // sync with global page directory
    for (uint32_t i = (0xC0000000 >> 22); i < (0xFFFFFFFF >> 22); i++) {
        KERNEL_PAGE_DIRECTORY[i] = PARENT_KPD[i];
    }
    

    asm volatile (
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %%cr3"
        :
        :
        : "eax"
    ); // refresh TLB

    context_switch_noret();
}

void zombieContextSwitch() {
    struct Process* iter = current;
    while (iter->next->zombie || iter->next->status != 0) { // next process is a zombie
        iter = iter->next;
    }
    zombieContextSwitchTo(iter->next);
}

void contextSwitch(struct InterruptStackFrame* isf) {
    // printf("yield\n");
    current->kesp = (uint32_t) isf;
    zombieContextSwitch();
}

void contextSwitchTo(struct InterruptStackFrame* isf, struct Process* next) {
    current->kesp = (uint32_t) isf;
    zombieContextSwitchTo(next);
}

void switchMemoryContext(uint32_t addr) {
    asm volatile(
            "mov %0, %%eax\n\t"
            "mov %%eax, %%cr3\n\t"
            "jmp 1f\n\t"
            "1:"
            :
            : "r" (addr)
            : "eax"
    );
    // sync with global page directory
    for (uint32_t i = (0xC0000000 >> 22); i < (0xFFFFFFFF >> 22); i++) {
        KERNEL_PAGE_DIRECTORY[i] = PARENT_KPD[i];
    }
}

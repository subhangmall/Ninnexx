#include <kernel/processes/process.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

struct Process* current;
struct Process procHead;

uint32_t currentProcID = 0;

uint32_t kernelStackAllocationZoneTop = 0xB0000000;

uint32_t createNewProcess(bool kernel, bool v8086, uint32_t cr3, uint32_t kernelStackTop, uint32_t startEip, uint32_t usrEspIfNeeded) {
    struct InterruptStackFrame* isf = (struct InterruptStackFrame*) malloc(sizeof(struct InterruptStackFrameFromUser));
    struct Process* new = (struct Process*) malloc(sizeof(struct Process));

    new->procID = ++currentProcID;
    new->kernel = kernel;
    new->v8086 = v8086;
    new->next = &procHead;
    new->cr3 = cr3;
    if (kernel) {
        new->kesp = kernelStackTop-sizeof(struct InterruptStackFrame);
    } else {
        new->kesp = kernelStackTop-sizeof(struct InterruptStackFrameFromUser);
    }
    
    new->krnlStackTop = kernelStackTop;

    // printf("a");

    isf->eflags = 0x00000000;
    isf->eflags |= 1 << 1; // bit 1 always 1
    isf->eflags |= 1 << 9; // interrupts enabled
    if (v8086) {
        isf->eflags |= 1 << 17;
    }
    isf->eip = startEip;
    isf->edi = 0x00000000;
    isf->esi = 0x00000000;
    isf->ebp = 0x00000000; 
    isf->esp = 0x00000000; 
    isf->ebx = 0x00000000;
    isf->edx = 0x00000000;
    isf->ecx = 0x00000000;
    isf->eax = 0x00000000;
    isf->intNum = 0x00; // dont matter cause they wont ever be accessible to the program
    isf->errNum = 0x00; // dont matter cause they wont ever be accessible to the program

    if (kernel) {
        isf->cs = 0x08;
        isf->gs = 0x10;
        isf->fs = 0x10;
        isf->es = 0x10;
        isf->ds = 0x10;
    } else {
        isf->cs = 0x1B; // |= 0x03 b/c ring 3
        isf->gs = 0x23;
        isf->fs = 0x23;
        isf->es = 0x23;
        isf->ds = 0x23;
    }


    if (kernel) {
        memcpy((void*)(kernelStackTop - sizeof(struct InterruptStackFrame)), isf, sizeof(struct InterruptStackFrame));
    } else {
        struct InterruptStackFrameFromUser* isfu = (struct InterruptStackFrameFromUser*) isf; // STACK CORRUPTION???????????
        isfu->usrEsp = usrEspIfNeeded;
        isfu->usrSS = 0x23;
        memcpy((void*)(kernelStackTop - sizeof(struct InterruptStackFrameFromUser)), isfu, sizeof(struct InterruptStackFrameFromUser));
    }
    free(isf);

    struct Process* iterator = (struct Process*) &procHead;
    while ((uint32_t) iterator->next != (uint32_t) &procHead) {
        iterator = iterator->next;
    }

    // once we are at the last process
    iterator->next = (struct Process*)new;

    // process is now in the runqueue
    return new->procID;
}

uint32_t allocateKernelStack() {
    kernelStackAllocationZoneTop -= 0x3000;
    if (!vmmAddPage(kernelStackAllocationZoneTop + 0x2000, true, VMM_WRITABLE) || !vmmAddPage(kernelStackAllocationZoneTop + 0x1000, true, VMM_WRITABLE)) {
        return (uint32_t)NULL;
    }
    return kernelStackAllocationZoneTop + 0x2FFF;
}

void deleteProcess(uint32_t pid) {
    struct Process* iter;
    iter = (struct Process*) &procHead;
    
    while (iter->procID != pid) {
        iter = iter->next;
    }

    uint32_t procAddr = (uint32_t) iter;
    uint32_t next = (uint32_t) iter->next;
    

    iter = (struct Process*) &procHead;

    // find one before it
    while ((uint32_t)iter->next != procAddr) {
        iter = iter->next;
    }

    // set it's next to the next of the removed element
    iter->next = (struct Process*) next;

    while (current->procID == pid) {}
    free((void*)procAddr);
}
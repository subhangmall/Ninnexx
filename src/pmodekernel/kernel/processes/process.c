#include <kernel/processes/process.h>
#include <kernel/processes/yield.h>
#include <kernel/processes/contextSwitch.h>
#include <kernel/interrupts/intrStructs.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <kernel/memory/pmm.h>

bool procEnabled = false;

struct Process* current;
struct Process procHead;

uint32_t currentProcID = 0;

uint32_t kernelStackAllocationZone = 0xE0000000;
// uint32_t kernelPageDirectoryStorageBottom = 0xD0000000;

// uint32_t createNewProcess(bool kernel, bool v8086, uint32_t cr3, uint32_t kernelStackTop, uint32_t startEip, uint32_t usrEspIfNeeded) {
//     struct InterruptStackFrame* isf = (struct InterruptStackFrame*) malloc(sizeof(struct InterruptStackFrameFromUser));
//     struct Process* new = (struct Process*) malloc(sizeof(struct Process));

//     new->procID = ++currentProcID;
//     new->kernel = kernel;
//     new->v8086 = v8086;
//     new->next = &procHead;
//     new->cr3 = cr3;
//     new->zombie = false;
//     if (kernel) {
//         new->kesp = kernelStackTop-sizeof(struct InterruptStackFrame);
//     } else {
//         new->kesp = kernelStackTop-sizeof(struct InterruptStackFrameFromUser);
//     }
    
//     new->krnlStackTop = kernelStackTop;

//     isf->eflags = 0x00000000;
//     isf->eflags |= 1 << 1; // bit 1 always 1
//     isf->eflags |= 1 << 9; // interrupts enabled
//     if (v8086) {
//         isf->eflags |= 1 << 17;
//     }
//     isf->eip = startEip;
//     isf->edi = 0x00000000;
//     isf->esi = 0x00000000;
//     isf->ebp = 0x00000000; 
//     isf->esp = 0x00000000; 
//     isf->ebx = 0x00000000;
//     isf->edx = 0x00000000;
//     isf->ecx = 0x00000000;
//     isf->eax = 0x00000000;
//     isf->intNum = 0x00; // dont matter cause they wont ever be accessible to the program
//     isf->errNum = 0x00; // dont matter cause they wont ever be accessible to the program

//     if (kernel) {
//         isf->cs = 0x08;
//         isf->gs = 0x10;
//         isf->fs = 0x10;
//         isf->es = 0x10;
//         isf->ds = 0x10;
//     } else {
//         isf->cs = 0x1B; // |= 0x03 b/c ring 3
//         isf->gs = 0x23;
//         isf->fs = 0x23;
//         isf->es = 0x23;
//         isf->ds = 0x23;
//     }


//     if (kernel) {
//         memcpy((void*)(kernelStackTop - sizeof(struct InterruptStackFrame)), isf, sizeof(struct InterruptStackFrame));
//     } else {
//         struct InterruptStackFrameFromUser* isfu = (struct InterruptStackFrameFromUser*) isf;
//         isfu->usrEsp = usrEspIfNeeded;
//         isfu->usrSS = 0x23;
//         memcpy((void*)(kernelStackTop - sizeof(struct InterruptStackFrameFromUser)), isfu, sizeof(struct InterruptStackFrameFromUser));
//     }
//     free(isf);

//     struct Process* iterator = (struct Process*) &procHead;
//     while ((uint32_t) iterator->next != (uint32_t) &procHead) {
//         iterator = iterator->next;
//     }

//     // once we are at the last process
//     iterator->next = (struct Process*)new;

//     // process is now in the runqueue
//     return new->procID;
// }

uint32_t createNewProcess(bool kernel, bool v8086, uint32_t procStackDirStruct, uint32_t startEip, uint32_t usrEspIfNeeded) {
    struct InterruptStackFrame* isf = (struct InterruptStackFrame*) malloc(sizeof(struct InterruptStackFrameFromUser));
    struct Process* new = (struct Process*) malloc(sizeof(struct Process));

    new->procID = ++currentProcID;
    new->kernel = kernel;
    new->v8086 = v8086;
    new->next = &procHead;
    new->cr3 = virtToPhysAddr(procStackDirStruct);
    uint32_t kernelStackTop = procStackDirStruct + 0x3FFF;

    new->zombie = false;
    if (kernel) {
        new->kesp = kernelStackTop-sizeof(struct InterruptStackFrame);
    } else {
        new->kesp = kernelStackTop-sizeof(struct InterruptStackFrameFromUser);
    }
    
    new->krnlStackTop = kernelStackTop;

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
        struct InterruptStackFrameFromUser* isfu = (struct InterruptStackFrameFromUser*) isf;
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

uint32_t createProcStackDirectoryStructure() {
    uint32_t eflags;
    asm volatile (
        "pushf\n\t"
        "pop %0\n\t"
        "cli"
        : "=r" (eflags)
        :
        : "memory"
    );
    kernelStackAllocationZone -= 0x4000;
    if (!vmmAddPage(kernelStackAllocationZone + 0x3000, false, VMM_WRITABLE) || !vmmAddPage(kernelStackAllocationZone + 0x2000, false, VMM_WRITABLE) || !vmmAddPage(kernelStackAllocationZone, false, VMM_WRITABLE)) {
        if (eflags & (1 << 9)) asm volatile ("sti");
        // printf("err");
        return (uint32_t)NULL;
    }

    struct PageDirectoryEntry* pde = (struct PageDirectoryEntry*)(kernelStackAllocationZone);

    memset(pde, 0, 4096);
    for (uint32_t i = (0xC0000000 >> 22); i <= (0xFFFFFFFF >> 22); i++) {
        pde[i] = PARENT_KPD[i];
    }
    pde[0xFFFFFFFF >> 22].pageAddress = (virtToPhysAddr(kernelStackAllocationZone) >> 12);

    if (eflags & (1 << 9)) asm volatile ("sti");
    printf("%X", kernelStackAllocationZone);
    return kernelStackAllocationZone;
}

void deleteProcess(uint32_t pid) {
    uint32_t eflags;
    asm volatile (
        "pushf\n\t"
        "pop %0\n\t"
        "cli"
        : "=r" (eflags)
        :
        : "memory"
    );
    if (pid==0) return; // cannot end swapper process
    if (current->procID != pid) {
        struct Process* iter = (struct Process*) &procHead;
        while (iter->next->procID != pid) {
            iter = iter->next;
        }
        struct Process* structToFree = iter->next;
        iter->next = iter->next->next;
        free(structToFree);
    } else {
        current->zombie = true;
        if (eflags & (1 << 9)) asm volatile ("sti");
        zombieContextSwitch();
    }
    if (eflags & (1 << 9)) asm volatile ("sti");
}

uint32_t createNewPageDirectoryTable() {
    
}
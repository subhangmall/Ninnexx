#include <kernel/processes/process.h>
#include <kernel/interrupts/intrStructs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Process* current;
struct Process procHead;

uint32_t currentProcID = 0;

uint32_t createNewProcess(bool kernel, bool v8086, uint32_t cr3, uint32_t kernelStackTop, uint32_t startEip, uint32_t usrEspIfNeeded) {
    // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
     // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
      // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
       // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
        // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
         // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
          // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
           // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
            // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
             // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
              // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
               // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                 // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                  // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                   // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                    // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                     // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                      // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                       // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                        // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                         // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                          // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!
                           // NOTE: KERNEL-> USER ALSO PUSHES USER ESP/SS!!!!!!!!!

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

void deleteProcess() {

}
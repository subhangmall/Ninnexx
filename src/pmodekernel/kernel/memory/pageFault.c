#include <kernel/interrupts/intrStructs.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory.h>
#include <kernel/processes/process.h>

void pageFault(struct InterruptStackFrame* isf) {
    bool pageAccessedPresent = false;
    bool write = false; // if false read
    bool userProgram = false; // if false kernel

    if (isf->errNum & 0b001) {
        pageAccessedPresent = true;
    }

    if (isf->errNum & 0b010) {
        write = true;
    }

    if (isf->errNum & 0b100) {
        userProgram = true;
    }



    uint32_t addressTriedToAccess;
    asm volatile (
        "mov %%cr2, %%eax\n\t"
        "mov %%eax, %0\n\t"
        : "=m" (addressTriedToAccess)
        : 
        :
    );
    
    if (userProgram && addressTriedToAccess < 0xC0000000 && !pageAccessedPresent) {
        // user code, just wanbts more memory :DDDDDDDDDDDDDDDDDDDDDDDD
        printf("Adding more memory for user program!\n");
        vmmAddPage(addressTriedToAccess, true, VMM_WRITABLE);
        return;
    }

    if (userProgram && pageAccessedPresent) {
        printf("Program committed an illegal memory operation and will now be terminated!\n");
        deleteProcess(current->procID);
        return;
    }

    // if (!userProgram && !pageAccessedPresent && )
    printf("Page Fault while accessing %X!\n", addressTriedToAccess);
    asm volatile ("hlt");
}
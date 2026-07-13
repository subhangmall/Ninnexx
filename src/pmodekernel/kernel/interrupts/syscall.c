#include <kernel/interrupts/intrStructs.h>
#include <kernel/logging.h>
#include <stdio.h>
#include <kernel/genericDrivers/ps2keyboard.h>
#include <kernel/processes/process.h>
#include <kernel/interrupts/pic.h>
#include <kernel/io/iolibrary.h>
#include <stdint.h>

extern bool gettingInput;
extern uint32_t bufferAddr;
extern uint32_t pidOccupying;

extern struct Process* current;

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

/*
error codes: 
1 -> privelege level error
2 -> resource temporarily occupied
3 -> process not available
*/

void sysIntr(struct InterruptStackFrame* isf) {
    switch (isf->eax) {
        case 0x00: // print
            if (isf->ebx + 1 >= 0xC0000000) {
                // address in kernel space!!!!!!
                isf->eflags |= 1; // cf enabled; error
                isf->eax = 0x00000001; // privelege access error
                return;
            }
            printf((char*)isf->ebx);
            break;
        case 0x01: // print char
            kputc((char)isf->ebx);
            break;
        case 0x02: // read char
            current->status = 1;
            asm volatile (
                "mov $0, %%eax\n\t"
                "int $0xFF"
                :
                : 
                : "eax", "memory"
            );
            current->status = 0;
            isf->eax = getCharFromEvent(buffer[curIdx]);
            // printf("eax: %X, curIDX: %X", isf->eax, curIdx);
            break;
        
    }
}
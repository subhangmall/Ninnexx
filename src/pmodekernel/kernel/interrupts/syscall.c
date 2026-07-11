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
            // if (isf->ebx >= 0xC0000000) { // buffer address
            //     isf->eflags |= 1; // cf enabled; error
            //     isf->eax = 0x00000001; // privelege access error
            //     return;
            // }
            if (gettingInput) {
                isf->eflags |= 1; // cf enabled; error
                isf->eax = 0x00000002; // resource error
                // printf("sorry bozo");
                return;
            }
            gettingInput = true;
            bufferAddr = isf->ebx;
            pidOccupying = current->procID;

            // enable interrupts and only unmask the keyboard interrupts
            outb(PIC1_DATA, 0b11111101);
            outb(PIC2_DATA, 0b11111111);
            asm volatile("sti");

            while (gettingInput) {} // wait for buffer to fill
            printf("waiting");

            asm volatile("cli");
            enablePIC();

            break;
        
    }
}
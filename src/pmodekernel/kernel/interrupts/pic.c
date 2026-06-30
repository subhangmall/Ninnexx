// THANKS TO THE OS DEV WIKI for help in programming the PIC init, disable, and eoi functions
#include <stdint.h>
#include <kernel/io/iolibrary.h>
#include <kernel/logging.h>
#include <kernel/interrupts/idt.h>
#include <kernel/interrupts/intrStructs.h>
#include <stdio.h>

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI 0x20

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define CASCADE_IRQ 2

uint8_t masterOffset;
uint8_t slaveOffset;

void defaultPICHandler(struct InterruptStackFrame* stack);
uint8_t irqNumToIntNum(uint8_t irqNum);

bool linkIRQHandler(uint8_t irqNum, uint32_t address) {
	if (irqNumToIntNum(irqNum) == 0x00) {
		return false;
	}
	setIDTHandler(irqNumToIntNum(irqNum), address);
}

void initPIC(uint8_t mOffset, uint8_t sOffset) {
	masterOffset = mOffset;
	slaveOffset = sOffset;

	// kprint("INIT PIC SLAVE OFFSET");
	// kprint_hex(slaveOffset);

	for (int i = 0; i < 16; i++) {
		linkIRQHandler(i, (uint32_t) defaultPICHandler);
	}

	// for (int i = masterOffset; i < masterOffset+8; i++) {
	// 	setIDTHandler(i, (uint32_t) defaultPICHandler);
	// }

	// for (int i = slaveOffset; i < slaveOffset+8; i++) {
	// 	setIDTHandler(i, (uint32_t) defaultPICHandler);
	// }

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	ioWait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	ioWait();
	outb(PIC1_DATA, masterOffset);                 // ICW2: Master PIC vector offset
	ioWait();
	outb(PIC2_DATA, slaveOffset);                 // ICW2: Slave PIC vector offset
	ioWait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	ioWait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	ioWait();
	
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	ioWait();
	outb(PIC2_DATA, ICW4_8086);
	ioWait();

	// Unmask both PICs.
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void disablePIC(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void enablePIC(void) {
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void sendEOIToPIC(uint8_t irq) {
	if (irq >= 8)
		outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

uint8_t intNumToIRQNum(uint8_t intNum) {
	if (intNum >= masterOffset && intNum < masterOffset + 8) {
		return intNum - masterOffset;
	} else if (intNum >= slaveOffset && intNum < slaveOffset + 8) {
		return intNum - slaveOffset + 8;
	} else {
		return 0xFF; // error code
	}
}

uint8_t irqNumToIntNum(uint8_t irqNum) {
	if (irqNum > 0xFF) return 0x00; // error code
	
	if (irqNum >= 8) {
		return slaveOffset + irqNum - 8;
	} else {
		return masterOffset + irqNum;
	}
}

void defaultPICHandler(struct InterruptStackFrame* stack) {
	printf("Uninitialized PIC interrupt %H\n", intNumToIRQNum(stack->intNum));

	sendEOIToPIC(intNumToIRQNum(stack->intNum));
    return;
}
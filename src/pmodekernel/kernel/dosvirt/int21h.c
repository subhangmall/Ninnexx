#include <kernel/interrupts/intrStructs.h>
#include <kernel/logging.h>

void intr21(struct InterruptStackFrame* isf) {
    if (!(isf->eflags & (1 << 17))) {
        return; // not VM
    }

    struct InterruptStackFrameV8086* isfv = (struct InterruptStackFrameV8086*) isf;

    if (((isf->eax & 0x0000FF00) == 0x0200)) {
        // write character in dl to scren
        kputc((uint8_t)isfv->edx);
    } else if ((isfv->eax & 0x0000FF00) == 0x0900) {
        // print $ terminated string ds:dl
        uint32_t addr = 16*(isfv->dsHardware) + (0x0000FFFF & isfv->edx);
        while (*(char*)addr != '$') {
            kputc(*(char*)(addr++));
        }
    }

    
}
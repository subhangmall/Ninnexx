#include <stdint.h>

#ifndef INTRSTRUCT_H
#define INTRSTRUCT_H

#pragma pack(push, 1)

struct InterruptStackFrameFromUser {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t intNum;
    uint32_t errNum;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t usrEsp;
    uint32_t usrSS;
};

struct InterruptStackFrame {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t intNum;
    uint32_t errNum;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct InterruptStackFrameV8086 {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t intNum;
    uint32_t errNum;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t usrEsp;
    uint32_t usrSS;
    uint32_t esHardware;
    uint32_t dsHardware;
    uint32_t fsHardware;
    uint32_t gsHardware;
};
#pragma pack(pop)

#endif
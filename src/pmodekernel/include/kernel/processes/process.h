#include <stdbool.h>
#include <stdint.h>
#include <kernel/interrupts/intrStructs.h>

#ifndef PROCESS_H
#define PROCESS_H

struct Process {
    uint32_t procID;

    // runqueue
    struct ProcessAttributeStruct* next;

    // flags
    bool kernel;
    bool v8086;

    // integral information per process
    uint32_t cr3; // pointer to page table
    uint32_t kesp; // kernel stack 
    uint32_t krnlStacktop;
} __attribute__((packed));

#endif
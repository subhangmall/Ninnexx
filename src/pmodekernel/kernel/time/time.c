#include <kernel/io/iolibrary.h>
#include <kernel/time/pitIntr.h>
#include <kernel/interrupts/pic.h>
#include <kernel/logging.h>

uint32_t freq;
volatile uint32_t ticks = 0; 

// credit to: https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/08_Timers.md for this funcition
void setPitPeriodic(uint32_t count) {
    outb(0x43, 0b00110100);
    outb(0x40, count & 0xFF); //low-byte
    outb(0x40, count >> 8); //high-byte
    freq = 1193182/count;
}

void initTimeIntrHandler() {
    linkIRQHandler(0, (uint32_t) pitIntr);
}

// __attribute__((aligned(4096)))
void sleep(uint32_t ms) {
    uint32_t startingTime = ticks;
    uint32_t targetTime = (ms * freq)/1000 + startingTime;
    // kprint_hex(startingTime);
    while (ticks < targetTime) {
        asm volatile("pause" ::: "memory");
    };
}
#include <stdint.h>
#include <kernel/processes/atlock.h>

void acquireLock(atlock* l) {
    asm volatile (
        "mov %1, %%ah\n\t"
        "mov %1, %%al\n\t"
        "loop:\n\t"
        "xchgb %%ah, %0\n\t"
        "cmp %%ah, %%al\n\t"
        "pause\n\t"
        "je loop\n\t"
        : "+m" (*l)
        : "i" (ATLOCK_LOCKED)
        : "eax", "memory"
    );
}

void releaseLock(atlock* l) {
    asm volatile("" ::: "memory");
    *l = ATLOCK_UNLOCKED;
}

void acquireLockIRQ(atlock* l, uint32_t* flags) {
    asm volatile (
        "pushf\n\t"
        "pop %1\n\t" 
        "cli\n\t"
        "mov %2, %%ah\n\t"
        "mov %2, %%al\n\t"
        "loopi:\n\t"
        "xchgb %%ah, %0\n\t"
        "cmp %%ah, %%al\n\t"
        "pause\n\t"
        "je loopi\n\t"
        : "+m" (*l), "=r" (flags)
        : "i" (ATLOCK_LOCKED)
        : "eax", "memory"
    );
}

void releaseLockIRQ(atlock* l, uint32_t* flags) {
    asm volatile("" ::: "memory");
    *l = ATLOCK_UNLOCKED;
    if (*flags & (1<<9)) {
        asm volatile("sti");
    }
}
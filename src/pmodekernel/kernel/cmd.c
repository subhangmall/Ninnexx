// a command prompt in kernel mode (i know peak cybersecurity!!) cause i dont wanna implement all the bs user space interrupts

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/logging.h>
#include <kernel/time/time.h>
#include <kernel.h>

int getCmdAndParse(char* cmd);

void cmd() {
    char* cmd = (char*) calloc(100, sizeof(char));
    printf("Welcome to Ninnexx!\n");
    while (1) {
        printf("\nCommand Parser: ");
        uint32_t a = getCmdAndParse(cmd);
        if (strncmp(cmd, "clear", 5) == 0) {
            kclear();
        } else {
            printf("\nCommand not found!");
        }
        memset(cmd, '\0', 100);
    }
}

int getCmdAndParse(char* cmd) {
    uint32_t idx = 0;
    while (idx < 100) {
        asm volatile (
            "startOfLoop: \n\t"
            "xor %%eax, %%eax\n\t"
            "xor %%ebx, %%ebx\n\t"
            "mov $0x02, %%eax\n\t"
            "int $0x80\n\t"
            "jc startOfLoop\n\t"
            "movb %%al, %0\n\t"
            : "+m" (cmd[idx])
            : 
            : "eax", "ebx", "memory"
        );
        if (cmd[idx] == '\n') {
            idx++;
            break;
        }
        kputc(cmd[idx]);
        idx++;
    } 
    return idx;
}
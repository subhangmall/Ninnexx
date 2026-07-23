// a command prompt in kernel mode (i know peak cybersecurity!!) cause i dont wanna implement all the bs user space interrupts

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/logging.h>
#include <kernel/time/time.h>
#include <kernel.h>
#include <kernel/fskernelapi/loadfile.h>
#include <kernel/processes/process.h>
#include <kernel/memory.h>
#include <kernel/memory/pmm.h>


int getCmdAndParse(char* cmd);

extern struct Process* current;

void cmd() {
    char* cmd = (char*) calloc(100, sizeof(char));
    printf("Welcome to Ninnexx!\n");
    while (1) {
        printf("\nCommand Parser: ");
        uint32_t len = getCmdAndParse(cmd);
        if (strncmp(cmd, "clear", 5) == 0) {
            kclear();
        } else if (strncmp(cmd, "lc", 2) == 0) {
            if (len < 4) {
                printf("\nPlease list a file!\n");
                continue;
            }
            uint32_t procStackStruct = createProcStackDirectoryStructure();
            uint32_t result = loadFile((char*)(((uint32_t)cmd)+3), virtToPhysAddr((uint32_t)procStackStruct), 0x0000100);
            if (result == 1) {
                printf("\nFile not found!\n");
                continue;
            } else if (result == 2) {
                printf("\nMemory allocation error!\n");
                continue;
            } else if (result == 3) {
                printf("\nFailed to read file!\n");
                continue;
            }
            uint32_t procID = createNewProcess(false, true, procStackStruct, 0x0100, 0xFFFE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
        } else {
            printf("\nCommand not found!\n");
        }
        memset(cmd, '\0', 100);
    }

    while (1) {}
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
            cmd[idx] = '\0';
            idx++;
            break;
        }
        kputc(cmd[idx]);
        idx++;
    } 
    return idx;
}
// /*

// The method suggested in the ATA specs for sending ATA commands tells you to check the BSY and DRQ bits before trying to send a command. This means that you need to read a Status Register (Alternate Status is a good choice) for the proper drive before sending the next command. Which means that you need to select the correct device first, before you can read that status (and then send all the other values to the other IO ports). Which means that a drive select may always happen just before a status read. This is bad. Many drives require a little time to respond to a "select", and push their status onto the bus. The suggestion is to read the Status register FIFTEEN TIMES, and only pay attention to the value returned by the last one -- after selecting a new master or slave device. The point being that you can assume an IO port read takes at least 30ns, so doing the first fourteen creates a 420ns delay -- which allows the drive time to push the correct voltages onto the bus. 

// driver always needs to block if the current device is actively modifying BSY/DRQ/ERR, and your device driver always already knows that the device is in that condition (because the driver just sent the command to the device, and it hasn't been marked "complete" yet). 


// once a drive has actually completed a command, it will always clear BSY and DRQ. You can simply verify this, before your next Device Select command -- that the previously selected device cleared BSY and DRQ properly at command completion. Then you will never have to check if they are clear after a Device Select -- so you will not have to read the Status Register after the Device Select at all.

// On some drives it is necessary to "manually" flush the hardware write cache after every write command. This is done by sending the 0xE7 command to the Command Register (then waiting for BSY to clear). If a driver does not do this, then subsequent write commands can fail invisibly, or "temporary bad sectors" can be created on your disk. 

// BSY and DRQ will be cleared, but ERR or DF remain set until just after you write a new command to the Command Register. If you are using polling (see below), you should account for the fact that your first four reads of the Status Register, after sending your command byte, may have the ERR or DF bits still set accidentally.
// */

// //!!!!!!!!!!!!!!!!!!!!!!

// // GLARING BUG THAT WILL PROBABLY HIT ME IN THE FUTURE: 
// // CASTING SECTOR NUMBER TO UINT8_T IS A HORRIBLE IDEA

// // !!!!!!!!!!!!!!!!!!!!!

// #include <stdint.h>
// #include <kernel/io/iolibrary.h>
// #include <kernel/logging.h>
// // #include <kernel/memory/kalloc.h>

// #define PRIM_DATA_REG 0x1F0
// #define PRIM_ERR_REG 0x1F1
// #define PRIM_SEC_COUNT 0x1F2
// #define PRIM_LBA_LOW 0x1F3
// #define PRIM_LBA_MID 0x1F4
// #define PRIM_LBA_HI 0x1F5
// #define PRIM_DRIVE_SEL 0x1F6
// #define PRIM_STATUS_REG 0x1F7
// #define PRIM_CMD_REG 0x177
// #define PRIM_ALT_STAT_DEV_CTRL_REG 0x3F6

// #define BSY 0x80
// #define RDY 0x40
// #define DRQ 0x08 // data request
// #define DF 0x20 // drive fault
// #define ERR 0x01

// // int readSectors(unsigned long lbaAddr, uint8_t* secData, unsigned long secCount) {
// //     kprint("count: ");
// //     kprint_hex(secCount);
// //     while ((inb(PRIM_STATUS_REG) & BSY) && !(inb(PRIM_STATUS_REG) & RDY)) {}

// //     outb(PRIM_DRIVE_SEL, 0xE0 | ((lbaAddr >> 24) & 0x0F)); // define LBA
// //     outb(PRIM_SEC_COUNT, (uint8_t) secCount);
// //     outb(PRIM_LBA_LOW, lbaAddr & 0x000000FF);
// //     outb(PRIM_LBA_MID, (lbaAddr & 0x0000FF00) >> 8);
// //     outb(PRIM_LBA_HI, (lbaAddr & 0x00FF0000) >> 16);
// //     outb(PRIM_STATUS_REG, 0x20);

// //     for (int i = 0; i < secCount; i++) {
// //         while (!(inb(PRIM_STATUS_REG) & RDY) && !(inb(PRIM_STATUS_REG) & DRQ)) {}
// //         for (int j = 0; j < 512; j += 2) {
// //             uint16_t dataWord = insw(PRIM_DATA_REG);
// //             secData[i*512 + j] = (uint8_t) dataWord;
// //             secData[i*512 + j + 1] = (uint8_t) (dataWord >> 8);
// //         }
// //     }

// //     return 1; // success, CHANGE LATER TO ADD FAILURE CODES
// // }

// int writeSectors(unsigned long lbaAddr, uint8_t* value, unsigned long secCount) {
//     // lbaAddr &= 0b00001111111111111111111111111111;
//     while ((inb(PRIM_STATUS_REG) & BSY) && !(inb(PRIM_STATUS_REG) & RDY)) {}

//     outb(PRIM_DRIVE_SEL, 0xE0 | ((lbaAddr >> 24) & 0x0F)); // define LBA
//     outb(PRIM_SEC_COUNT, (uint8_t) secCount); // 1 sector
//     outb(PRIM_LBA_LOW, lbaAddr & 0x000000FF);
//     outb(PRIM_LBA_MID, (lbaAddr & 0x0000FF00) >> 8);
//     outb(PRIM_LBA_HI, (lbaAddr & 0x00FF0000) >> 16);
//     outb(PRIM_STATUS_REG, 0x30);

//     for (int i = 0; i < (uint8_t) secCount; i++) {
//         while (!(inb(PRIM_STATUS_REG) & RDY) && !(inb(PRIM_STATUS_REG) & DRQ)) {}
//         for (int j = 0; j < 512; j += 2) {
//             outsw(PRIM_DATA_REG, value[i*512 + j] + (value[i*512 + j + 1] << 8));
//         }
//     }
//     outb(PRIM_STATUS_REG, 0xE7); // commit to disk

//     return 1; // success, CHANGE LATER TO ADD FAILURE CODES
// }


// #include <kernel/
// #include "port.h"
// #include "kheap.h"
#include <stdio.h>
#include <stdlib.h>
#include <kernel/io/iolibrary.h>

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMM_REGSTAT 0x1F7
#define ATA_PRIMARY_ALTSTAT_DCR  0x3F6


#define STAT_ERR  (1 << 0) // Indicates an error occurred. Send a new command to clear it
#define STAT_DRQ  (1 << 3) // Set when the drive has PIO data to transfer, or is ready to accept PIO data.
#define STAT_SRV  (1 << 4) // Overlapped Mode Service Request.
#define STAT_DF   (1 << 5) // Drive Fault Error (does not set ERR).
#define STAT_RDY  (1 << 6) // Bit is clear when drive is spun down, or after an error. Set otherwise.
#define STAT_BSY  (1 << 7) // Indicates the drive is preparing to send/receive data (wait for it to clear).
                           // In case of 'hang' (it never clears), do a software reset.


/**
 * To use the IDENTIFY command, select a target drive by sending 0xA0 for the master drive,
 * or 0xB0 for the slave, to the "drive select" IO port. On the Primary bus, this would be port 0x1F6.
 * Then set the Sectorcount, LBAlo, LBAmid, and LBAhi IO ports to 0 (port 0x1F2 to 0x1F5). Then send
 * the IDENTIFY command (0xEC) to the Command IO port (0x1F7).
 *
 * Then read the Status port (0x1F7) again.
 * If the value read is 0, the drive does not exist.
 * For any other value: poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80) clears.
 * Because of some ATAPI drives that do not follow spec, at this point you need to check the
 * LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are non-zero. If so, the drive is not ATA,
 * and you should stop polling. Otherwise, continue polling one of the Status
 * ports until bit 3 (DRQ, value = 8) sets, or until bit 0 (ERR, value = 1) sets.
 *
 * At that point, if ERR is clear, the data is ready to read from the Data port (0x1F0).
 * Read 256 16-bit values, and store them.
 */

 /*
MY CUSTOM WRITE SUCKED, SO I GOT THE IMPLEMENTATION FROM: https://github.com/knusbaum/kernel
*/


uint8_t identify() {
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_SECCOUNT, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_LO, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_MID, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_LBA_HI, 0);
    inb(ATA_PRIMARY_COMM_REGSTAT);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xEC);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);

    // Read the status port. If it's zero, the drive does not exist.
    uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);

    printf("Waiting for status.\n");
    while(status & STAT_BSY) {
        uint32_t i = 0;
        while(1) {
            printf("Printing stuff %d\n", i);
            i++;
        }
        for(i = 0; i < 0x0FFFFFFF; i++) {}
        printf("Checking regstat.\n");
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }
    
    if(status == 0) return 0;

    printf("Status indicates presence of a drive. Polling while STAT_BSY... ");
    while(status & STAT_BSY) {
      printf("\ninb(ATA_PRIMARY_COMM_REGSTAT);... ");
      status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }
    printf("Done.\n");

    uint8_t mid = inb(ATA_PRIMARY_LBA_MID);
    uint8_t hi = inb(ATA_PRIMARY_LBA_HI);
    if(mid || hi) {
        // The drive is not ATA. (Who knows what it is.)
        return 0;
    }

    printf("Waiting for ERR or DRQ.\n");
    // Wait for ERR or DRQ
    while(!(status & (STAT_ERR | STAT_DRQ))) {
        status = inb(ATA_PRIMARY_COMM_REGSTAT);
    }

    if(status & STAT_ERR) {
        // There was an error on the drive. Forget about it.
        return 0;
    }

    printf("Reading IDENTIFY structure.\n");
    //uint8_t *buff = kmalloc(40960, 0, NULL);
    uint8_t buff[256 * 2];
    insw(ATA_PRIMARY_DATA, buff, 256);
    printf("Success. Disk is ready to go.\n");
    // We read it!
    return 1;
}


/**
 * 48-bit LBA read
 *
 * Send 0x40 for the "master" or 0x50 for the "slave" to port 0x1F6: outb(0x1F6, 0x40 | (slavebit << 4))
 * outb (0x1F2, sectorcount high byte)
 * outb (0x1F3, LBA4)
 * outb (0x1F4, LBA5)
 * outb (0x1F5, LBA6)
 * outb (0x1F2, sectorcount low byte)
 * outb (0x1F3, LBA1)
 * outb (0x1F4, LBA2)
 * outb (0x1F5, LBA3)
 * Send the "READ SECTORS EXT" command (0x24) to port 0x1F7: outb(0x1F7, 0x24)
 */

int ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {
    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x24);                   // READ SECTORS EXT


    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
        }
        // Transfer the data!
        insw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    return 1;
}

/**
 * To write sectors in 48 bit PIO mode, send command "WRITE SECTORS EXT" (0x34), instead.
 * (As before, do not use REP outsw when writing.) And remember to do a Cache Flush after
 * each write command completes.
 */

int ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {

    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x34);                   // READ SECTORS EXT

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
            else if(status & STAT_ERR) {
                printf("DISK SET ERROR STATUS!");
                return 0;
            }
        }
        // Transfer the data!
        outsw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    // Flush the cache.
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
    // Poll for BSY.
    while(inb(ATA_PRIMARY_COMM_REGSTAT) & STAT_BSY) {}

    return 1;
}

int fatReadWrapper(unsigned long lbaAddr, uint8_t* secData, unsigned long secCount) {
    return ata_pio_read48(lbaAddr, secCount, secData);
}

int fatWriteWrapper(unsigned long lbaAddr, uint8_t* secData, unsigned long secCount) {
    return ata_pio_write48(lbaAddr, secCount, secData);
}
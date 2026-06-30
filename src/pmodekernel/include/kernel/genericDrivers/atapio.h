#ifndef ATAPIO_H
#define ATAPIO_H

// int readSectors(unsigned long startAddr, uint8_t* secData, unsigned long secCount);
// int writeSectors(unsigned long lbaAddr, uint8_t* value, unsigned long secCount);
int fatWriteWrapper(unsigned long lbaAddr, uint8_t* secData, unsigned long secCount);
int fatReadWrapper(unsigned long lbaAddr, uint8_t* secData, unsigned long secCount);
uint8_t identify();

#endif
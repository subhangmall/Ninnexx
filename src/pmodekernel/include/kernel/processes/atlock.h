#include <stdint.h>

#ifndef ATLOCK_H
#define ATLOCK_H

#define ATLOCK_UNLOCKED 0
#define ATLOCK_LOCKED 1

typedef uint8_t atlock;

void acquireLock(atlock* l);
void releaseLock(atlock* l);


#endif
#ifndef PS2KEYBOARD_H
#define PS2KEYBOARD_H

#include <stdint.h>

extern uint32_t bufferAddr;
extern bool gettingInput;
extern uint32_t pidOccupying;

void initKeyboard();

#endif
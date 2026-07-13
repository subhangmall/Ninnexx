#ifndef PS2KEYBOARD_H
#define PS2KEYBOARD_H

#include <stdint.h>

struct KeyEvent {
    uint8_t code;
    uint8_t modifiers;
} __attribute__((packed));

extern struct KeyEvent buffer[256];
extern uint16_t curIdx;
char getCharFromEvent(struct KeyEvent ev);

void initKeyboard();

#endif
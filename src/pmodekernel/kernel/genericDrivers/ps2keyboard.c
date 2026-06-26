// partially inspired by baponkar.githubio

#include <stdint.h>

#include <kernel/interrupts/pic.h>
#include <kernel/io/iolibrary.h>
#include <kernel/logging.h>


uint8_t bufferPosition = 0;

struct KeyEvent {
    uint8_t code;
    uint8_t modifiers;
};

struct KeyEvent keyBuffer[256];

#define ERROR_1 0x00
#define ERROR_2 0xFF
#define SELF_TEST_PASSED 0xAA
#define ECHO 0xEE
#define CMD_ACKNOWLEDGED 0xFA
#define SELF_TEST_FAILED_1 0xFC
#define SELF_TEST_FAILED_2 0xFD
#define RESEND 0xFE
#define DATA_REG 0x60
#define STATUS_REG 0x64

#define NORMAL_STATE 0
#define PREFIX_STATE 1

#define R_CTRL_MASK 0b00000001
#define L_CTRL_MASK 0b00000010
#define R_ALT_MASK 0b000000100
#define L_ALT_MASK 0b000001000
#define R_SHIFT_MASK 0b00010000
#define L_SHIFT_MASK 0b00100000
#define CAPS_LOCK_MASK 0b01000000

uint8_t currentModifiers;
uint8_t currentState;

typedef enum Keys {
    A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z, 
    one, two, three, four, five, six, seven, eight, nine, zero
};

static char KeysAscii[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

void keyboardIRQHandler(struct InterruptStackFrame* stack) {
    uint8_t scancode = inb(0x60);
    struct KeyEvent ev;    

    if (scancode == 0xE0) {
        currentState = PREFIX_STATE;
        // update state, exit
        // kprint("e");
        sendEOIToPIC(0x01);
        return;
    }

    if (currentState == PREFIX_STATE) {
        // EXTENDED MODIFIERS

        // right ctrl pressed
        if (scancode == 0x1D) {
            currentModifiers |= R_CTRL_MASK;
            currentState = NORMAL_STATE;
            goto end;
        }

        // right contorl released
        if (scancode == 0x9D) {
            currentModifiers &= ~R_CTRL_MASK;
            currentState = NORMAL_STATE;
            goto end;
        }

        // ralt pressed
        if (scancode == 0x38) {
            currentModifiers |= R_ALT_MASK;
            goto end;
        }
        
        // ralt released
        if (scancode == 0xB8) {
            currentModifiers &= ~R_ALT_MASK;
            goto end;
        }
    }

    // NON-EXTENDED MODIFIERS

    // SHIFT

    // shifts pressed
    if (scancode == 0x2A /*lshift*/) {
        currentModifiers |= L_SHIFT_MASK;
        goto end;
    }
    if (scancode == 0x36 /*rshift*/) {
        currentModifiers |= R_SHIFT_MASK;
        goto end;
    }
    
    // shifts released
    if (scancode == 0xAA) {
        currentModifiers &= ~L_SHIFT_MASK;
        goto end;
    }

    if (scancode == 0xB6) {
        currentModifiers &= ~R_SHIFT_MASK;
        goto end;
    }

    // CTRL
     
    // lctrl pressed
    if (scancode == 0x1D) {
        currentModifiers |= L_CTRL_MASK;
        goto end;
    }
    
    // lctrl released
    if (scancode == 0x9D) {
        currentModifiers &= ~L_CTRL_MASK;
        goto end;
    }

    // ALT

    // lalt pressed
    if (scancode == 0x38) {
        currentModifiers |= L_ALT_MASK;
        goto end;
    }
    
    // lalt released
    if (scancode == 0xB8) {
        currentModifiers &= ~L_ALT_MASK;
        goto end;
    }

    // CAPS LOCK

    if (scancode == 0x3A) {
        if (currentModifiers & CAPS_LOCK_MASK) {
            // caps lock currently active
            currentModifiers &= ~CAPS_LOCK_MASK;
        } else {
            // caps lock currently inactive
            currentModifiers |= CAPS_LOCK_MASK;
        }
        goto end;
    }

    // letters

    if (scancode == 0x1E) {
        ev.code = A;
        goto character;
    }

    character:
        ev.modifiers = currentModifiers;
        if (bufferPosition++ ==  255)
            bufferPosition = 0;
        keyBuffer[bufferPosition] = ev;
        goto end;

    end:
        kprint_hex(keyBuffer[bufferPosition].code);
        sendEOIToPIC(0x01);
        return;
    
}

void initKeyboard() {
    // SCAN CODE 1
    currentState = NORMAL_STATE;
    currentModifiers = 0;
    linkIRQHandler(0x01, (uint32_t) &keyboardIRQHandler);
}


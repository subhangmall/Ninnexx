// partially inspired by baponkar.githubio
#include <stdint.h>
#include <kernel/interrupts/pic.h>
#include <kernel/io/iolibrary.h>
#include <kernel/logging.h>
#include <kernel/processes/process.h>
#include <kernel/processes/contextSwitch.h>
#include <stdio.h>
#include <kernel/genericDrivers/ps2keyboard.h>

extern struct Process* current;
extern struct Process procHead;

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

struct KeyEvent buffer[256];
uint16_t curIdx = 0;

extern uint32_t procIDShowing;

typedef enum Keys {
    A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z, 
    one, two, three, four, five, six, seven, eight, nine, zero,
    space, enter, 
    semicolon, backslash, period, comma, singlequote, leftbracket, rightbracket, minus, equals, slash, tick
};

static char VisibleKeysAscii[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ' ', '\r', ';', '\\', '.', ',', '\'', '[', ']', '-', '=', '/', '`'
};

static char VisibleKeysAsciiShift[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', ' ', '\r', ':', '|', '>', '<', '"', '{', '}', '_', '+', '?', '~'
};

char getCharFromEvent(struct KeyEvent ev) {
    if (ev.code >= sizeof(VisibleKeysAscii)) {
        return '\0';
    }

    if (ev.modifiers & R_SHIFT_MASK || ev.modifiers & L_SHIFT_MASK) {
        return VisibleKeysAsciiShift[ev.code];
    } else if ((ev.modifiers & CAPS_LOCK_MASK) && ev.code < 26){
        return VisibleKeysAsciiShift[ev.code];
    } else {
        return VisibleKeysAscii[ev.code];
    }
}

void keyboardIRQHandler(struct InterruptStackFrame* stack) {
    uint8_t scancode = inb(0x60);
    struct KeyEvent ev;    

    if (scancode == 0xE0) {
        currentState = PREFIX_STATE;
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

        if ((ev.modifiers & L_ALT_MASK) && (ev.modifiers & L_CTRL_MASK)) {
            switchForegroundProcess();
            goto end;
        }

        goto character;
    }

    if (scancode == 0x30) {
        ev.code = B;
        goto character;
    }

    if (scancode == 0x2E) {
        ev.code = C;
        goto character;
    }

    if (scancode == 0x20) {
        ev.code = D;
        goto character;
    }

    if (scancode == 0x12) {
        ev.code = E;
        goto character;
    }

    if (scancode == 0x21) {
        ev.code = F;
        goto character;
    }

    if (scancode == 0x22) {
        ev.code = G;
        goto character;
    }

    if (scancode == 0x23) {
        ev.code = H;
        goto character;
    }

    if (scancode == 0x17) {
        ev.code = I;
        goto character;
    }

    if (scancode == 0x24) {
        ev.code = J;
        goto character;
    }

    if (scancode == 0x25) {
        ev.code = K;
        goto character;
    }

    if (scancode == 0x26) {
        ev.code = L;
        goto character;
    }

    if (scancode == 0x32) {
        ev.code = M;
        goto character;
    }

    if (scancode == 0x31) {
        ev.code = N;
        goto character;
    }

    if (scancode == 0x18) {
        ev.code = O;
        goto character;
    }

    if (scancode == 0x19) {
        ev.code = P;
        goto character;
    }

    if (scancode == 0x10) {
        ev.code = Q;
        goto character;
    }

    if (scancode == 0x13) {
        ev.code = R;
        goto character;
    }

    if (scancode == 0x1F) {
        ev.code = S;
        goto character;
    }

    if (scancode == 0x14) {
        ev.code = T;
        goto character;
    }

    if (scancode == 0x16) {
        ev.code = U;
        goto character;
    }

    if (scancode == 0x2F) {
        ev.code = V;
        goto character;
    }

    if (scancode == 0x11) {
        ev.code = W;
        goto character;
    }

    if (scancode == 0x2D) {
        ev.code = X;
        goto character;
    }

    if (scancode == 0x15) {
        ev.code = Y;
        goto character;
    }

    if (scancode == 0x2C) {
        ev.code = Z;
        goto character;
    }

    if (scancode == 0x0B) {
        ev.code = zero;
        goto character;
    }

    if (scancode == 0x02) {
        ev.code = one;
        goto character;
    }

    if (scancode == 0x03) {
        ev.code = two;
        goto character;
    }

    if (scancode == 0x04) {
        ev.code = three;
        goto character;
    }

    if (scancode == 0x05) {
        ev.code = four;
        goto character;
    }

    if (scancode == 0x06) {
        ev.code = five;
        goto character;
    }

    if (scancode == 0x07) {
        ev.code = six;
        goto character;
    }

    if (scancode == 0x08) {
        ev.code = seven;
        goto character;
    }

    if (scancode == 0x09) {
        ev.code = eight;
        goto character;
    }

    if (scancode == 0xA) {
        ev.code = nine;
        goto character;
    }

    if (scancode == 0x39) {
        ev.code = space;
        goto character;
    }

    if (scancode == 0x1C) {
        ev.code = enter;
        goto character;
    }

    if (scancode == 0x2B) {
        ev.code = backslash;
        goto character;
    }

    if (scancode == 0x27) {
        ev.code = semicolon;
        goto character;
    }

    if (scancode == 0x34) {
        ev.code = period;
        goto character;
    }

    if (scancode == 0x33) {
        ev.code = comma;
        goto character;
    }

    if (scancode == 0x35) {
        ev.code = slash;
        goto character;
    }

    if (scancode == 0x28) {
        ev.code = singlequote;
        goto character;
    }

    if (scancode == 0x1A) {
        ev.code = leftbracket;
        goto character;
    }

    if (scancode == 0x1B) {
        ev.code = rightbracket;
        goto character;
    }

    if (scancode == 0x0C) {
        ev.code = minus;
        goto character;
    }

    if (scancode == 0x0D) {
        ev.code = equals;
        goto character;
    }

    if (scancode == 0x29) {
        ev.code = tick;
        goto character;
    }

    goto end; // if character fits none of these descriptions

    character:
        struct Process* iter = (struct Process*) &procHead;
        while (iter->procID != procIDShowing) {
            if (iter->next->procID == 0) goto end; // process doesnt exist, as it loops back to procHead
            iter = iter->next;
        }
        // now that iter is equal to requesting keyboard input
        if (iter->status != 1) {
            goto end; // ignore cause it isn't requesting input
        }
        

        ev.modifiers = currentModifiers;
        buffer[curIdx] = ev;

        // wake waiting process
        asm volatile(
            "mov $1, %%eax\n\t"
            "mov %0, %%ebx\n\t"
            "int $0xFF"
            :
            : "r" (procIDShowing)
            : "eax", "ebx", "memory"
        );

        curIdx++;
        if (curIdx == 256) curIdx = 0;

        goto end;

    end:
        sendEOIToPIC(0x01);
        return;
    
}

void initKeyboard() {
    // SCAN CODE 1
    
    // do {
    // outb(DATA_REG, 0xF0);
    // } while (inb(DATA_REG) != CMD_ACKNOWLEDGED);
    // sendEOIToPIC(0x01);
    // do {
    // outb(DATA_REG, 0x01);
    // } while (inb(DATA_REG) != CMD_ACKNOWLEDGED);
    // sendEOIToPIC(0x01);
    // kprint("hidwe");
    // sendEOIToPIC(0x01);

    while (inb(STATUS_REG) & 1) {
        inb(DATA_REG);
    }

    while (inb(STATUS_REG) & 0x02);
    outb(DATA_REG, 0xF4);

    while (inb(STATUS_REG) & 1) {
        inb(DATA_REG);
    }

    currentState = NORMAL_STATE;
    currentModifiers = 0;
    linkIRQHandler(0x01, (uint32_t) &keyboardIRQHandler);
}


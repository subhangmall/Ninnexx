; section .text

; global int0

; extern commonHandler

; int0: ; divide error
;     cli
;     push 0
;     push 0x0
;     jmp isr_common

; isr_common:
;     pusha
;     push ds
;     push es
;     push fs
;     push gs

;     mov ax, 0x10
;     mov ds, ax
;     mov es, ax

;     push esp
;     call commonHandler
;     add esp, 4

;     pop gs
;     pop fs
;     pop es
;     pop ds
;     popa
;     add esp, 8
;     sti
;     iretd



section .text

global int0
global int1
global int2
global int3
global int4
global int5
global int6
global int7
global int8
global int9
global intA
global intB
global intC
global intD
global intE ; SKIP F BECAUSE 0F IS RESERVED
global int10
global int11
global int12
global int13
global int14
global int15

extern commonHandler

; NON ERROR CODE INTERRUPTS

int0: ; divide error
    cli
    push 0
    push 0x0
    jmp isr_common

int1: ; debug exception
    cli
    push 0
    push 0x1 
    jmp isr_common

int2: ; Nonmaskable external interrupt
    cli
    push 0
    push 0x2
    jmp isr_common

int3: ; breakpoint
    cli
    push 0
    push 0x3
    jmp isr_common

int4: ; overflow
    cli
    push 0
    push 0x4
    jmp isr_common

int5: ; BOUND range exceeded
    cli
    push 0
    push 0x5
    jmp isr_common

int6: ; invalid opcode
    cli
    push 0
    push 0x6
    jmp isr_common

int7: ; no math coprocessor
    cli
    push 0
    push 0x7
    jmp isr_common

int9: ; coprocessor segment overrun
    cli
    push 0
    push 0x9
    jmp isr_common

; NOTE : SKIP INTEL 0F because not allowed to be used

int10: ; x87 FPU error
    cli
    push 0
    push 0x10
    jmp isr_common

int12: ; machine check; model dependent
    cli
    push 0
    push 0x12
    jmp isr_common

int13: ; machine check; model dependent
    cli
    push 0
    push 0x13
    jmp isr_common

int14: ; virtualization exception
    cli
    push 0
    push 0x14
    jmp isr_common

; global int15
; int15:
;     cli
;     push 0
;     push 0x15
;     jmp isr_common

global int16
int16:
    cli
    push 0
    push 0x16
    jmp isr_common

global int17
int17:
    cli
    push 0
    push 0x17
    jmp isr_common

global int18
int18:
    cli
    push 0
    push 0x18
    jmp isr_common

global int19
int19:
    cli
    push 0
    push 0x19
    jmp isr_common

global int1A
int1A:
    cli
    push 0
    push 0x1A
    jmp isr_common

global int1B
int1B:
    cli
    push 0
    push 0x1B
    jmp isr_common

global int1C
int1C:
    cli
    push 0
    push 0x1C
    jmp isr_common

global int1D
int1D:
    cli
    push 0
    push 0x1D
    jmp isr_common

global int1E
int1E:
    cli
    push 0
    push 0x1E
    jmp isr_common

global int1F
int1F:
    cli
    push 0
    push 0x1F
    jmp isr_common

global int20
int20:
    cli
    push 0
    push 0x20
    jmp isr_common

global int21
int21:
    cli
    push 0
    push 0x21
    jmp isr_common

global int22
int22:
    cli
    push 0
    push 0x22
    jmp isr_common

global int23
int23:
    cli
    push 0
    push 0x23
    jmp isr_common

global int24
int24:
    cli
    push 0
    push 0x24
    jmp isr_common

global int25
int25:
    cli
    push 0
    push 0x25
    jmp isr_common

global int26
int26:
    cli
    push 0
    push 0x26
    jmp isr_common

global int27
int27:
    cli
    push 0
    push 0x27
    jmp isr_common

global int28
int28:
    cli
    push 0
    push 0x28
    jmp isr_common

global int29
int29:
    cli
    push 0
    push 0x29
    jmp isr_common

global int2A
int2A:
    cli
    push 0
    push 0x2A
    jmp isr_common

global int2B
int2B:
    cli
    push 0
    push 0x2B
    jmp isr_common

global int2C
int2C:
    cli
    push 0
    push 0x2C
    jmp isr_common

global int2D
int2D:
    cli
    push 0
    push 0x2D
    jmp isr_common

global int2E
int2E:
    cli
    push 0
    push 0x2E
    jmp isr_common

global int2F
int2F:
    cli
    push 0
    push 0x2F
    jmp isr_common

global int30
int30:
    cli
    push 0
    push 0x30
    jmp isr_common

global int31
int31:
    cli
    push 0
    push 0x31
    jmp isr_common

global int32
int32:
    cli
    push 0
    push 0x32
    jmp isr_common

global int33
int33:
    cli
    push 0
    push 0x33
    jmp isr_common

global int34
int34:
    cli
    push 0
    push 0x34
    jmp isr_common

global int35
int35:
    cli
    push 0
    push 0x35
    jmp isr_common

global int36
int36:
    cli
    push 0
    push 0x36
    jmp isr_common

global int37
int37:
    cli
    push 0
    push 0x37
    jmp isr_common

global int38
int38:
    cli
    push 0
    push 0x38
    jmp isr_common

global int39
int39:
    cli
    push 0
    push 0x39
    jmp isr_common

global int3A
int3A:
    cli
    push 0
    push 0x3A
    jmp isr_common

global int3B
int3B:
    cli
    push 0
    push 0x3B
    jmp isr_common

global int3C
int3C:
    cli
    push 0
    push 0x3C
    jmp isr_common

global int3D
int3D:
    cli
    push 0
    push 0x3D
    jmp isr_common

global int3E
int3E:
    cli
    push 0
    push 0x3E
    jmp isr_common

global int3F
int3F:
    cli
    push 0
    push 0x3F
    jmp isr_common

global int40
int40:
    cli
    push 0
    push 0x40
    jmp isr_common

global int41
int41:
    cli
    push 0
    push 0x41
    jmp isr_common

global int42
int42:
    cli
    push 0
    push 0x42
    jmp isr_common

global int43
int43:
    cli
    push 0
    push 0x43
    jmp isr_common

global int44
int44:
    cli
    push 0
    push 0x44
    jmp isr_common

global int45
int45:
    cli
    push 0
    push 0x45
    jmp isr_common

global int46
int46:
    cli
    push 0
    push 0x46
    jmp isr_common

global int47
int47:
    cli
    push 0
    push 0x47
    jmp isr_common

global int48
int48:
    cli
    push 0
    push 0x48
    jmp isr_common

global int49
int49:
    cli
    push 0
    push 0x49
    jmp isr_common

global int4A
int4A:
    cli
    push 0
    push 0x4A
    jmp isr_common

global int4B
int4B:
    cli
    push 0
    push 0x4B
    jmp isr_common

global int4C
int4C:
    cli
    push 0
    push 0x4C
    jmp isr_common

global int4D
int4D:
    cli
    push 0
    push 0x4D
    jmp isr_common

global int4E
int4E:
    cli
    push 0
    push 0x4E
    jmp isr_common

global int4F
int4F:
    cli
    push 0
    push 0x4F
    jmp isr_common

global int50
int50:
    cli
    push 0
    push 0x50
    jmp isr_common

global int51
int51:
    cli
    push 0
    push 0x51
    jmp isr_common

global int52
int52:
    cli
    push 0
    push 0x52
    jmp isr_common

global int53
int53:
    cli
    push 0
    push 0x53
    jmp isr_common

global int54
int54:
    cli
    push 0
    push 0x54
    jmp isr_common

global int55
int55:
    cli
    push 0
    push 0x55
    jmp isr_common

global int56
int56:
    cli
    push 0
    push 0x56
    jmp isr_common

global int57
int57:
    cli
    push 0
    push 0x57
    jmp isr_common

global int58
int58:
    cli
    push 0
    push 0x58
    jmp isr_common

global int59
int59:
    cli
    push 0
    push 0x59
    jmp isr_common

global int5A
int5A:
    cli
    push 0
    push 0x5A
    jmp isr_common

global int5B
int5B:
    cli
    push 0
    push 0x5B
    jmp isr_common

global int5C
int5C:
    cli
    push 0
    push 0x5C
    jmp isr_common

global int5D
int5D:
    cli
    push 0
    push 0x5D
    jmp isr_common

global int5E
int5E:
    cli
    push 0
    push 0x5E
    jmp isr_common

global int5F
int5F:
    cli
    push 0
    push 0x5F
    jmp isr_common

global int60
int60:
    cli
    push 0
    push 0x60
    jmp isr_common

global int61
int61:
    cli
    push 0
    push 0x61
    jmp isr_common

global int62
int62:
    cli
    push 0
    push 0x62
    jmp isr_common

global int63
int63:
    cli
    push 0
    push 0x63
    jmp isr_common

global int64
int64:
    cli
    push 0
    push 0x64
    jmp isr_common

global int65
int65:
    cli
    push 0
    push 0x65
    jmp isr_common

global int66
int66:
    cli
    push 0
    push 0x66
    jmp isr_common

global int67
int67:
    cli
    push 0
    push 0x67
    jmp isr_common

global int68
int68:
    cli
    push 0
    push 0x68
    jmp isr_common

global int69
int69:
    cli
    push 0
    push 0x69
    jmp isr_common

global int6A
int6A:
    cli
    push 0
    push 0x6A
    jmp isr_common

global int6B
int6B:
    cli
    push 0
    push 0x6B
    jmp isr_common

global int6C
int6C:
    cli
    push 0
    push 0x6C
    jmp isr_common

global int6D
int6D:
    cli
    push 0
    push 0x6D
    jmp isr_common

global int6E
int6E:
    cli
    push 0
    push 0x6E
    jmp isr_common

global int6F
int6F:
    cli
    push 0
    push 0x6F
    jmp isr_common

global int70
int70:
    cli
    push 0
    push 0x70
    jmp isr_common

global int71
int71:
    cli
    push 0
    push 0x71
    jmp isr_common

global int72
int72:
    cli
    push 0
    push 0x72
    jmp isr_common

global int73
int73:
    cli
    push 0
    push 0x73
    jmp isr_common

global int74
int74:
    cli
    push 0
    push 0x74
    jmp isr_common

global int75
int75:
    cli
    push 0
    push 0x75
    jmp isr_common

global int76
int76:
    cli
    push 0
    push 0x76
    jmp isr_common

global int77
int77:
    cli
    push 0
    push 0x77
    jmp isr_common

global int78
int78:
    cli
    push 0
    push 0x78
    jmp isr_common

global int79
int79:
    cli
    push 0
    push 0x79
    jmp isr_common

global int7A
int7A:
    cli
    push 0
    push 0x7A
    jmp isr_common

global int7B
int7B:
    cli
    push 0
    push 0x7B
    jmp isr_common

global int7C
int7C:
    cli
    push 0
    push 0x7C
    jmp isr_common

global int7D
int7D:
    cli
    push 0
    push 0x7D
    jmp isr_common

global int7E
int7E:
    cli
    push 0
    push 0x7E
    jmp isr_common

global int7F
int7F:
    cli
    push 0
    push 0x7F
    jmp isr_common

global int80
int80:
    cli
    push 0
    push 0x80
    jmp isr_common

global int81
int81:
    cli
    push 0
    push 0x81
    jmp isr_common

global int82
int82:
    cli
    push 0
    push 0x82
    jmp isr_common

global int83
int83:
    cli
    push 0
    push 0x83
    jmp isr_common

global int84
int84:
    cli
    push 0
    push 0x84
    jmp isr_common

global int85
int85:
    cli
    push 0
    push 0x85
    jmp isr_common

global int86
int86:
    cli
    push 0
    push 0x86
    jmp isr_common

global int87
int87:
    cli
    push 0
    push 0x87
    jmp isr_common

global int88
int88:
    cli
    push 0
    push 0x88
    jmp isr_common

global int89
int89:
    cli
    push 0
    push 0x89
    jmp isr_common

global int8A
int8A:
    cli
    push 0
    push 0x8A
    jmp isr_common

global int8B
int8B:
    cli
    push 0
    push 0x8B
    jmp isr_common

global int8C
int8C:
    cli
    push 0
    push 0x8C
    jmp isr_common

global int8D
int8D:
    cli
    push 0
    push 0x8D
    jmp isr_common

global int8E
int8E:
    cli
    push 0
    push 0x8E
    jmp isr_common

global int8F
int8F:
    cli
    push 0
    push 0x8F
    jmp isr_common

global int90
int90:
    cli
    push 0
    push 0x90
    jmp isr_common

global int91
int91:
    cli
    push 0
    push 0x91
    jmp isr_common

global int92
int92:
    cli
    push 0
    push 0x92
    jmp isr_common

global int93
int93:
    cli
    push 0
    push 0x93
    jmp isr_common

global int94
int94:
    cli
    push 0
    push 0x94
    jmp isr_common

global int95
int95:
    cli
    push 0
    push 0x95
    jmp isr_common

global int96
int96:
    cli
    push 0
    push 0x96
    jmp isr_common

global int97
int97:
    cli
    push 0
    push 0x97
    jmp isr_common

global int98
int98:
    cli
    push 0
    push 0x98
    jmp isr_common

global int99
int99:
    cli
    push 0
    push 0x99
    jmp isr_common

global int9A
int9A:
    cli
    push 0
    push 0x9A
    jmp isr_common

global int9B
int9B:
    cli
    push 0
    push 0x9B
    jmp isr_common

global int9C
int9C:
    cli
    push 0
    push 0x9C
    jmp isr_common

global int9D
int9D:
    cli
    push 0
    push 0x9D
    jmp isr_common

global int9E
int9E:
    cli
    push 0
    push 0x9E
    jmp isr_common

global int9F
int9F:
    cli
    push 0
    push 0x9F
    jmp isr_common

global intA0
intA0:
    cli
    push 0
    push 0xA0
    jmp isr_common

global intA1
intA1:
    cli
    push 0
    push 0xA1
    jmp isr_common

global intA2
intA2:
    cli
    push 0
    push 0xA2
    jmp isr_common

global intA3
intA3:
    cli
    push 0
    push 0xA3
    jmp isr_common

global intA4
intA4:
    cli
    push 0
    push 0xA4
    jmp isr_common

global intA5
intA5:
    cli
    push 0
    push 0xA5
    jmp isr_common

global intA6
intA6:
    cli
    push 0
    push 0xA6
    jmp isr_common

global intA7
intA7:
    cli
    push 0
    push 0xA7
    jmp isr_common

global intA8
intA8:
    cli
    push 0
    push 0xA8
    jmp isr_common

global intA9
intA9:
    cli
    push 0
    push 0xA9
    jmp isr_common

global intAA
intAA:
    cli
    push 0
    push 0xAA
    jmp isr_common

global intAB
intAB:
    cli
    push 0
    push 0xAB
    jmp isr_common

global intAC
intAC:
    cli
    push 0
    push 0xAC
    jmp isr_common

global intAD
intAD:
    cli
    push 0
    push 0xAD
    jmp isr_common

global intAE
intAE:
    cli
    push 0
    push 0xAE
    jmp isr_common

global intAF
intAF:
    cli
    push 0
    push 0xAF
    jmp isr_common

global intB0
intB0:
    cli
    push 0
    push 0xB0
    jmp isr_common

global intB1
intB1:
    cli
    push 0
    push 0xB1
    jmp isr_common

global intB2
intB2:
    cli
    push 0
    push 0xB2
    jmp isr_common

global intB3
intB3:
    cli
    push 0
    push 0xB3
    jmp isr_common

global intB4
intB4:
    cli
    push 0
    push 0xB4
    jmp isr_common

global intB5
intB5:
    cli
    push 0
    push 0xB5
    jmp isr_common

global intB6
intB6:
    cli
    push 0
    push 0xB6
    jmp isr_common

global intB7
intB7:
    cli
    push 0
    push 0xB7
    jmp isr_common

global intB8
intB8:
    cli
    push 0
    push 0xB8
    jmp isr_common

global intB9
intB9:
    cli
    push 0
    push 0xB9
    jmp isr_common

global intBA
intBA:
    cli
    push 0
    push 0xBA
    jmp isr_common

global intBB
intBB:
    cli
    push 0
    push 0xBB
    jmp isr_common

global intBC
intBC:
    cli
    push 0
    push 0xBC
    jmp isr_common

global intBD
intBD:
    cli
    push 0
    push 0xBD
    jmp isr_common

global intBE
intBE:
    cli
    push 0
    push 0xBE
    jmp isr_common

global intBF
intBF:
    cli
    push 0
    push 0xBF
    jmp isr_common

global intC0
intC0:
    cli
    push 0
    push 0xC0
    jmp isr_common

global intC1
intC1:
    cli
    push 0
    push 0xC1
    jmp isr_common

global intC2
intC2:
    cli
    push 0
    push 0xC2
    jmp isr_common

global intC3
intC3:
    cli
    push 0
    push 0xC3
    jmp isr_common

global intC4
intC4:
    cli
    push 0
    push 0xC4
    jmp isr_common

global intC5
intC5:
    cli
    push 0
    push 0xC5
    jmp isr_common

global intC6
intC6:
    cli
    push 0
    push 0xC6
    jmp isr_common

global intC7
intC7:
    cli
    push 0
    push 0xC7
    jmp isr_common

global intC8
intC8:
    cli
    push 0
    push 0xC8
    jmp isr_common

global intC9
intC9:
    cli
    push 0
    push 0xC9
    jmp isr_common

global intCA
intCA:
    cli
    push 0
    push 0xCA
    jmp isr_common

global intCB
intCB:
    cli
    push 0
    push 0xCB
    jmp isr_common

global intCC
intCC:
    cli
    push 0
    push 0xCC
    jmp isr_common

global intCD
intCD:
    cli
    push 0
    push 0xCD
    jmp isr_common

global intCE
intCE:
    cli
    push 0
    push 0xCE
    jmp isr_common

global intCF
intCF:
    cli
    push 0
    push 0xCF
    jmp isr_common

global intD0
intD0:
    cli
    push 0
    push 0xD0
    jmp isr_common

global intD1
intD1:
    cli
    push 0
    push 0xD1
    jmp isr_common

global intD2
intD2:
    cli
    push 0
    push 0xD2
    jmp isr_common

global intD3
intD3:
    cli
    push 0
    push 0xD3
    jmp isr_common

global intD4
intD4:
    cli
    push 0
    push 0xD4
    jmp isr_common

global intD5
intD5:
    cli
    push 0
    push 0xD5
    jmp isr_common

global intD6
intD6:
    cli
    push 0
    push 0xD6
    jmp isr_common

global intD7
intD7:
    cli
    push 0
    push 0xD7
    jmp isr_common

global intD8
intD8:
    cli
    push 0
    push 0xD8
    jmp isr_common

global intD9
intD9:
    cli
    push 0
    push 0xD9
    jmp isr_common

global intDA
intDA:
    cli
    push 0
    push 0xDA
    jmp isr_common

global intDB
intDB:
    cli
    push 0
    push 0xDB
    jmp isr_common

global intDC
intDC:
    cli
    push 0
    push 0xDC
    jmp isr_common

global intDD
intDD:
    cli
    push 0
    push 0xDD
    jmp isr_common

global intDE
intDE:
    cli
    push 0
    push 0xDE
    jmp isr_common

global intDF
intDF:
    cli
    push 0
    push 0xDF
    jmp isr_common

global intE0
intE0:
    cli
    push 0
    push 0xE0
    jmp isr_common

global intE1
intE1:
    cli
    push 0
    push 0xE1
    jmp isr_common

global intE2
intE2:
    cli
    push 0
    push 0xE2
    jmp isr_common

global intE3
intE3:
    cli
    push 0
    push 0xE3
    jmp isr_common

global intE4
intE4:
    cli
    push 0
    push 0xE4
    jmp isr_common

global intE5
intE5:
    cli
    push 0
    push 0xE5
    jmp isr_common

global intE6
intE6:
    cli
    push 0
    push 0xE6
    jmp isr_common

global intE7
intE7:
    cli
    push 0
    push 0xE7
    jmp isr_common

global intE8
intE8:
    cli
    push 0
    push 0xE8
    jmp isr_common

global intE9
intE9:
    cli
    push 0
    push 0xE9
    jmp isr_common

global intEA
intEA:
    cli
    push 0
    push 0xEA
    jmp isr_common

global intEB
intEB:
    cli
    push 0
    push 0xEB
    jmp isr_common

global intEC
intEC:
    cli
    push 0
    push 0xEC
    jmp isr_common

global intED
intED:
    cli
    push 0
    push 0xED
    jmp isr_common

global intEE
intEE:
    cli
    push 0
    push 0xEE
    jmp isr_common

global intEF
intEF:
    cli
    push 0
    push 0xEF
    jmp isr_common

global intF0
intF0:
    cli
    push 0
    push 0xF0
    jmp isr_common

global intF1
intF1:
    cli
    push 0
    push 0xF1
    jmp isr_common

global intF2
intF2:
    cli
    push 0
    push 0xF2
    jmp isr_common

global intF3
intF3:
    cli
    push 0
    push 0xF3
    jmp isr_common

global intF4
intF4:
    cli
    push 0
    push 0xF4
    jmp isr_common

global intF5
intF5:
    cli
    push 0
    push 0xF5
    jmp isr_common

global intF6
intF6:
    cli
    push 0
    push 0xF6
    jmp isr_common

global intF7
intF7:
    cli
    push 0
    push 0xF7
    jmp isr_common

global intF8
intF8:
    cli
    push 0
    push 0xF8
    jmp isr_common

global intF9
intF9:
    cli
    push 0
    push 0xF9
    jmp isr_common

global intFA
intFA:
    cli
    push 0
    push 0xFA
    jmp isr_common

global intFB
intFB:
    cli
    push 0
    push 0xFB
    jmp isr_common

global intFC
intFC:
    cli
    push 0
    push 0xFC
    jmp isr_common

global intFD
intFD:
    cli
    push 0
    push 0xFD
    jmp isr_common

global intFE
intFE:
    cli
    push 0
    push 0xFE
    jmp isr_common

global intFF
intFF:
    cli
    push 0
    push 0xFF
    jmp isr_common

; ERROR CODE EXCEPTIONS

int8: ; double fault
    cli
    push 0x08
    jmp isr_common

intA: ; invalid TSS
    cli
    push 0x0A
    jmp isr_common

intB: ; segment not present
    cli
    push 0x0B
    jmp isr_common

intC: ; stack segment fault
    cli
    push 0x0C
    jmp isr_common

intD: ;general protection fault
    cli
    push 0x0D
    jmp isr_common

intE: ; page fault
    cli
    push 0x0E
    jmp isr_common

int11: ; alignment check
    cli
    push 0x11
    jmp isr_common

int15: ; control protection exception
    cli
    push 0x15
    jmp isr_common

isr_common:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax

    push esp
    call commonHandler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd

isr_err:
    iretd
[bits 32]

extern current

section .text
global context_switch_noret
context_switch_noret:
    mov eax, [current]
    add eax, 14
    mov esp, dword [eax]; 
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd
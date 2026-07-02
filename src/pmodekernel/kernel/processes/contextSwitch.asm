[bits 32]

extern current

section .text
global context_switch_noret
context_switch_noret:
    mov eax, current
    add eax, 10
    mov cr3, dword [eax]
    jmp 0x08:.far_jmp_flsh
    .far_jmp_flsh: 
    add eax, 4
    mov esp, dword [eax]; 
    ; add esp, 4 ; b/c ESP already points to interrupt stack frame, not argumetn
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iretd
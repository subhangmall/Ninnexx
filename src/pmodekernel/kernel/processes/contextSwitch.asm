[bits 32]

extern current

section .text
global context_switch_noret
context_switch_noret:
    mov eax, [current]
    add eax, 10
    mov ebx, dword [eax]
    mov cr3, ebx
    jmp 0x08:.far_jmp_flsh
    .far_jmp_flsh: 
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
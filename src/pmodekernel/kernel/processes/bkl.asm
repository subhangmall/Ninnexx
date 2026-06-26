; thanks to the os dev wiki
; big kernel lock
bits 32

bklck: 
    dd 0x00

section .text
global acquireBKL
acquireBKL:
    lock bts [bklck],0
    jc .spinWithPause
    ret

.spinWithPause:
    pause                    
    test dword [bklck],1      
    jnz .spinWithPause     
    jmp acquireBKL         

global releaseBKL
releaseBKL:
    mov dword [bklck],0
    ret


[bits 16]
org 0x100

jmp a
a:
    mov ah, 0x09
    mov dx, teststring
    int 0x21
    b:
    jmp b

teststring: db "hi :DDD $"
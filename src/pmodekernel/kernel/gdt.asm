bits 32

global gdt
gdt:
    ; 1st entry: must be empty
    dq 0 ; NULL DESC

    ; 2nd entry: 32b code segment, flat memory model
gdt_selector_32pm_cs:
    dw 0xFFFF                                       ; Limit (0-15 bits)
    dw 0                                            ; Base (0-15 bits)
    db 0                                            ; Base (16-23 bits)
    db 1001101_0b                              ; Access Byte (reversed in little endian), left to right:
                                                    ; Present Bit, always 1 for a valid segment
                                                    ; Privilege (2b): privilige level required (Ring 0, 1, 2, 3)
                                                    ; Type: 0 for system segment, 1 for code or data segment
                                                    ; Executable: 0 for data segment, 1 for code segment
                                                    ; Direction/Confirming (code selectors):
                                                    ;   0: execution only allowed for specified ring privilege
                                                    ;   1: execution only allowed for equal/lower ring level,
                                                    ;      privilege remains but can read and execute the code
                                                    ;   (data selectors):
                                                    ;   0: Segments grows (ex: stack) up
                                                    ;   1: Segments grows down (offset > limit)
                                                    ; RW (code segments): 1 enables read (write never allowed)
                                                    ;    (data segments): 1 enables write (read always allowed)
                                                    ; Accessed: set by CPU when segment is accessed
    db 11001111b                                ; Flags (reversed in little endian), left to right:
                                                    ; Granularity: 1 to shl the limit value (with 1s, not 0s)
                                                    ; Size: 0 for 16b protected mode, 1 for 32b protected mode
                                                    ; Long-Mode: 1 for 64b protected mode (Size must be 0)
                                                    ; Limit (16-19 bits)
    db 0                                            ; Base (24-31 bits)

    ; 3rd entry: 32b data segment, flat memory model
gdt_selector_32pm_ds:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b                              ; access byte
    db 11001111b
    db 0

; 4th entry: 32bit user mode code semgents
gdt_selector_32pm_user_cs:
    dw 0xFFFF
    dw 0
    db 0
    db 0xFA ; access byte                             
    db 11001111b
    db 0

; 5th entry 32 bit user mode data segments
gdt_selector_32pm_user_ds:
    dw 0xFFFF
    dw 0
    db 0
    db 0xF2 ; access byte                             
    db 11001111b
    db 0
gdt_end:
; In order to load GDT, another structure is
; requried: the GDT descriptor.
; It contains GDT's size - 1 (2B) and address (4B).
global gdt_descriptor
gdt_descriptor:
    dw gdt_end - gdt - 1             ; Calculate size using address offsets
    dd gdt

section .text
global gdtFlush
gdtFlush:
    lgdt [gdt_descriptor]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.reload_cs

.reload_cs:
    ret
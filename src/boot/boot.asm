; NOTE: credit to https://github.com/Cotezzo/asm-protected-mode/ for the basic outline

; ==== MEMORY AND ARCH DIRECTIVES ============================================================================ ;
org 0x7c00
bits 16

; ==== BOOTLOADER MAIN ======================================================================================= ;
main:
    [bits 16]
    mov edi, 0xB8002

    mov ax, 'A'         ; ASCII 'A'
    mov ah, 0xF0       ; attribute byte: black background, green foreground
    mov [edi], ax        ; write both bytes

    ; Setup segment registers and stack pointer
    mov ax, 0
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov sp, 0x7C00

    ; load kernel
    call hdd_read

    mov edi, 0xB8002

    mov ax, 'B'         ; ASCII 'A'
    mov ah, 0xF0       ; attribute byte: black background, green foreground
    mov [edi], ax        ; write both bytes

    ; mov si, text_hdd_was_read
    ; call print16r

    call do_e820

    ret_ok:
    ; mov al, 'X'
    ; mov ah, 0x0E
    ; int 0x10

    ; mov si, text_mmap_was_read
    ; call print16r


    ; mov si, text16r_a20_enabled
    ; call print16r

    ; clear
    ; ==== SWITCH TO 32BIT PROTECTED MODE ==================================================================== ;
    cli

    ; store if the A20 Line is enabled in the E flag
    call a20_check
    cmp ax, 1

    ; if A20 is disabled, enable it and check again
    je .a20_enabled
    call a20_enable
    call a20_check
    cmp ax, 1
    je .a20_enabled

    ; print a20 failure
    ; mov si, text16r_a20_disabled
    ; call print16r
    hlt

    .a20_enabled:
    ; mov si, text16r_a20_enabled
    ; call print16r

    ; load gdt
    lgdt [gdt_descriptor]

    ; switch to pmode
    mov eax, cr0
    or al, 1
    mov cr0, eax

    mov edi, 0xB8002

    mov ax, 'C'         ; ASCII 'A'
    mov ah, 0xF0       ; attribute byte: black background, green foreground
    mov [edi], ax        ; write both bytes

    ; far jump to 32-bit code segment
    jmp 0x08:.32pm

    ; ==== 32BIT PROTECTED MODE ================== ;
    .32pm:
    [bits 32]

    ; Setup data segments for 32-bit flat memory
    mov ax, 0x10       ; GDT data segment
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x3F0000   

    ; mov edi, 0xB8002

    ; mov ax, 'D'         ; ASCII 'A'
    ; mov ah, 0xF0       ; attribute byte: black background, green foreground
    ; mov [edi], ax    

    call copy_data_to_high_half

    jmp 0x08:0x00100000

    ; mov edi, 0xB8002

    ; mov ax, 'E'         ; ASCII 'A'
    ; mov ah, 0xF0       ; attribute byte: black background, green foreground
    ; mov [edi], ax    

    ; ==== LOOP FOREVER ===================================================================== ;
    .loop:
        hlt
        jmp .loop


; ==== METHODS =============================================================================================== ;
; Methods called by the main:
; - print16r
; - print32p
; - a20_check
; - a20_enable
; ==== PRINT ===================================== ;
; Define special characters and strings to print.
; %define LF 0x0D
; %define CR 0x0A
; text16r_test: db `Real!`, LF, CR, 0
; text16r_a20_enabling: db `Enabling A20 Line...`, LF, CR, 0
; text16r_a20_disabled: db `Could not enable A20 Line`, LF, CR, 0
; text16r_a20_enabled: db `A20 Line Enabled`, LF, CR, 0
; text_hdd_was_not_read: db `Kernel not loaded into memory`, LF, CR, 0
; text_hdd_was_read: db `Kernel loaded into memory`, LF, CR, 0
; text_mmap_was_not_read: db `Failed to read MMAP.`, LF, CR, 0
; text_mmap_was_read: db `Loaded MMAP into memory.`, LF, CR, 0



; To move to a new line in VGA new lines, move
; cursor accordingly (buffer pointer), no CR/LF.
; New lines are used on an higher level to move
; into correct position.
%define VGA_TEXT_BUFFER 0xB8000
; text32p_test: db `Fake!`, 0   ; VGA, no CR/LF

;* Prints a string stored in memory to TTY.
;* Input:
;* - SI: string memory address
; print16r:
;     [bits 16]
;     push ax                                         ; Store AX
;     push bx                                         ; Store BX
;     push si                                         ; Store SI

;     mov ah, 0x0E                                    ; Interrupt method: 10, E: Write Character in TTY
;     mov bh, 0                                       ; Set page number used by int
;     .loop:                                          ; Loop start
;         lodsb                                       ; Load content of ds:[si] in AL, inc SI
;         test al, al                                 ; Check whether the character was NULL (last one)
;         jz .end                                     ; If it is (Z flag is set since AL is 0), exit

;         int 0x10                                    ; Call int and display the character in AL
;         jmp .loop

;     .end:
;     pop si                                          ; Restore previous SI value
;     pop bx                                          ; Restore previous BX value
;     pop ax                                          ; Restore previous AX value
;     ret                                             ; If Z flag is set, return (pop ip)

;* Prints a string stored in memory to TTY.
;* Input:
;* - SI: string memory address
; print32p:
;     [bits 32]
;     push eax
;     push esi                                         ; Store SI
;     push edi

;     mov edi, VGA_TEXT_BUFFER

;     mov ah, 0x01                                    ; Forecolor (4b) & Foreground (4b) color: Black, Green

;     .loop:                                          ; Loop start
;         lodsb                                       ; Load content of ds:[si] in AL, inc SI
;         test al, al                                 ; Check whether the character was NULL (last one)
;         jz .end                                     ; If it is (Z flag is set since AL is 0), exit

;         mov [edi], al                               ; Write loaded character to VGA buffer
;         inc edi                                     ; Move pointer to next byte
;         mov [edi], ah                               ; Write charcter colors to VGA buffer
;         inc edi                                     ; Move pointer to next byte
;         jmp .loop

;     .end:

;     pop edi
;     pop esi                                          ; Restore previous SI value
;     pop eax
;     ret                                             ; If Z flag is set, return (pop ip)

; ==== KEYBOARD CONTROLLER (PS/2 CONTROLLER) ===== ;
; To emulate old CPUs with 20 mem address bus (0-19)
; for retrocomp, 20th address line is forced to 0.
; In order to get full access to the memory bus,
; we need to disable this behaviour and enable it.
; The A20 gate is controlled by the motherboard's
; keyboard controller.
; https://wiki.osdev.org/%228042%22_PS/2_Controller

; I/O instructions:
; - in register, port ---> Receive data from port
; - out port, register --> Send data to port
; For I/O only the AX register can be used.

; Magic Numbers that idenfity the ports bound to the
; keyboard controller for data and command handling
%define PS2_PORT_DATA 0x60
%define PS2_PORT_COMMAND 0x64

; The keyboard controller handles specific commands
%define PS2_COMMAND_DISABLE_KEYBOARD 0xAD
%define PS2_COMMAND_ENABLE_KEYBOARD 0xAE
%define PS2_COMMAND_READ_FROM_OUTPUT 0xD0
%define PS2_COMMAND_WRITE_TO_OUTPUT 0xD1

;* Checks if the A20 Line is enabled.
;* Old BIOSes might not enable it by default.
;* Output:
;* - AX, 1 if the A20 line is enabled, 0 otherwise.
a20_check:
    [bits 16]
    ; In the bootloader, bytes 511 - 512 are 0xAA55:
    ; comparing [0:7DFE] to this value
    ; should always assert to true.
    ;> cmp byte [es:di], 0x55
    ;> cmp byte [es:di+1], 0xAA
    ; To check whether the A20 line wraps around or
    ; not, try to access the same address + 1MB and
    ; check the value against the one at [0:7DFE].
    ; If the address wraps around and the same
    ; values are accessed, the A20 line is disabled.

    ; Save used registers
    push es
    push di
    push ds
    push si
    
    ; Setup segments and index regs for mem access
    xor ax, ax
    mov es, ax                                      ; Set ES to 0x0000
    mov di, 0x7DFE                                  ; ES:DI = 0000:7DFE --> REAL ADDR: 0x007DFE

    not ax
    mov ds, ax                                      ; Set DS to 0xFFFF
    mov si, 0x7E0E                                  ; DS:SI = FFFF:7E0E --> REAL ADDR: 0x107DFE (ES:DI + 1MB)
                                                    ;  1    0    7    D    F    E
                                                    ; 0001 0000 0111 1101 1111 1110
                                                    ;    '-- 20th bit - if A20 line is disabled, this becomes 0

    ; Set return value to 1 by default
    mov ax, 1

    ; Lazy and insecure implementation:
    ; Read magic number from DS:SI
    ; If the comparison fails, exit
    ; ES:DI -----> 55
    ; ES:DI +1 --> AA
    ; DS:SI -----> 00   (If A20 already enabled)
    ; DS:SI +1 --> 00   (If A20 already enabled)
    ;> cmp byte [ds:si], 0x55
    ;> jne .exit ; Byte NOT equal, mem does NOT wrap

    ; Safe implementation:
    ; Clear previous ES:DI value with v1;
    ; Write v2 to DS:SI (different from v1);
    ; Compare ES:DI against v2.
    ; If memory wraps, modifying DS:SI affects
    ; ES:DI and the value is the same as v2.
    ; If not, A20 is already enabled (ES:DI value
    ; is still v1).
    ; The values previously stored at ES:DI and
    ; DS:SI are restored to their previous state.
    push word [es:di]
    push word [ds:si]
    mov byte [es:di], 0x00  ; Set ES:DI to v1
    mov byte [ds:si], 0xFF  ; Set DS:SI to v2
    cmp byte [es:di], 0xFF  ; Check ES:DI for v2
    pop word [ds:si]
    pop word [es:di]
    jne .exit   ; Byte NOT equal, mem does NOT wrap

    ; If no exit is called, all cmps set the E flag:
    ; A20 disabled, memory wraps around; ret 0
    xor ax, ax

    ; If exit is called, a cmp failed:
    ; A20 enabled, memory doesn't wrap around; ret 1
    ; 1 is the default value of ax, jumping here
    ; avoids the xor that sets ax to 1
    .exit:

    ; Restore used registers and return
    pop si
    pop ds
    pop di
    pop es
    ret

;* Uses the hardware I/O ports on the Keyboard 
;* Controller to try enabling the A20 Gate.
a20_enable:
    [bits 16]
    push ax

    ;! Before writing command to input buffer,
    ;! always wait for it to be empty first.

    ; Disable keyboard to avoid interruptions.
    call ps2_wait_ibuf_empty                        ; Wait for the input buffer to be empty
    mov al, PS2_COMMAND_DISABLE_KEYBOARD
    out PS2_PORT_COMMAND, al

    ; Send read command to get controller configuration
    ; byte.
    ; Wait until the config byte can be read from
    ; the output buffer, read it from data port
    ; and save to AL register.
    call ps2_wait_ibuf_empty                        ; Wait for the input buffer to be empty
    mov al, PS2_COMMAND_READ_FROM_OUTPUT
    out PS2_PORT_COMMAND, al
    call ps2_wait_obuf_full                         ; Wait for the output buffer to be full
    in al, PS2_PORT_DATA
    push ax                                         ; Save AL for later, it is used for I/O operations

    ; Send write command so that we can change
    ; the value of the config byte.
    call ps2_wait_ibuf_empty                        ; Wait for the input buffer to be empty
    mov al, PS2_COMMAND_WRITE_TO_OUTPUT
    out PS2_PORT_COMMAND, al

    ; Second bit of config byte toggles the A20
    ; line, set it and write new byte to DATA port.
    call ps2_wait_ibuf_empty                        ; Wait for the input buffer to be empty
    pop ax                                          ;! pop AFTER waiting, since wait does not restore AL value
    or al, 2
    out PS2_PORT_DATA, al

    ; Re-enable keyboard
    call ps2_wait_ibuf_empty                        ; Wait for the input buffer to be empty
    mov al, PS2_COMMAND_ENABLE_KEYBOARD
    out PS2_PORT_COMMAND, al

    ; Wait for process to end and restore registers
    call ps2_wait_ibuf_empty
    pop ax
    ret

;* Waits until the PS/2 input buffer can be used.
;! Uses AX register without restoring its state.
; The controller is slow, loop until the previous
; process finished and the input buffer is free.
ps2_wait_ibuf_empty:
    [bits 16]

    ; Command port returns the status register, it
    ; shows the controller's state with flags
    in al, PS2_PORT_COMMAND

    ; Second status register bit: input buffer state
    ; 0: the buffer is empty; 1: the buffer is full
    test al, 2  ; al & 2, discard result, set flags

    ; If the buffer is full, keep waiting
    jnz ps2_wait_ibuf_empty

    ; If the buffer is empty, return
    ret

;* Waits until the PS/2 output buffer is full.
;! Uses AX register without restoring its state.
; The controller is slow, loop until the previous
; process finished and the output buffer is filled.
ps2_wait_obuf_full:
    [bits 16]

    ; Command port returns the status register, it
    ; shows the controller's state with flags
    in al, PS2_PORT_COMMAND

    ; First status register bit: output buffer state
    ; 0: the buffer is empty; 1: the buffer is full
    test al, 1

    ; If the buffer is empty, keep waiting
    jz ps2_wait_obuf_full

    ; If the buffer is full, return
    ret

gdt:
    ; 1st entry: must be empty
    dq 0 ; NULL DESC

    ; 2nd entry: 32b code segment, flat memory model
.gdt_selector_32pm_cs:
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
.gdt_selector_32pm_ds:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b                              ; access byte
    db 11001111b
    db 0

; NOT NEEDED B/C GDT RELOADED LATER
; 4th entry: 32bit user mode code semgents
; .gdt_selector_32pm_user_cs:
;     dw 0xFFFF
;     dw 0
;     db 0
;     db 0xFA ; access byte                             
;     db 11001111b
;     db 0

; ; 5th entry 32 bit user mode data segments
; .gdt_selector_32pm_user_ds:
;     dw 0xFFFF
;     dw 0
;     db 0
;     db 0xF2 ; access byte                             
;     db 11001111b
;     db 0


; In order to load GDT, another structure is
; requried: the GDT descriptor.
; It contains GDT's size - 1 (2B) and address (4B).
gdt_descriptor:
    dw gdt_descriptor - gdt - 1             ; Calculate size using address offsets
    dd gdt


; CREDIT TO THE OS DEV WIKI AS I AM USING THIS
; E280 BIOS MAP BECAUSE MINE DIDN'T CATCH ALL OF THE EDGE CASES
mmap_ent equ 0x8000             ; the number of entries will be stored at 0x8000

do_e820:
    mov di, 0x5000
	xor ebx, ebx		; ebx = 0
	xor bp, bp		; entry count in bp
	mov edx, 0x0534D4150	; "SMAP"
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24
	int 0x15
	jc short .failed	; carry on first call -> unsupported function
	mov edx, 0x0534D4150	
	cmp eax, edx		; success = eax must have been set to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 -> fail
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext ; jmp if below or equl, 20 byte response
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov [es:mmap_ent], bp	; store the entry count
	clc			
	ret
.failed:
	stc			
	ret




hdd_read:
    [bits 16]

   mov edi, 0xB8000     ; start of VGA text memory

    ; Write character 'A' in green on black, TO SHOW AN ERROR
    mov ax, KERN_SECTORS          ; ASCII 'A'
    mov ah, 0xF0        ; attribute byte: black background, green foreground
    mov [edi], ax        ; write both bytes

    mov ax, 0x0000
    mov es, ax              ; destination segment
    mov bx, 0x1000          ; destination offset

    mov si, KERN_SECTORS               ; number of sectors to read
    mov ch, 0               ; cylinder 0
    mov dh, 0               ; head 0
    mov cl, 2               ; start at sector 2

    mov ah, 0x02            ; BIOS read
    ; mov al, KERN_SECTORS  
    mov al, 43
    ; add al, 10      ; FOR TESTING    ; read 1 sector
    int 0x13
    jc disk_error

    ; mov edi, 0xB8000 ; FOR VRAM PURPOSES

    ; call read_loop

    ret

; read_loop:
;     [bits 16]
;     mov ah, 0x02            ; BIOS read
;     mov al, 0x01            ; read 1 sector
;     int 0x13
;     jc disk_error           ; carry flag = error

;     mov ax, 'A'         ; ASCII 'A'
;     mov ah, 0xF0       ; attribute byte: black background, green foreground
;     mov [edi], ax        ; write both bytes
;     inc edi
;     inc edi

;     add bx, 512             ; next memory location
;     inc cl                  ; next sector
;     cmp cl, 18
;     jle noSectorOverflow

;     ; ; if theres a sector overflwo
;     inc dh
;     mov cl, 0x01

;     noSectorOverflow: 
;     dec si
;     jnz read_loop

;     ret

disk_error:
    [bits 16]

    mov edi, 0xB8002     ; start of VGA text memory

    ; ; Write character 'A' in green on black, TO SHOW AN ERROR
    ; mov si, 'A'
    
    mov ax, 'A'         ; ASCII 'A'
    mov ah, 0xF0       ; attribute byte: black background, green foreground
    mov [edi], ax        ; write both bytes

    ; handle error here
    ; mov si, text_hdd_was_not_read
    ; call print16r
    hlt

copy_data_to_high_half:
    [bits 32]
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    mov ebx, 0x1000
    mov ecx, 0x100000
    call cpy_loop
    ret

cpy_loop:
    [bits 32]
    mov al, [ebx]
    mov [ecx], al
    inc ebx
    inc ecx
    cmp ebx, 0x10000
    jne cpy_loop
    ret


; mmap_err:
;     [bits 16]
;     mov al, 'E'
;     mov ah, 0x0E
;     int 0x10
;     ; mov si, text_mmap_was_not_read
;     ; call print16r
;     hlt


; ==== PADDING AND SIGNATURE ================================================================================= ;
times 510-($-$$) db 0
dw 0xAA55    

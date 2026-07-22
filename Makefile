.PHONY: all run clean

KERNEL := src/pmodekernel/kernel
MEM := $(KERNEL)/memory
INTR := $(KERNEL)/interrupts
IO := $(KERNEL)/io
TIME := $(KERNEL)/time
PROC := $(KERNEL)/processes
DRV := $(KERNEL)/genericDrivers
FS := $(KERNEL)/filesystem
FSKAPI := $(KERNEL)/fskernelapi
STDC := $(KERNEL)/standardLibraries
PROC := $(KERNEL)/processes
BOOT := src/boot
TMP := tmp
OUT := target

all: $(OUT)/freedos.img

run: $(OUT)/freedos.img
	qemu-system-i386 -m 4000 -rtc base=localtime -hda $(OUT)/freedos.img -boot d

$(OUT)/freedos.img: $(TMP)/kernel.bin $(TMP)/boot.com | $(OUT)
	@if [ "$$EUID" -ne 0 ]; then \
        echo "To flash freedos.img, make must be run as root"; \
        exit 1; \
    fi
	
	mkdir -p /mnt/freedos
	mount -t msdos -o loop,offset=32256 $(OUT)/freedos.img /mnt/freedos
	rm -f /mnt/freedos/custom/boot.com
	rm -f /mnt/freedos/custom/kernel.bin
	cp $(TMP)/boot.com /mnt/freedos/custom
	cp $(TMP)/kernel.bin /mnt/freedos/custom
	umount /mnt/freedos

$(TMP)/boot.com: $(BOOT)/main.asm | $(TMP)
	nasm -f bin $< -o $@

$(TMP)/kernel.bin: $(TMP)/kernel.elf
	objcopy -O binary $(TMP)/kernel.elf $(TMP)/kernel.bin

$(TMP)/kernel.elf: $(TMP)/kernel.o $ $(TMP)/memSetup.o $(TMP)/kalloc.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/initInterruptHandlers.o $(TMP)/pic.o $(TMP)/e820.o $(TMP)/pmm.o $(TMP)/contHighHalfSetup.o $(TMP)/vmm.o $(TMP)/gdt.o $(TMP)/time.o $(TMP)/pitIntr.o $(TMP)/ps2keyboard.o $(TMP)/atapio.o $(TMP)/fat_access.o $(TMP)/fat_cache.o $(TMP)/fat_filelib.o $(TMP)/fat_format.o $(TMP)/fat_misc.o $(TMP)/fat_string.o $(TMP)/fat_table.o $(TMP)/fat_write.o $(TMP)/stdio.o $(TMP)/string.o $(TMP)/stdlib.o $(TMP)/atlock.o $(TMP)/contextSwitch.o $(TMP)/contextSwitchAsm.o $(TMP)/process.o $(TMP)/yield.o  $(TMP)/pageFault.o $(TMP)/syscall.o $(TMP)/cmd.o $(TMP)/loadfile.o $(KERNEL)/linker.ld 
	ld -m elf_i386 -T $(KERNEL)/linker.ld -o $(TMP)/kernel.elf $(TMP)/kernel.o $(TMP)/memSetup.o $(TMP)/kalloc.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/initInterruptHandlers.o $(TMP)/e820.o $(TMP)/pmm.o $(TMP)/pic.o $(TMP)/contHighHalfSetup.o $(TMP)/vmm.o $(TMP)/gdt.o $(TMP)/time.o $(TMP)/pitIntr.o $(TMP)/ps2keyboard.o $(TMP)/atapio.o $(TMP)/fat_access.o $(TMP)/fat_cache.o $(TMP)/fat_filelib.o $(TMP)/fat_format.o $(TMP)/fat_misc.o $(TMP)/fat_string.o $(TMP)/fat_table.o $(TMP)/fat_write.o $(TMP)/stdio.o $(TMP)/string.o $(TMP)/stdlib.o $(TMP)/atlock.o $(TMP)/contextSwitch.o $(TMP)/contextSwitchAsm.o $(TMP)/process.o $(TMP)/yield.o $(TMP)/pageFault.o $(TMP)/syscall.o $(TMP)/cmd.o $(TMP)/loadfile.o

$(TMP)/pitIntr.o: $(TIME)/pitIntr.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/atapio.o: $(DRV)/atapio.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/time.o: $(TIME)/time.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/ps2keyboard.o: $(DRV)/ps2keyboard.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/gdt.o: $(KERNEL)/gdt.asm | $(TMP)
	nasm -f elf32 $< -o $@

$(TMP)/contextSwitchAsm.o: $(PROC)/contextSwitch.asm | $(TMP)
	nasm -f elf32 $< -o $@

$(TMP)/vmm.o: $(MEM)/vmm.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/e820.o: $(MEM)/e820.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/contHighHalfSetup.o: $(MEM)/contHighHalfSetup.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/pmm.o: $(MEM)/pmm.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/kernel.o: $(KERNEL)/kernel.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/kalloc.o: $(MEM)/kalloc.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/memSetup.o: $(MEM)/memSetup.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/logging.o: $(KERNEL)/logging.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/idt.o: $(INTR)/idt.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/pic.o: $(INTR)/pic.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/iolibrary.o: $(IO)/iolibrary.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/initInterruptHandlers.o: $(INTR)/initInterruptHandlers.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@


$(TMP)/intDispatchers.o: $(INTR)/intDispatchers.asm | $(TMP)
	nasm -f elf32 $< -o $@

$(TMP)/fat_access.o: $(FS)/fat_access.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_cache.o: $(FS)/fat_cache.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_filelib.o: $(FS)/fat_filelib.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_format.o: $(FS)/fat_format.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_misc.o: $(FS)/fat_misc.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_string.o: $(FS)/fat_string.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_table.o: $(FS)/fat_table.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/fat_write.o: $(FS)/fat_write.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/stdio.o: $(STDC)/stdio.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/pageFault.o: $(MEM)/pageFault.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/string.o: $(STDC)/string.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/stdlib.o: $(STDC)/stdlib.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/atlock.o: $(PROC)/atlock.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/contextSwitch.o: $(PROC)/contextSwitch.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/process.o: $(PROC)/process.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/yield.o: $(PROC)/yield.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/syscall.o: $(INTR)/syscall.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/cmd.o: $(KERNEL)/cmd.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP)/loadfile.o: $(FSKAPI)/loadfile.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -Isrc/pmodekernel/include -c $< -o $@

$(TMP):
	mkdir -p $(TMP)

$(OUT):
	mkdir -p $(OUT)

clean:
	rm -rf $(TMP)
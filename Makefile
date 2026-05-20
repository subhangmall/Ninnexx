.PHONY: all run clean

KERNEL := src/pmodekernel/kernel
MEM := $(KERNEL)/memory
INTR := $(KERNEL)/interrupts
IO := $(KERNEL)/io
BOOT := src/boot
TMP := tmp
OUT := target

all: $(OUT)/floppy.img

run: $(OUT)/floppy.img
	qemu-system-i386 -m 4000 -fda $(OUT)/floppy.img

$(OUT)/floppy.img: $(TMP)/kernel.bin $(TMP)/boot.bin | $(OUT)
	dd if=/dev/zero of=$(OUT)/floppy.img bs=512 count=2880
	dd if=$(TMP)/boot.bin of=$(OUT)/floppy.img conv=notrunc
	dd if=$(TMP)/kernel.bin of=$(OUT)/floppy.img bs=512 seek=1 conv=notrunc

$(TMP)/boot.bin: $(BOOT)/boot.asm $(TMP)/kernel.bin | $(TMP)
	@S=$$(( ($$(stat -c%s $(TMP)/kernel.bin) + 511) / 512 )); \
	nasm -f bin $(BOOT)/boot.asm -D KERN_SECTORS=$$S -o $(TMP)/boot.bin

$(TMP)/kernel.bin: $(TMP)/kernel.elf
	objcopy -O binary $(TMP)/kernel.elf $(TMP)/kernel.bin

# $(TMP)/kernel.elf: $(TMP)/kernel.o $(TMP)/memSetup.o $(TMP)/kmemmgt.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/pic.o $(TMP)/initInterruptHandlers.o $(KERNEL)/linker.ld 
# 	ld -m elf_i386 -T $(KERNEL)/linker.ld -o $(TMP)/kernel.elf $(TMP)/kernel.o $(TMP)/memSetup.o $(TMP)/kmemmgt.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/pic.o $(TMP)/initInterruptHandlers.o

$(TMP)/kernel.elf: $(TMP)/kernel.o $ $(TMP)/memSetup.o $(TMP)/kalloc.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/initInterruptHandlers.o $(TMP)/pic.o $(TMP)/e820.o $(TMP)/pmm.o $(TMP)/contHighHalfSetup.o $(TMP)/vmm.o $(TMP)/gdt.o $(KERNEL)/linker.ld 
	ld -m elf_i386 -T $(KERNEL)/linker.ld -o $(TMP)/kernel.elf $(TMP)/kernel.o $(TMP)/memSetup.o $(TMP)/kalloc.o $(TMP)/logging.o $(TMP)/idt.o $(TMP)/intDispatchers.o $(TMP)/iolibrary.o $(TMP)/initInterruptHandlers.o $(TMP)/e820.o $(TMP)/pmm.o $(TMP)/pic.o $(TMP)/contHighHalfSetup.o $(TMP)/vmm.o $(TMP)/gdt.o

# $(TMP)/kernel.elf: $(TMP)/kernel.o $(TMP)/memSetup.o $(TMP)/logging.o $(TMP)/kmemmgt.o $(KERNEL)/linker.ld
# 	ld -m elf_i386 -T $(KERNEL)/linker.ld -o $(TMP)/kernel.elf $(TMP)/kernel.o $(TMP)/logging.o $(TMP)/memSetup.o $(TMP)/kmemmgt.o

$(TMP)/gdt.o: $(KERNEL)/gdt.asm | $(TMP)
	nasm -f elf32 $< -o $@

$(TMP)/vmm.o: $(MEM)/vmm.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/e820.o: $(MEM)/e820.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/contHighHalfSetup.o: $(MEM)/contHighHalfSetup.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/pmm.o: $(MEM)/pmm.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/kernel.o: $(KERNEL)/kernel.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/kalloc.o: $(MEM)/kalloc.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/memSetup.o: $(MEM)/memSetup.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/logging.o: $(KERNEL)/logging.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/idt.o: $(INTR)/idt.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/pic.o: $(INTR)/pic.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/iolibrary.o: $(IO)/iolibrary.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@

$(TMP)/initInterruptHandlers.o: $(INTR)/initInterruptHandlers.c | $(TMP)
	gcc -m32 -ffreestanding -fno-stack-protector -c $< -o $@


$(TMP)/intDispatchers.o: $(INTR)/intDispatchers.asm | $(TMP)
	nasm -f elf32 $< -o $@

$(TMP):
	mkdir -p $(TMP)

$(OUT):
	mkdir -p $(OUT)

clean:
	rm -rf $(TMP) $(OUT)
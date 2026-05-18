#ifndef COMMONMACROS_H
#define COMMONMACROS_H

#define NULL ((void*)0)

#define MAX_REGIONS 1024
#define AVAILABLE_MEM_E820 1

#define PAGE_SIZE 4096

#define PMM_UNAVAILABLE true
#define PMM_AVAILABLE false

#define ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define ALIGN_UP(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

#define MMIO_VIRTUAL_SPACE_BASE 0xE0000000
#define MMIO_VIRTUAL_SPACE_SIZE 0x10000000

#define KERNEL_PAGE_DIRECTORY ((struct PageDirectoryEntry*) KERNEL_PAGE_DIRECTORY_ADDR) 
#define KERNEL_PAGE_DIRECTORY_ADDR 0xFFFFF000
#define RECURSIVE_PT_ADDR 0xFFC00000

#define VMM_PRESENT    0x1
#define VMM_WRITABLE   0x2
#define VMM_USER       0x4
#define VMM_MMIO       0x8

#endif
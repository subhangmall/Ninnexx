#include <stdint.h>

#ifndef KERNEL_H
#define KERNEL_H

#define NULL ((void*)0)
#define DEFAULT_KRNL_STACK_TOP_VIRT 0x3F0000 
__attribute__((section(".boot"))) void kentry(uint32_t dummyCS, uint32_t e820LenAddr, uint32_t e820StartAddress) ;

#endif
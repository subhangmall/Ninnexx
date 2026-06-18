ENTRY(kentry)      

KRNL_BASE = 0xC0000000;

SECTIONS {
  . = 0x100000;       /* Load address for kernel in memory */

  .boot : {
    *(.boot)
  }

  .boot.data : {
    *(.boot.data)
  }

  . += KRNL_BASE;

  /* . = 0xC0001000 + _higher_half_lma - 0x1000; */
  .text : AT(ADDR(.text) - KRNL_BASE) 
   { *(.text) }   
  .rodata : AT(ADDR(.rodata) - KRNL_BASE)
   { *(.rodata) } 
  .data : AT(ADDR(.data) - KRNL_BASE)
   { *(.data) }   
  .bss : AT(ADDR(.bss)- KRNL_BASE)
   { 
    *(.bss*) 
    *(COMMON)
   }    

   /DISCARD/ : {
    *(.note*)
    *(.comment)
    *(.eh_frame*)
   } 
}
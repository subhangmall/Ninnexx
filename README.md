[Work in Progress]

Hobbyist 32 bit kernel that bootstraps off of DOS and aims to be able to virtualize DOS in the operating system using v8086.

Completed: DOS bootstrap, E820, paging, GDT, IDT, malloc, sleep subsystem, FAT, PS2 keyboard, PMM/VMM, logging, processes

In Progress: higher half remapping, reimplementing essential C standard library functions, implementing concurrency

Future: v8086, returning to DOS

NOTE: the fat filesystem is copied from https://github.com/ultraembedded/fat_io_lib.

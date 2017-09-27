#ifndef VIC_20_H
#define VIC_20_H

#include "cpu.h"
#include "memory.h"
#include "vic.h"

enum memory_extensions {
    RAM_3K   =  1,
    RAM_8K_1 =  2,
    RAM_8K_2 =  4,
    RAM_8K_3 =  8,
    ROM_8K   = 16,
};

extern void load_rom_from_array(const unsigned *source, word address, word length);
extern void vic20_config(unsigned extensions);

#endif
#include "VIC-20.h"

extern unsigned character_rom[];
extern unsigned basic_rom[];
extern unsigned kernal_rom[];


void load_rom_from_array(const unsigned *source, word address, word length) {
    for(byte *memory = memory_get_ptr(address); length > 0; length -= 4) {
        unsigned u = *source++;
        *memory++ = u & 0xFF;
        u >>= 8;
        *memory++ = u & 0xFF;
        u >>= 8;
        *memory++ = u & 0xFF;
        u >>= 8;
        *memory++ = u & 0xFF;
    }
}

void vic20_config(enum memory_extensions extensions) {
    memory_setPageType(0x00, 0x03, MEM_READ | MEM_WRITE); /* 1K Low RAM */
    memory_setPageType(0x10, 0x1F, MEM_READ | MEM_WRITE); /* 4K Main RAM Screen memory 0x10-0x1D (>=8K Expansion RAM) or 0x1e-0x1F (standard)*/
    if(extensions & RAM_8K_1)
        memory_setPageType(0x20, 0x3F, MEM_READ); /* 8K Expansion RAM-1 */
    if(extensions & RAM_8K_1)
        memory_setPageType(0x40, 0x5F, MEM_READ); /* 8K Expansion RAM-2 */
    if(extensions & RAM_8K_1)
        memory_setPageType(0x60, 0x7F, MEM_READ); /* 8K Expansion RAM-3 */
    memory_setPageType(0x80, 0x8F, MEM_READ); /* 4K Character ROM */
    load_rom_from_array(character_rom, 0x8000, 0x1000);
    memory_setPageType(0x90, 0x90, MEM_IO | IO_VIC); /* 6561 VIC-1 I/O page */
    memory_setPageType(0x91, 0x91, MEM_IO | IO_VIA); /* 6522 VIA (2x) I/O page */
    if(extensions == RAM_NONE || extensions == RAM_3K)
        memory_setPageType(0x96, 0x97, MEM_READ | MEM_WRITE); /* 0,5K Colour RAM (standard)*/
    else
        memory_setPageType(0x94, 0x95, MEM_READ | MEM_WRITE); /* 0,5K Colour RAM (>=8K Expansion RAM)*/
    memory_setPageType(0xA0, 0xBF, MEM_READ); /* Expansion ROM */
    memory_setPageType(0xC0, 0xDF, MEM_READ); /* Basic ROM */
    load_rom_from_array(basic_rom, 0xC000, 0x2000);
    memory_setPageType(0xE0, 0xFF, MEM_READ); /* Kernel ROM */
    load_rom_from_array(kernal_rom, 0xE000, 0x2000);

    cpu_reset();
    vic_reset();
    via_reset();
}
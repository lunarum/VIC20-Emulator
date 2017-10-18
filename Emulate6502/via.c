#include "via.h"

byte *memory;

void via_reset() {
    memory = memory_get_ptr(0);
}

byte via_read_register(word address) {
    switch(address) {
        case 0x9003:    return (memory[0x9003] & 0x7F) | ((0 & 1) << 7);
        case 0x9004:    return (0 >> 1);
    }
    return memory[address];
}

void via_write_register(word address, byte value) {
    switch(address) {
        case 0x9000:
            break;
    }
    // save for read
    memory[address] = value;
}

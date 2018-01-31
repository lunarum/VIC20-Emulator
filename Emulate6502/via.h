#ifndef VIA_H
#define VIA_H

#include "memory.h"

extern struct via_registers {
    word T1C;    // Timer-1 Counter
    word T1L;    // Timer-1 Latch
    word T2C;    // Timer-2 Counter
    byte T2L;    // Timer-2 Latch
    byte SR;     // Shift Register
    byte ACR;    // Auxilary Control Register
    byte PCR;    // Peripheral Control Register
    byte IFR;    // Interrupt Flag Register
    byte IER;    // Interrupt Enable Register
} via1, via2;

extern byte via_read_register(word address);
extern void via_write_register(word address, byte value);
extern void via_reset();
extern bool via_count_NMI(byte cycles);
extern bool via_count_IRQ(byte cycles);

#endif
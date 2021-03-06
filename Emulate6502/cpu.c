#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

struct cpu_registers cpu;
struct counters counter[MAX_COUNTERS];

void cpu_reset() {
    cpu.SP = MEM_PAGE_SIZE;
    cpu.PC = memory_get_vector(MEM_RESET);
    cpu.cycles = 0;
    for(unsigned u = 0; u < MAX_COUNTERS; ++u) {
        counter[u].signal = NULL;
        counter[u].counter = 0;
    }
}

void cpu_statusPush(void) {
    byte value = cpu_get_state();
    memory_stackPush(value);
}

void cpu_statusPull(void) {
    byte value = memory_stackPull();
    cpu.PS_N = ((value & 0x80) != 0);
    cpu.PS_V = ((value & 0x40) != 0);
    cpu.PS_B = ((value & 0x10) != 0);
    cpu.PS_D = ((value & 0x08) != 0);
    cpu.PS_I = ((value & 0x04) != 0);
    cpu.PS_Z = ((value & 0x02) != 0);
    cpu.PS_C = ((value & 0x01) != 0);
}

void cpu_disassemble(word address) {
    for(int n = 0; n < 8; ++n) {
        byte b1, b2, b3, b4,
            opcode = memory_get(address);
        word address2;
        int offset;
        
        fprintf(stderr, "%c%04X %s ", (address == cpu.PC ? '>' : ' '), address, opcodes[opcode].mnemonic);
        ++address;

        switch(opcodes[opcode].access) {
            case AM_IMM: /* Immediate: #aa */
                b1 = memory_get(address++);
                fprintf(stderr, "#%02X", (int)b1);
                break;
            case AM_ABA: /* Absolute: aaaa */
            case AM_ABS: /* Absolute: aaaa */
                b1 = memory_get(address++);
                b2 = memory_get(address++);
                fprintf(stderr, "%02X%02X", (int)b2, (int)b1);
                break;
            case AM_ZPG: /* Zeo Page: aa */
                b1 = memory_get(address++);
                fprintf(stderr, "%02X", (int)b1);
                break;
            case AM_IMP: /* Implied */
                break;
            case AM_IND: /* Indirect Absolute: (aaaa) */
                b1 = memory_get(address++);
                b2 = memory_get(address++);
                fprintf(stderr, "(%02X%02X)", (int)b2, (int)b1);
                break;
            case AM_AIX: /* Absolute Indexed,X: aaaa,X */
                b1 = memory_get(address++);
                b2 = memory_get(address++);
                fprintf(stderr, "%02X%02X,X", (int)b2, (int)b1);
                if(address == cpu.PC) {
                    address2 = getAddress(b1, b2) + cpu.X;
                    fprintf(stderr, " => %04X", (int)address2);
                }
                break;
            case AM_AIY: /* Absolute Indexed,Y: aaaa,Y */
                b1 = memory_get(address++);
                b2 = memory_get(address++);
                fprintf(stderr, "%02X%02X,Y", (int)b2, (int)b1);
                if(address == cpu.PC) {
                    address2 = getAddress(b1, b2) + cpu.Y;
                    fprintf(stderr, " => %04X", (int)address2);
                }
            case AM_ZIX: /* Zero Page Indexed,X: aa,X */
                b1 = memory_get(address++);
                fprintf(stderr, "%02X,X", (int)b1);
                if(address == cpu.PC) {
                    b2 = b1 + cpu.X;
                    fprintf(stderr, " => %02X", (int)b2);
                }
                break;
            case AM_ZIY: /* Zero Page Indexed,Y: aa,Y */
                b1 = memory_get(address++);
                fprintf(stderr, "%02X,Y", (int)b1);
                if(address == cpu.PC) {
                    b2 = b1 + cpu.Y;
                    fprintf(stderr, " => %02X", (int)b2);
                }
                break;
            case AM_IIX: /* Indexed Indirect: (aa,X) */
                b1 = memory_get(address++);
                fprintf(stderr, "(%02X,X)", (int)b1);
                if(address == cpu.PC) {
                    b2 = b1 + cpu.X;
                    b3 = memory_getZero(b2++),
                    b4 = memory_getZero(b2);
                    address2 = getAddress(b3, b4);
                    fprintf(stderr, " => %04X", (int)address2);
                }
                break;
            case AM_IIY: /* Indirect Indexed: (aa),Y */
                b1 = memory_get(address++);
                fprintf(stderr, "(%02X),Y", (int)b1);
                if(address == cpu.PC) {
                    b3 = memory_getZero(b1++),
                    b4 = memory_getZero(b1);
                    address2 = getAddress(b3, b4) + cpu.Y;
                    fprintf(stderr, " => %04X", (int)address2);
                }
                break;
            case AM_REL: /* Relative: aaaa */
                b1 = memory_get(address++);
                offset = (b1 & 0x80) ? /* Two's complement negative number */ b1 - 256 : b1;
                address2 = cpu.PC + 1 + offset;
                fprintf(stderr, "%02X => %04X", (int)b1, (int)address2);
                break;
        }
        fprintf(stderr, "\n");
    }
}

void cpu_logStatus(cpu_result result) {
    fprintf(stderr, "A:%02X X:%02X Y:%02X SP:%02X PC:%04X PS:%c%c-%c%c%c%c%c - %1X\n",
        (unsigned)cpu.A, (unsigned)cpu.X, (unsigned)cpu.Y, (unsigned)cpu.SP, (unsigned)cpu.PC,
        cpu.PS_N ? 'N' : '-',
        cpu.PS_V ? 'V' : '-',
        cpu.PS_B ? 'B' : '-',
        cpu.PS_D ? 'D' : '-',
        cpu.PS_I ? 'I' : '-',
        cpu.PS_Z ? 'Z' : '-',
        cpu.PS_C ? 'C' : '-',
        (unsigned)result);
}

#include "cpu.h"
#include "memory.h"
#include "via.h"


bool single_step = false;

cpu_result cpu_run() {
    bool tmp_PS_C;
    byte opcode, value;
    word value_w, value_w2;
    unsigned count_tmp;
    void (*signal_func)();

    for(;;) {
        opcode = memory_getImmediate();
        switch(opcode) {
        case 0x00: /* BRK  */
            cpu.cycles = 7;
            memory_stackPushAddress(cpu.PC+1);
            cpu.PS_B = true;
            cpu_statusPush();
            cpu.PS_I = true;
            cpu.PC = memory_get_vector(MEM_IRQ_BREAK);
            break;
        case 0x01: /* ORA (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x02: KIL  */
        /* Illegal opcode 0x03: SLO (aa,X) */
        /* Illegal opcode 0x04: NOP aa */
        case 0x05: /* ORA aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x06: /* ASL aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            cpu.PS_C = ((value & 0x80) != 0);
            value <<= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x07: SLO aa */
        case 0x08: /* PHP  */
            cpu.cycles = 3;
            cpu_statusPush();
            break;
        case 0x09: /* ORA #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x0A: /* ASL  */
            cpu.cycles = 2;
            cpu.PS_C = ((cpu.A & 0x80) != 0);
            cpu.A <<= 1;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x0B: ANC #aa */
        /* Illegal opcode 0x0C: NOP aaaa */
        case 0x0D: /* ORA aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x0E: /* ASL aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            cpu.PS_C = ((value & 0x80) != 0);
            value <<= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x0F: SLO aaaa */
        case 0x10: /* BPL aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_N) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x11: /* ORA (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x12: KIL  */
        /* Illegal opcode 0x13: SLO (aa),Y */
        /* Illegal opcode 0x14: NOP aa,X */
        case 0x15: /* ORA aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x16: /* ASL aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            cpu.PS_C = ((value & 0x80) != 0);
            value <<= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x17: SLO aa,X */
        case 0x18: /* CLC  */
            cpu.cycles = 2;
            cpu.PS_C = false;
            break;
        case 0x19: /* ORA aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x1A: NOP  */
        /* Illegal opcode 0x1B: SLO aaaa,Y */
        /* Illegal opcode 0x1C: NOP aaaa,X */
        case 0x1D: /* ORA aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            cpu.A |= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x1E: /* ASL aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            cpu.PS_C = ((value & 0x80) != 0);
            value <<= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x1F: SLO aaaa,X */
        case 0x20: /* JSR aaaa */
            cpu.cycles = 6;
            value_w = memory_getAbsoluteAddress();
            memory_stackPushAddress(cpu.PC - 1);
            cpu.PC = value_w;
            break;
        case 0x21: /* AND (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x22: KIL  */
        /* Illegal opcode 0x23: RLA (aa,X) */
        case 0x24: /* BIT aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_V = ((value & 0x40) != 0);
            cpu.PS_Z = ((value & cpu.A) != 0);
            break;
        case 0x25: /* AND aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x26: /* ROL aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x80) != 0);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x27: RLA aa */
        case 0x28: /* PLP  */
            cpu.cycles = 4;
            cpu_statusPull();
            break;
        case 0x29: /* AND #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x2A: /* ROL  */
            cpu.cycles = 2;
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((cpu.A & 0x80) != 0);
            cpu.A = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x2B: ANC #aa */
        case 0x2C: /* BIT aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_V = ((value & 0x40) != 0);
            cpu.PS_Z = ((value & cpu.A) != 0);
            break;
        case 0x2D: /* AND aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x2E: /* ROL aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x80) != 0);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x2F: RLA aaaa */
        case 0x30: /* BMI aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(cpu.PS_N) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x31: /* AND (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x32: KIL  */
        /* Illegal opcode 0x33: RLA (aa),Y */
        /* Illegal opcode 0x34: NOP aa,X */
        case 0x35: /* AND aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x36: /* ROL aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x80) != 0);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x37: RLA aa,X */
        case 0x38: /* SEC  */
            cpu.cycles = 2;
            cpu.PS_C = true;
            break;
        case 0x39: /* AND aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x3A: NOP  */
        /* Illegal opcode 0x3B: RLA aaaa,Y */
        /* Illegal opcode 0x3C: NOP aaaa,X */
        case 0x3D: /* AND aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            cpu.A &= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x3E: /* ROL aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x80) != 0);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x3F: RLA aaaa,X */
        case 0x40: /* RTI  */
            cpu.cycles = 6;
            cpu_statusPull();
            cpu.PC = memory_stackPullAddress();
            break;
        case 0x41: /* EOR (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x42: KIL  */
        /* Illegal opcode 0x43: SRE (aa,X) */
        /* Illegal opcode 0x44: NOP aa */
        case 0x45: /* EOR aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x46: /* LSR aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            cpu.PS_C = ((value & 0x01) != 0);
            value >>= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x47: SRE aa */
        case 0x48: /* PHA  */
            cpu.cycles = 3;
            memory_stackPush(cpu.A);
            break;
        case 0x49: /* EOR #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x4A: /* LSR  */
            cpu.cycles = 2;
            cpu.PS_C = ((cpu.A & 0x01) != 0);
            cpu.A >>= 1;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x4B: ALR #aa */
        case 0x4C: /* JMP aaaa */
            cpu.cycles = 3;
            cpu.PC = memory_getAbsoluteAddress();
            break;
        case 0x4D: /* EOR aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x4E: /* LSR aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            cpu.PS_C = ((value & 0x01) != 0);
            value >>= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x4F: SRE aaaa */
        case 0x50: /* BVC aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_V) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x51: /* EOR (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x52: KIL  */
        /* Illegal opcode 0x53: SRE (aa),Y */
        /* Illegal opcode 0x54: NOP aa,X */
        case 0x55: /* EOR aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x56: /* LSR aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            cpu.PS_C = ((value & 0x01) != 0);
            value >>= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x57: SRE aa,X */
        case 0x58: /* CLI  */
            cpu.cycles = 2;
            cpu.PS_I = false;
            break;
        case 0x59: /* EOR aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x5A: NOP  */
        /* Illegal opcode 0x5B: SRE aaaa,Y */
        /* Illegal opcode 0x5C: NOP aaaa,X */
        case 0x5D: /* EOR aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            cpu.A ^= value;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x5E: /* LSR aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            cpu.PS_C = ((value & 0x01) != 0);
            value >>= 1;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x5F: SRE aaaa,X */
        case 0x60: /* RTS  */
            cpu.cycles = 6;
            cpu.PC = memory_stackPullAddress() + 1;
            break;
        case 0x61: /* ADC (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        /* Illegal opcode 0x62: KIL  */
        /* Illegal opcode 0x63: RRA (aa,X) */
        /* Illegal opcode 0x64: NOP aa */
        case 0x65: /* ADC aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        case 0x66: /* ROR aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x01) != 0);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x67: RRA aa */
        case 0x68: /* PLA  */
            cpu.cycles = 4;
            cpu.A = memory_stackPull();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x69: /* ADC #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        case 0x6A: /* ROR  */
            cpu.cycles = 2;
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((cpu.A & 0x01) != 0);
            cpu.A = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x6B: ARR #aa */
        case 0x6C: /* JMP (aaaa) */
            cpu.cycles = 5;
            cpu.PC = memory_getIndirectAbsoluteAddress();
            break;
        case 0x6D: /* ADC aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        case 0x6E: /* ROR aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x01) != 0);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x6F: RRA aaaa */
        case 0x70: /* BVS aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(cpu.PS_V) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x71: /* ADC (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        /* Illegal opcode 0x72: KIL  */
        /* Illegal opcode 0x73: RRA (aa),Y */
        /* Illegal opcode 0x74: NOP aa,X */
        case 0x75: /* ADC aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        case 0x76: /* ROR aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x01) != 0);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x77: RRA aa,X */
        case 0x78: /* SEI  */
            cpu.cycles = 2;
            cpu.PS_I = true;
            break;
        case 0x79: /* ADC aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        /* Illegal opcode 0x7A: NOP  */
        /* Illegal opcode 0x7B: RRA aaaa,Y */
        /* Illegal opcode 0x7C: NOP aaaa,X */
        case 0x7D: /* ADC aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            if(cpu.PS_D) {
                value_w = (cpu.A & 0x0F) + (value & 0x0F) + (cpu.PS_C ? 1 : 0);
                if(value_w >= 0x0A)
                    value_w = ((value_w + 0x06) & 0x0F) + 0x10;
                value_w += (cpu.A & 0xF0) + (value & 0xF0);

                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.PS_Z = (value_w == 0);

                if(value_w >= 0xA0)
                    value_w += 0x60;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C = (value_w >= 0x100);
            } else {
                value_w = cpu.A + value + (cpu.PS_C ? 1 : 0);
                cpu.PS_N = ((value_w & 0x80) != 0);
                cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_Z = (cpu.A == 0);
                cpu.PS_C = (value_w >= 0x100);
            }

            break;
        case 0x7E: /* ROR aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = ((value & 0x01) != 0);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0x7F: RRA aaaa,X */
        /* Illegal opcode 0x80: NOP #aa */
        case 0x81: /* STA (aa,X) */
            cpu.cycles = 6;
            memory_setIndexedIndirectX(cpu.A);
            break;
        /* Illegal opcode 0x82: NOP #aa */
        /* Illegal opcode 0x83: SAX (aa,X) */
        case 0x84: /* STY aa */
            cpu.cycles = 3;
            memory_setZeroPage(cpu.Y);
            break;
        case 0x85: /* STA aa */
            cpu.cycles = 3;
            memory_setZeroPage(cpu.A);
            break;
        case 0x86: /* STX aa */
            cpu.cycles = 3;
            memory_setZeroPage(cpu.X);
            break;
        /* Illegal opcode 0x87: SAX aa */
        case 0x88: /* DEY  */
            cpu.cycles = 2;
            --cpu.Y;
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        /* Illegal opcode 0x89: NOP #aa */
        case 0x8A: /* TXA  */
            cpu.cycles = 2;
            cpu.A = cpu.X;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0x8B: XAA #aa */
        case 0x8C: /* STY aaaa */
            cpu.cycles = 4;
            memory_setAbsolute(cpu.Y);
            break;
        case 0x8D: /* STA aaaa */
            cpu.cycles = 4;
            memory_setAbsolute(cpu.A);
            break;
        case 0x8E: /* STX aaaa */
            cpu.cycles = 4;
            memory_setAbsolute(cpu.X);
            break;
        /* Illegal opcode 0x8F: SAX aaaa */
        case 0x90: /* BCC aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_C) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x91: /* STA (aa),Y */
            cpu.cycles = 6;
            memory_setIndirectIndexedY(cpu.A);
            break;
        /* Illegal opcode 0x92: KIL  */
        /* Illegal opcode 0x93: AHX (aa),Y */
        case 0x94: /* STY aa,X */
            cpu.cycles = 4;
            memory_setZeroPageIndexedX(cpu.Y);
            break;
        case 0x95: /* STA aa,X */
            cpu.cycles = 4;
            memory_setZeroPageIndexedX(cpu.A);
            break;
        case 0x96: /* STX aa,Y */
            cpu.cycles = 4;
            memory_setZeroPageIndexedY(cpu.X);
            break;
        /* Illegal opcode 0x97: SAX aa,Y */
        case 0x98: /* TYA  */
            cpu.cycles = 2;
            cpu.A = cpu.Y;
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x99: /* STA aaaa,Y */
            cpu.cycles = 5;
            memory_setAbsoluteIndexedY(cpu.A);
            break;
        case 0x9A: /* TXS  */
            cpu.cycles = 2;
            cpu.SP = cpu.X;
            break;
        /* Illegal opcode 0x9B: TAS aaaa,Y */
        /* Illegal opcode 0x9C: SHY aaaa,X */
        case 0x9D: /* STA aaaa,X */
            cpu.cycles = 5;
            memory_setAbsoluteIndexedX(cpu.A);
            break;
        /* Illegal opcode 0x9E: SHX aaaa,Y */
        /* Illegal opcode 0x9F: AHX aaaa,Y */
        case 0xA0: /* LDY #aa */
            cpu.cycles = 2;
            cpu.Y = memory_getImmediate();
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA1: /* LDA (aa,X) */
            cpu.cycles = 6;
            cpu.A = memory_getIndexedIndirectX();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xA2: /* LDX #aa */
            cpu.cycles = 2;
            cpu.X = memory_getImmediate();
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xA3: LAX (aa,X) */
        case 0xA4: /* LDY aa */
            cpu.cycles = 3;
            cpu.Y = memory_getZeroPage();
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA5: /* LDA aa */
            cpu.cycles = 3;
            cpu.A = memory_getZeroPage();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xA6: /* LDX aa */
            cpu.cycles = 3;
            cpu.X = memory_getZeroPage();
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xA7: LAX aa */
        case 0xA8: /* TAY  */
            cpu.cycles = 2;
            cpu.Y = cpu.A;
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA9: /* LDA #aa */
            cpu.cycles = 2;
            cpu.A = memory_getImmediate();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xAA: /* TAX  */
            cpu.cycles = 2;
            cpu.X = cpu.A;
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xAB: LAX #aa */
        case 0xAC: /* LDY aaaa */
            cpu.cycles = 4;
            cpu.Y = memory_getAbsolute();
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xAD: /* LDA aaaa */
            cpu.cycles = 4;
            cpu.A = memory_getAbsolute();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xAE: /* LDX aaaa */
            cpu.cycles = 4;
            cpu.X = memory_getAbsolute();
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xAF: LAX aaaa */
        case 0xB0: /* BCS aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(cpu.PS_C) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xB1: /* LDA (aa),Y */
            cpu.cycles = 5;
            cpu.A = memory_getIndirectIndexedY();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        /* Illegal opcode 0xB2: KIL  */
        /* Illegal opcode 0xB3: LAX (aa),Y */
        case 0xB4: /* LDY aa,X */
            cpu.cycles = 4;
            cpu.Y = memory_getZeroPageIndexedX();
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xB5: /* LDA aa,X */
            cpu.cycles = 4;
            cpu.A = memory_getZeroPageIndexedX();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xB6: /* LDX aa,Y */
            cpu.cycles = 4;
            cpu.X = memory_getZeroPageIndexedY();
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xB7: LAX aa,Y */
        case 0xB8: /* CLV  */
            cpu.cycles = 2;
            cpu.PS_V = false;
            break;
        case 0xB9: /* LDA aaaa,Y */
            cpu.cycles = 4;
            cpu.A = memory_getAbsoluteIndexedY();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xBA: /* TSX  */
            cpu.cycles = 2;
            cpu.X = cpu.SP;
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xBB: LAS aaaa,Y */
        case 0xBC: /* LDY aaaa,X */
            cpu.cycles = 4;
            cpu.Y = memory_getAbsoluteIndexedX();
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xBD: /* LDA aaaa,X */
            cpu.cycles = 4;
            cpu.A = memory_getAbsoluteIndexedX();
            cpu.PS_N = ((cpu.A & 0x80) != 0);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xBE: /* LDX aaaa,Y */
            cpu.cycles = 4;
            cpu.X = memory_getAbsoluteIndexedY();
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xBF: LAX aaaa,Y */
        case 0xC0: /* CPY #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            value = cpu.Y - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC1: /* CMP (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        /* Illegal opcode 0xC2: NOP #aa */
        /* Illegal opcode 0xC3: DCP (aa,X) */
        case 0xC4: /* CPY aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            value = cpu.Y - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC5: /* CMP aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC6: /* DEC aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            --value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xC7: DCP aa */
        case 0xC8: /* INY  */
            cpu.cycles = 2;
            ++cpu.Y;
            cpu.PS_N = ((cpu.Y & 0x80) != 0);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xC9: /* CMP #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCA: /* DEX  */
            cpu.cycles = 2;
            --cpu.X;
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        /* Illegal opcode 0xCB: AXS #aa */
        case 0xCC: /* CPY aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            value = cpu.Y - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCD: /* CMP aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCE: /* DEC aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            --value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xCF: DCP aaaa */
        case 0xD0: /* BNE aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_Z) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xD1: /* CMP (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        /* Illegal opcode 0xD2: KIL  */
        /* Illegal opcode 0xD3: DCP (aa),Y */
        /* Illegal opcode 0xD4: NOP aa,X */
        case 0xD5: /* CMP aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xD6: /* DEC aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            --value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xD7: DCP aa,X */
        case 0xD8: /* CLD  */
            cpu.cycles = 2;
            cpu.PS_D = false;
            break;
        case 0xD9: /* CMP aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        /* Illegal opcode 0xDA: NOP  */
        /* Illegal opcode 0xDB: DCP aaaa,Y */
        /* Illegal opcode 0xDC: NOP aaaa,X */
        case 0xDD: /* CMP aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            value = cpu.A - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xDE: /* DEC aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            --value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xDF: DCP aaaa,X */
        case 0xE0: /* CPX #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            value = cpu.X - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xE1: /* SBC (aa,X) */
            cpu.cycles = 6;
            value = memory_getIndexedIndirectX();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        /* Illegal opcode 0xE2: NOP #aa */
        /* Illegal opcode 0xE3: ISC (aa,X) */
        case 0xE4: /* CPX aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            value = cpu.X - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xE5: /* SBC aa */
            cpu.cycles = 3;
            value = memory_getZeroPage();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        case 0xE6: /* INC aa */
            cpu.cycles = 5;
            value = memory_getZeroPage();
            ++value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xE7: ISC aa */
        case 0xE8: /* INX  */
            cpu.cycles = 2;
            ++cpu.X;
            cpu.PS_N = ((cpu.X & 0x80) != 0);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xE9: /* SBC #aa */
            cpu.cycles = 2;
            value = memory_getImmediate();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        case 0xEA: /* NOP  */
            cpu.cycles = 2;
            /* do nothing */
            break;
        /* Illegal opcode 0xEB: SBC #aa */
        case 0xEC: /* CPX aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            value = cpu.X - value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xED: /* SBC aaaa */
            cpu.cycles = 4;
            value = memory_getAbsolute();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        case 0xEE: /* INC aaaa */
            cpu.cycles = 6;
            value = memory_getAbsolute();
            ++value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xEF: ISC aaaa */
        case 0xF0: /* BEQ aaaa */
            cpu.cycles = 2;
            value_w = memory_getRelativeAddress();
            if(cpu.PS_Z) {
                ++cpu.cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xF1: /* SBC (aa),Y */
            cpu.cycles = 5;
            value = memory_getIndirectIndexedY();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        /* Illegal opcode 0xF2: KIL  */
        /* Illegal opcode 0xF3: ISC (aa),Y */
        /* Illegal opcode 0xF4: NOP aa,X */
        case 0xF5: /* SBC aa,X */
            cpu.cycles = 4;
            value = memory_getZeroPageIndexedX();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        case 0xF6: /* INC aa,X */
            cpu.cycles = 6;
            value = memory_getZeroPageIndexedX();
            ++value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xF7: ISC aa,X */
        case 0xF8: /* SED  */
            cpu.cycles = 2;
            cpu.PS_D = true;
            break;
        case 0xF9: /* SBC aaaa,Y */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedY();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        /* Illegal opcode 0xFA: NOP  */
        /* Illegal opcode 0xFB: ISC aaaa,Y */
        /* Illegal opcode 0xFC: NOP aaaa,X */
        case 0xFD: /* SBC aaaa,X */
            cpu.cycles = 4;
            value = memory_getAbsoluteIndexedX();
            value_w = cpu.A - value - (cpu.PS_C ? 0 : 1);
            cpu.PS_N = ((value_w & 0x80) != 0);
             cpu.PS_V = ((cpu.A ^ value_w) & 0x80) && !((cpu.A ^ value) & 0x80);
            cpu.PS_Z = (value_w == 0);
            cpu.PS_C = (value_w < 0x100);

            if(cpu.PS_D) {
                value_w2 = (cpu.A & 0x0F) - (value & 0x0F) + (cpu.PS_C ? 0 : 1);
                if(value_w2 & 0x8000)
                    value_w = ((value_w - 0x06) & 0x0F) - 0x10;
                value_w2 += (cpu.A & 0xF0) - (value & 0xF0);
                if(value_w2 & 0x8000)
                    value_w -= 0x60;
                cpu.A = (byte)(value_w2 & 0xFF);
            } else
                cpu.A = (byte)(value_w & 0xFF);

            break;
        case 0xFE: /* INC aaaa,X */
            cpu.cycles = 7;
            value = memory_getAbsoluteIndexedX();
            ++value;
            cpu.PS_N = ((value & 0x80) != 0);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        /* Illegal opcode 0xFF: ISC aaaa,X */

        default: /* just in case :) */
            cpu.cycles = 1;
            return RESULT_ILLEGAL_INSTUCTION;
        }

        if(via_count_NMI(cpu.cycles)) {
            memory_stackPushAddress(cpu.PC+1);
            memory_stackPush(cpu_get_state());
            cpu.PS_I = true;
            cpu.PC = memory_get_vector(MEM_NMI);
            cpu.cycles += 7;
            via_count_NMI(7);
            break;
        }
        if(via_count_IRQ(cpu.cycles)) {
            memory_stackPushAddress(cpu.PC+1);
            cpu.PS_B = false;
            memory_stackPush(cpu_get_state());
            cpu.PS_I = true;
            cpu.PC = memory_get_vector(MEM_IRQ_BREAK);
            cpu.cycles += 7;
            via_count_NMI(7);
            via_count_IRQ(7);
        }

        for(value_w = 0; value_w < MAX_COUNTERS; ++value_w) {
            signal_func = counter[value_w].signal;
            if(signal_func) {
                count_tmp = counter[value_w].counter;
                counter[value_w].counter -= cpu.cycles;
                if(counter[value_w].counter > count_tmp)
                    (*signal_func)();
            }
        }

        if(single_step)
            return RESULT_STEP;
    }
    return RESULT_ILLEGAL_INSTUCTION;
}
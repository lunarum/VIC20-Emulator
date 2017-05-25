#include "cpu.h"
#include "memory.h"

bool single_step = false;


void cpu_run() {
    bool tmp_PS_C, s1, s2;
    byte opcode, value;
    word value_w;
    int cycles = 0;

    for(;;) {
        opcode = memory_getImmediate();

        switch(opcode) {
        case 0x0: /* BRK  */
            memory_stackPushAddress(cpu.PC+1);
            cpu_statusPush();
            cpu.PS_B = true;
            cpu.PC = MEM_IRQ_BREAK;
            break;
        case 0x1: /* ORA aa,X */
            value = memory_getZeroPageIndexedX();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x5: /* ORA aa */
            value = memory_getZeroPage();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x6: /* ASL aa */
            value = memory_getZeroPage();
            cpu.PS_C = (bool)(value & 0x80);
            value <<= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x8: /* PHP  */
            cpu_statusPush();
            break;
        case 0x9: /* ORA #aa */
            value = memory_getImmediate();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xA: /* ASL  */
            cpu.PS_C = (bool)(cpu.A & 0x80);
            cpu.A <<= 1;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xD: /* ORA aaaa */
            value = memory_getAbsolute();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xE: /* ASL aaaa */
            value = memory_getAbsolute();
            cpu.PS_C = (bool)(value & 0x80);
            value <<= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x10: /* BPL aaaa */
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_N) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x11: /* ORA aa,Y */
            value = memory_getZeroPageIndexedY();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x15: /* ORA (aa,X) */
            value = memory_getIndexedIndirectX();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x16: /* ASL (aa,X) */
            value = memory_getIndexedIndirectX();
            cpu.PS_C = (bool)(value & 0x80);
            value <<= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x18: /* CLC  */
            cpu.PS_C = false;
            break;
        case 0x19: /* ORA aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x1D: /* ORA aaaa,X */
            value = memory_getAbsoluteIndexedX();
            cpu.A |= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x1E: /* ASL aaaa,X */
            value = memory_getAbsoluteIndexedX();
            cpu.PS_C = (bool)(value & 0x80);
            value <<= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x20: /* JSR aaaa */
            value_w = memory_getAbsolute();
            memory_stackPushAddress(cpu.PC - 1);
            cpu.PC = value_w;
            break;
        case 0x21: /* AND aa,X */
            value = memory_getZeroPageIndexedX();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x24: /* BIT aa */
            value = memory_getZeroPage();
            cpu.PS_N = (bool)(value & 0x80);
            cpu.PS_V = (bool)(value & 0x40);
            cpu.PS_Z = !(bool)(value & cpu.A);
            break;
        case 0x25: /* AND aa */
            value = memory_getZeroPage();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x26: /* ROL aa */
            value = memory_getZeroPage();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x80);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x28: /* PLP  */
            cpu_statusPull();
            break;
        case 0x29: /* AND #aa */
            value = memory_getImmediate();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x2A: /* ROL  */
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(cpu.A & 0x80);
            cpu.A = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x2C: /* BIT aaaa */
            value = memory_getAbsolute();
            cpu.PS_N = (bool)(value & 0x80);
            cpu.PS_V = (bool)(value & 0x40);
            cpu.PS_Z = !(bool)(value & cpu.A);
            break;
        case 0x2D: /* AND aaaa */
            value = memory_getAbsolute();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x2E: /* ROL aaaa */
            value = memory_getAbsolute();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x80);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x30: /* BMI aaaa */
            value_w = memory_getRelativeAddress();
            if(cpu.PS_N) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x31: /* AND aa,Y */
            value = memory_getZeroPageIndexedY();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x35: /* AND (aa,X) */
            value = memory_getIndexedIndirectX();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x36: /* ROL (aa,X) */
            value = memory_getIndexedIndirectX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x80);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x38: /* SEC  */
            cpu.PS_C = true;
            break;
        case 0x39: /* AND aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x3D: /* AND aaaa,X */
            value = memory_getAbsoluteIndexedX();
            cpu.A &= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x3E: /* ROL aaaa,X */
            value = memory_getAbsoluteIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x80);
            value = (value << 1) | (byte)tmp_PS_C;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x40: /* RTI  */
            cpu_statusPull();
            cpu.PC = memory_stackPullAddress();
            break;
        case 0x41: /* EOR aa,X */
            value = memory_getZeroPageIndexedX();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x45: /* EOR aa */
            value = memory_getZeroPage();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x46: /* LSR aa */
            value = memory_getZeroPage();
            cpu.PS_C = (bool)(value & 0x01);
            value >>= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x48: /* PHA  */
            memory_stackPush(cpu.A);
            break;
        case 0x49: /* EOR #aa */
            value = memory_getImmediate();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x4A: /* LSR  */
            cpu.PS_C = (bool)(cpu.A & 0x01);
            cpu.A >>= 1;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x4C: /* JMP aaaa */
            cpu.PC = memory_getAbsolute();
            break;
        case 0x4D: /* EOR aaaa */
            value = memory_getAbsolute();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x4E: /* LSR aaaa */
            value = memory_getAbsolute();
            cpu.PS_C = (bool)(value & 0x01);
            value >>= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x50: /* BVC aaaa */
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_V) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x51: /* EOR aa,Y */
            value = memory_getZeroPageIndexedY();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x55: /* EOR (aa,X) */
            value = memory_getIndexedIndirectX();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x56: /* LSR (aa,X) */
            value = memory_getIndexedIndirectX();
            cpu.PS_C = (bool)(value & 0x01);
            value >>= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x58: /* CLI  */
            cpu.PS_I = false;
            break;
        case 0x59: /* EOR aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x5D: /* EOR aaaa,X */
            value = memory_getAbsoluteIndexedX();
            cpu.A ^= value;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x5E: /* LSR aaaa,X */
            value = memory_getAbsoluteIndexedX();
            cpu.PS_C = (bool)(value & 0x01);
            value >>= 1;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x60: /* RTS  */
            cpu.PC = memory_stackPullAddress() + 1;
            break;
        case 0x61: /* ADC aa,X */
            value = memory_getZeroPageIndexedX();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x65: /* ADC aa */
            value = memory_getZeroPage();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x66: /* ROR aa */
            value = memory_getZeroPage();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x01);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x68: /* PLA  */
            cpu.A = memory_stackPull();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x69: /* ADC #aa */
            value = memory_getImmediate();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x6A: /* ROR  */
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(cpu.A & 0x01);
            cpu.A = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x6C: /* JMP (aaaa) */
            cpu.PC = memory_getIndirectAbsoluteAddress();
            break;
        case 0x6D: /* ADC aaaa */
            value = memory_getAbsolute();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x6E: /* ROR aaaa */
            value = memory_getAbsolute();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x01);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x70: /* BVS aaaa */
            value_w = memory_getRelativeAddress();
            if(cpu.PS_V) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x71: /* ADC aa,Y */
            value = memory_getZeroPageIndexedY();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x75: /* ADC (aa,X) */
            value = memory_getIndexedIndirectX();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x76: /* ROR (aa,X) */
            value = memory_getIndexedIndirectX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x01);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x78: /* SEI  */
            cpu.PS_I = true;
            break;
        case 0x79: /* ADC aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x7D: /* ADC aaaa,X */
            value = memory_getAbsoluteIndexedX();
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0x7E: /* ROR aaaa,X */
            value = memory_getAbsoluteIndexedX();
            tmp_PS_C = cpu.PS_C;
            cpu.PS_C = (bool)(value & 0x01);
            value = (value >> 1) | (((byte)tmp_PS_C) << 7);
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0x81: /* STA aa,X */
            memory_setZeroPageIndexedX(cpu.A);
            break;
        case 0x84: /* STY aa */
            memory_setZeroPage(cpu.Y);
            break;
        case 0x85: /* STA aa */
            memory_setZeroPage(cpu.A);
            break;
        case 0x86: /* STX aa */
            memory_setZeroPage(cpu.X);
            break;
        case 0x88: /* DEY  */
            --cpu.Y;
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0x8A: /* TXA  */
            cpu.A = cpu.X;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x8C: /* STY aaaa */
            memory_setAbsolute(cpu.Y);
            break;
        case 0x8D: /* STA aaaa */
            memory_setAbsolute(cpu.A);
            break;
        case 0x8E: /* STX aaaa */
            memory_setAbsolute(cpu.X);
            break;
        case 0x90: /* BCC aaaa */
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_C) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0x91: /* STA aa,Y */
            memory_setZeroPageIndexedY(cpu.A);
            break;
        case 0x94: /* STY (aa,X) */
            memory_setIndexedIndirectX(cpu.Y);
            break;
        case 0x95: /* STA (aa,X) */
            memory_setIndexedIndirectX(cpu.A);
            break;
        case 0x96: /* STX (aa),Y */
            memory_setIndirectIndexedY(cpu.X);
            break;
        case 0x98: /* TYA  */
            cpu.A = cpu.Y;
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0x99: /* STA aaaa,Y */
            memory_setAbsoluteIndexedY(cpu.A);
            break;
        case 0x9A: /* TXS  */
            cpu.SP = cpu.X;
            break;
        case 0x9D: /* STA aaaa,X */
            memory_setAbsoluteIndexedX(cpu.A);
            break;
        case 0xA0: /* LDY #aa */
            cpu.Y = memory_getImmediate();
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA1: /* LDA aa,X */
            cpu.A = memory_getZeroPageIndexedX();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xA2: /* LDX #aa */
            cpu.X = memory_getImmediate();
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xA4: /* LDY aa */
            cpu.Y = memory_getZeroPage();
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA5: /* LDA aa */
            cpu.A = memory_getZeroPage();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xA6: /* LDX aa */
            cpu.X = memory_getZeroPage();
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xA8: /* TAY  */
            cpu.Y = cpu.A;
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xA9: /* LDA #aa */
            cpu.A = memory_getImmediate();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xAA: /* TAX  */
            cpu.X = cpu.A;
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xAC: /* LDY aaaa */
            cpu.Y = memory_getAbsolute();
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xAD: /* LDA aaaa */
            cpu.A = memory_getAbsolute();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xAE: /* LDX aaaa */
            cpu.X = memory_getAbsolute();
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xB0: /* BCS aaaa */
            value_w = memory_getRelativeAddress();
            if(cpu.PS_C) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xB1: /* LDA aa,Y */
            cpu.A = memory_getZeroPageIndexedY();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xB4: /* LDY (aa,X) */
            cpu.Y = memory_getIndexedIndirectX();
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xB5: /* LDA (aa,X) */
            cpu.A = memory_getIndexedIndirectX();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xB6: /* LDX (aa),Y */
            cpu.X = memory_getIndirectIndexedY();
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xB8: /* CLV  */
            cpu.PS_V = false;
            break;
        case 0xB9: /* LDA aaaa,Y */
            cpu.A = memory_getAbsoluteIndexedY();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xBA: /* TSX  */
            cpu.X = cpu.SP;
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xBC: /* LDY aaaa,X */
            cpu.Y = memory_getAbsoluteIndexedX();
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xBD: /* LDA aaaa,X */
            cpu.A = memory_getAbsoluteIndexedX();
            cpu.PS_N =  (bool)(cpu.A & 0x80);
            cpu.PS_Z = (cpu.A == 0);
            break;
        case 0xBE: /* LDX aaaa,Y */
            cpu.X = memory_getAbsoluteIndexedY();
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xC0: /* CPY #aa */
            value = memory_getImmediate();
            value = cpu.Y - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC1: /* CMP aa,X */
            value = memory_getZeroPageIndexedX();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC4: /* CPY aa */
            value = memory_getZeroPage();
            value = cpu.Y - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC5: /* CMP aa */
            value = memory_getZeroPage();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xC6: /* DEC aa */
            value = memory_getZeroPage();
            --value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xC8: /* INY  */
            ++cpu.Y;
            cpu.PS_N =  (bool)(cpu.Y & 0x80);
            cpu.PS_Z = (cpu.Y == 0);
            break;
        case 0xC9: /* CMP #aa */
            value = memory_getImmediate();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCA: /* DEX  */
            --cpu.X;
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xCC: /* CPY aaaa */
            value = memory_getAbsolute();
            value = cpu.Y - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCD: /* CMP aaaa */
            value = memory_getAbsolute();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xCE: /* DEC aaaa */
            value = memory_getAbsolute();
            --value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xD0: /* BNE aaaa */
            value_w = memory_getRelativeAddress();
            if(!cpu.PS_Z) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xD1: /* CMP aa,Y */
            value = memory_getZeroPageIndexedY();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xD5: /* CMP (aa,X) */
            value = memory_getIndexedIndirectX();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xD6: /* DEC (aa,X) */
            value = memory_getIndexedIndirectX();
            --value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xD8: /* CLD  */
            cpu.PS_D = false;
            break;
        case 0xD9: /* CMP aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xDD: /* CMP aaaa,X */
            value = memory_getAbsoluteIndexedX();
            value = cpu.A - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xDE: /* DEC aaaa,X */
            value = memory_getAbsoluteIndexedX();
            --value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xE0: /* CPX #aa */
            value = memory_getImmediate();
            value = cpu.X - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xE1: /* SBC aa,X */
            value = memory_getZeroPageIndexedX();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xE4: /* CPX aa */
            value = memory_getZeroPage();
            value = cpu.X - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xE5: /* SBC aa */
            value = memory_getZeroPage();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xE6: /* INC aa */
            value = memory_getZeroPage();
            ++value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xE8: /* INX  */
            ++cpu.X;
            cpu.PS_N =  (bool)(cpu.X & 0x80);
            cpu.PS_Z = (cpu.X == 0);
            break;
        case 0xE9: /* SBC #aa */
            value = memory_getImmediate();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xEA: /* NOP  */
            /* do nothing */
            break;
        case 0xEC: /* CPX aaaa */
            value = memory_getAbsolute();
            value = cpu.X - value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            cpu.PS_C = !cpu.PS_N;
            break;
        case 0xED: /* SBC aaaa */
            value = memory_getAbsolute();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xEE: /* INC aaaa */
            value = memory_getAbsolute();
            ++value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xF0: /* BEQ aaaa */
            value_w = memory_getRelativeAddress();
            if(cpu.PS_Z) {
                ++cycles;
                cpu.PC = value_w;
            }
            break;
        case 0xF1: /* SBC aa,Y */
            value = memory_getZeroPageIndexedY();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xF5: /* SBC (aa,X) */
            value = memory_getIndexedIndirectX();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xF6: /* INC (aa,X) */
            value = memory_getIndexedIndirectX();
            ++value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        case 0xF8: /* SED  */
            cpu.PS_D = true;
            break;
        case 0xF9: /* SBC aaaa,Y */
            value = memory_getAbsoluteIndexedY();
            value = ~value;
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xFD: /* SBC aaaa,X */
            value = ~memory_getAbsoluteIndexedX();
            /* SBC is ADC with inverted value */
            if(cpu.PS_D) {
            } else {
                s1 =  (bool)(cpu.A & 0x80);
                s2 =  (bool)(value & 0x80);
                value_w = (word)cpu.A + (word)value + (word)cpu.PS_C;
                cpu.A = (byte)(value_w & 0xFF);
                cpu.PS_C =  (bool)(cpu.A & 0xFF00);
                cpu.PS_Z = (bool)value_w;
                cpu.PS_N =  (bool)(cpu.A & 0x80);
                cpu.PS_V =  (s1 && s2 && !cpu.PS_N) || (!s1 && !s2 && cpu.PS_N);
            }
            break;
        case 0xFE: /* INC aaaa,X */
            value = memory_getAbsoluteIndexedX();
            ++value;
            cpu.PS_N =  (bool)(value & 0x80);
            cpu.PS_Z = (value == 0);
            memory_setLast(value);
            break;
        }
        cycle_counter -= cycles;
        if(cycle_counter < 0) {
            /* TODO: interrupt logic */
            cycle_counter += cycle_reset;
        }
        if(single_step || opcode == 0)
            break;
    }
}
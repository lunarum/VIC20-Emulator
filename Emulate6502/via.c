#include "via.h"
#include "cpu.h"
#include <stdio.h>

/*
 *    6522 Versatile Interface Adapter Description
 * 
 * from: http://vic-20.appspot.com/docs/vicvia.txt
 * 
 *  ADDR  DESCRIPTION                 REGISTER
 *   0     Port B                      AAAAAAAA
 *   1     Port A (with handshaking)   BBBBBBBB
 *   2     Data Direction B            CCCCCCCC
 *   3     Data Direction A            DDDDDDDD
 *   4     Timer #1, low byte          EEEEEEEE
 *   5     Timer #1, high byte         FFFFFFFF
 *   6     Timer #1, low byte to load  GGGGGGGG
 *   7     Timer #1, high byte to load HHHHHHHH
 *   8     Timer #2, low byte          IIIIIIII
 *   9     Timer #2, high byte         JJJJJJJJ
 *   A     Shift Register              KKKKKKKK
 *   B     Auxiliary Control           LLMNNNOP
 *   C     Peripheral Control          QQQRSSST
 *   D     Interrupt Flags             UVWXYZab
 *   E     Interrupt Enable            cdefghij
 *   F     Port A (no handshaking)     kkkkkkkk
*/

#define IRQ_FLAG_SET    0x80
#define IRQ_FLAG_T1C    0x40
#define IRQ_FLAG_T2C    0x20
#define IRQ_FLAG_CB1    0x10
#define IRQ_FLAG_CB2    0x08
#define IRQ_FLAG_SR     0x04
#define IRQ_FLAG_CA1    0x02
#define IRQ_FLAG_CA2    0x01

#define ACR_T1_PB7_OUT      0x80
#define ACR_T1_FREE_RUN     0x40
#define ACR_T2_PB6_COUNT    0x20

struct via_registers via1, via2;
static char const* HEXCHARS = "0123456789abcdef";

byte via_read_register(word address) {
    struct via_registers *via;
    byte register_offset = address & 0x000F;    // last byte defines register used
    byte value = 0;
    address &= 0xFFF0;    // strip register bits (byte) from address

//putchar('R');
    if(address == 0x9110) {
        via = &via1;
//putchar('1');
    }
    else if(address == 0x9120) {
        via = &via2;
//putchar('2');
    }
    else {
//putchar('?');
        return 0;
    }
//putchar(HEXCHARS[register_offset]);

    switch(register_offset) {
        case 0x04:
            value = (byte)(via->T1C & 0xFF);
            // clear Timer-1 IRQ flag
            via->IFR &= ~IRQ_FLAG_T1C;
            // Clear interrupt signal if Timer1 IRQ flag was last interrupt source
            if(!(via->IFR & ~IRQ_FLAG_SET))
                via->IFR = 0;
            break;
        case 0x05:
            value = (byte)(via->T1C >> 8);
            break;
        case 0x06:
            value = (byte)(via->T1L & 0xFF);
            break;
        case 0x07:
            value = (byte)(via->T1L >> 8);
            break;
        case 0x08:
            value = (byte)(via->T2C & 0xFF);
            // clear Timer-2 IRQ flag
            via->IFR &= ~IRQ_FLAG_T2C;
            // Clear interrupt signal if Timer2 IRQ flag was last interrupt source
            if(!(via->IFR & ~IRQ_FLAG_SET))
                via->IFR = 0;
            break;
        case 0x09:
            value = (byte)(via->T2C >> 8);
            break;
        case 0x0A:
            value = via->SR;
            break;
        case 0x0B:
            value = via->ACR;
            break;
        case 0x0C:
            value = via->PCR;
            break;
        case 0x0D:
            value = via->IFR;
            break;
        case 0x0E:
            // bit 7 IER is always read as a 1
            value = 0x80 | via->IER;
            break;
    }
        
//putchar(HEXCHARS[value>>4]);
//putchar(HEXCHARS[value&15]);
    return value;
}

void via_write_register(word address, byte value) {
    struct via_registers *via;
    byte register_offset = address & 0x000F;    // last byte defines register used
    address &= 0xFFF0;    // strip register bits (byte) from address

//putchar('W');
    if(address == 0x9110) {
        via = &via1;
//putchar('1');
    }
    else if(address == 0x9120) {
        via = &via2;
//putchar('2');
    }
    else {
//putchar('?');
        return;
    }
//putchar(HEXCHARS[register_offset]);
//putchar(HEXCHARS[value>>4]);
//putchar(HEXCHARS[value&15]);

    switch(register_offset) {
        case 0x04:
        case 0x06:
            // Write Timer-1 Latch low-byte
            via->T1L &= 0xFF00;
            via->T1L |= value;
            break;
        case 0x05:
            // Write Timer-1 Latch High-byte
            via->T1L &= 0xFF;
            via->T1L |= (value << 8);
            // Move Latch to Timer (which starts Timer)
            via->T1C = via->T1L;
//printf(" T1 start %04x %c\n", via->T1C, (via->ACR & ACR_T1_FREE_RUN) ? 'F' : 'S');
            // clear Timer-1 IRQ flag
            via->IFR &= ~IRQ_FLAG_T1C;
            // Clear interrupt signal if Timer1 IRQ flag was last interrupt source
            if(!(via->IFR & ~IRQ_FLAG_SET))
                via->IFR = 0;
            break;
        case 0x07:
            // Write Timer-1 Latch High-byte
            via->T1L &= 0xFF;
            via->T1L |= (value << 8);
            // clear Timer-1 IRQ flag
            via->IFR &= ~IRQ_FLAG_T1C;
            // Clear interrupt signal if Timer1 IRQ flag was last interrupt source
            if(!(via->IFR & ~IRQ_FLAG_SET))
                via->IFR = 0;
            break;
        case 0x08:
            // Write Timer-2 Latch low-byte
            via->T2L = value;
            break;
        case 0x09:
            // Write Timer-2 Counter (which starts Timer)
            via->T2C = via->T2L | (value << 8);
//printf(" T2 start %04x\n", via->T2C);
            // Clear Timer-2 IRQ flag
            via->IFR &= ~IRQ_FLAG_T2C;
            // Clear interrupt signal if Timer2 IRQ flag was last interrupt source
            if(!(via->IFR & ~IRQ_FLAG_SET))
                via->IFR = 0;
            break;
        case 0x0A:
            via->SR = value;
            break;
        case 0x0B:
            via->ACR = value;
            break;
        case 0x0C:
            via->PCR = value;
            break;
       case 0x0D:
            // clear all 7 lower IFR bits marked with a 1 (ignore/leave bit 7 alone)
            via->IFR &= 0x80 | ~value;
            break;
       case 0x0E:
            if(value & IRQ_FLAG_SET) {
                // set given interrupts enabled
                via->IER |= value;
            }
            else {
                // set given interrupts disabled
                via->IER &= (~value & 0x7F);
                // Reset bit7 IFR if all interrupts are disabled (bits 0-7 of IER are 0)
                if(!(via->IER & 0x7F))
                    via->IFR &= 0x7F;
            }
            break;
    }
}

bool via_count_NMI(byte cycles) {
    bool raise_interrupt = false;

    if(via1.T1C > 0) {
        if(via1.T1C > cycles)
            via1.T1C -= cycles;
        else {
            if(via1.ACR & ACR_T1_FREE_RUN) {
                // free running; reload counter from latch
                cycles -= via1.T1C;
                via1.T1C = via1.T1L - cycles;
            } else {
                // One shot mode; clear timer to stop counting
                via1.T1C = 0;
            }
            // Set interrupt
            via1.IFR |= IRQ_FLAG_T1C;
            // Raise interrupt if enabled
            if(via1.IER & IRQ_FLAG_T1C) {
                via1.IFR |= IRQ_FLAG_SET;
                raise_interrupt = true;
putchar('#');
putchar('1');
            }
        }
    }

    if(via1.T2C > 0) {
        if(via1.ACR & ACR_T2_PB6_COUNT) {
            // not implemented
        } else {
            if(via1.T2C > cycles)
                via1.T2C -= cycles;
            else {
                // One shot mode; clear timer to stop counting
                via1.T2C = 0;
                // Set interrupt
                via1.IFR |= IRQ_FLAG_T2C;
                // Raise interrupt if enabled
                if(via1.IER & IRQ_FLAG_T2C) {
                    via1.IFR |= IRQ_FLAG_SET;
                    raise_interrupt = true;
putchar('#');
putchar('2');
                }
            }
        }
    }

    // edge_level triggered NMI
    return raise_interrupt;
}
    
bool via_count_IRQ(byte cycles) {
    bool raise_interrupt = false;

    if(via2.T1C > 0) {
        if(via2.T1C > cycles)
            via2.T1C -= cycles;
        else {
            if(via2.ACR & ACR_T1_FREE_RUN) {
                // free running; reload counter from latch
//printf("\n*T1 free %04x %02x %04x\n", via2.T1C, (unsigned)cycles, via2.T1L);
                cycles -= via2.T1C;
                via2.T1C = via2.T1L - cycles;
            } else {
                // One shot mode; clear timer to stop counting
                via2.T1C = 0;
//printf("\n*T1 shot\n");
            }
            // Set interrupt
            via2.IFR |= IRQ_FLAG_T1C;
            // Raise interrupt if enabled
            if(via2.IER & IRQ_FLAG_T1C) {
                via2.IFR |= IRQ_FLAG_SET;
                raise_interrupt = true;
putchar('#');
            }
        }
    }

    if(via2.T2C > 0) {
        if(via2.ACR & ACR_T2_PB6_COUNT) {
            // not implemented
        } else {
            if(via2.T2C > cycles)
                via2.T2C -= cycles;
            else {
                // One shot mode; clear timer to stop counting
                via2.T2C = 0;
                // Set interrupt
                via2.IFR |= IRQ_FLAG_T2C;
                // Raise interrupt if enabled
                if(via2.IER & IRQ_FLAG_T2C) {
                    via2.IFR |= IRQ_FLAG_SET;
                    raise_interrupt = true;
putchar('*');
                }
            }
        }
    }

    // edge_level triggered IRQ
    return raise_interrupt;
}

void via_reset() {
    via1.T1C = 0;
    via1.T2C = 0;
    via1.IFR = 0;
    via1.IER = 0;

    via2.T1C = 0;
    via2.T2C = 0;
    via2.IFR = 0;
    via2.IER = 0;
}

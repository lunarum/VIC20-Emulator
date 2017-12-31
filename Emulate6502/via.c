#include "via.h"
#include <stdio.h>

/*
 *    6522 Versatile Interface Adapter Description
 * 
 * from: http://vic-20.appspot.com/docs/vicvia.txt
 * 
 *  ADDRESS   DESCRIPTION                 REGISTER
 *   9110     Port B                      AAAAAAAA
 *   9111     Port A (with handshaking)   BBBBBBBB
 *   9112     Data Direction B            CCCCCCCC
 *   9113     Data Direction A            DDDDDDDD
 *   9114     Timer #1, low byte          EEEEEEEE
 *   9115     Timer #1, high byte         FFFFFFFF
 *   9116     Timer #1, low byte to load  GGGGGGGG
 *   9117     Timer #1, high byte to load HHHHHHHH
 *   9118     Timer #2, low byte          IIIIIIII
 *   9119     Timer #2, high byte         JJJJJJJJ
 *   911A     Shift Register              KKKKKKKK
 *   911B     Auxiliary Control           LLMNNNOP
 *   911C     Peripheral Control          QQQRSSST
 *   911D     Interrupt Flags             UVWXYZab
 *   911E     Interrupt Enable            cdefghij
 *   911F     Port A (no handshaking)     kkkkkkkk
*/

static byte *memory;
static word dirty_registers = 0;
static byte IFR = 0;    // Interrupt Flag Register
static byte IER = 0;    // Interrupt Enable Register

#define IRQ_FLAG_SET    0x80
#define IRQ_FLAG_TIMER1 0x40
#define IRQ_FLAG_TIMER2 0x20
#define IRQ_FLAG_CB1    0x10
#define IRQ_FLAG_CB2    0x08
#define IRQ_FLAG_SR     0x04
#define IRQ_FLAG_CA1    0x02
#define IRQ_FLAG_CA2    0x01


byte via_read_register(word address) {
    byte value = 0;
    word flag = (1 << (address & 0xF));

    putchar('R');
    putchar('a'+(address & 0xF));
    if(dirty_registers & flag) {
        switch(address) {
            case 0x911D:
                value = IFR;
                break;
            case 0x911E:
                value = IER;
                break;
        }
        memory[address] = value;
        dirty_registers ^= flag;
    }
    else
        value = memory[address];
        
    return value;
}

void via_write_register(word address, byte value) {
    putchar('W');
    putchar('a'+(address & 0xF));
    switch(address) {
        case 0x911E:
            if(value & IRQ_FLAG_SET) {
                // set given interrupts enabled
                IER |= value;
            }
            else {
                // set given interrupts disabled
                IER &= (~value & 0x7F);
            }
            dirty_registers |= 0x40;
            break;
    }
}

void via_reset() {
    memory = memory_get_ptr(0);
    dirty_registers = 0;
    IFR = 0;
    IER = 0;
}

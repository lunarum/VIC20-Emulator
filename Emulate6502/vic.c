#include "vic.h"
//#include <stdio.h>

#define CLR_BLACK   0x000000
#define CLR_WHITE   0xffffff
#define CLR_RED     0x782922
#define CLR_CYAN    0x87d6dd
#define CLR_PURPLE  0xa95fb6
#define CLR_GREEN   0x56a049
#define CLR_BLUE    0x40318e
#define CLR_YELLOW  0xbfce73
#define CLR_ORANGE          0xaa7448
#define CLR_LIGHT_ORANGE    0xeab488
#define CLR_PINK            0xb86962
#define CLR_LIGHT_CYAN      0xc7ffff
#define CLR_LIGHT_PURPLE    0xe99ff6
#define CLR_LIGHT_GREEN     0x94e088
#define CLR_LIGHT_BLUE      0x8071cc
#define CLR_LIGHT_YELLOW    0xfeffb3

unsigned color_palette[] = {
    CLR_BLACK,
    CLR_WHITE,
    CLR_RED,
    CLR_CYAN,
    CLR_PURPLE,
    CLR_GREEN,
    CLR_BLUE,
    CLR_YELLOW,
    CLR_ORANGE,
    CLR_LIGHT_ORANGE,
    CLR_PINK,
    CLR_LIGHT_CYAN,
    CLR_LIGHT_PURPLE,
    CLR_LIGHT_GREEN,
    CLR_LIGHT_BLUE,
    CLR_LIGHT_YELLOW
};

unsigned frame_counter = 0;

/*
 * Location       Bits   Default value	Function
 *  hex   dec
 * $9000 36864	ABBBBBBB $0C Interlace mode / Screen origin (horizontal)
 * $9001 36865	CCCCCCCC $26 Screen origin (vertical)
 * $9002 36866	HDDDDDDD $16 Screen memory location / Number of vic.columns
 * $9003 36867	GEEEEEEF $2E Raster value (lowest bit)/ Number of vic.rows / Double character size
 * $9004 36868	GGGGGGGG  -- Raster value
 * $9005 36869	HHHHIIII $F0 Screen memory location / Character memory location
 * $9006 36870	JJJJJJJJ $00 Light pen position (horizontal)
 * $9007 36871	KKKKKKKK $00 Light pen position (vertical)
 * $9008 36872	LLLLLLLL $FF Paddle 1
 * $9009 36873	MMMMMMMM $FF Paddle 2
 * $900A 36874	NRRRRRRR $00 Bass sound switch & frequency
 * $900B 36875	OSSSSSSS $00 Alto sound switch & frequency
 * $900C 36876	PTTTTTTT $00 Soprano sound switch & frequency
 * $900D 36877	QUUUUUUU $00 Noise switch & frequency
 * $900E 36878	WWWWVVVV $00 Auxiliary color / Composite sound volume
 * $900F 36879	XXXXYZZZ $1B Screen color / Reverse mode / Border color
 * 
 * 9005     36869   bits 0-3 start of character memory
                           (default = 0)
                           bits 4-7 is rest of video address
                           (default= F)
                           BITS 3,2,1,0 CM startinq address
                                        HEX   DEC
                           0000   ROM   8000  32768
                           0001         8400  33792
                           0010         8800  34816
                           0011         8C00  35840
                           1000   RAM   0000  0000
                           1001 xxxx
                           1010 xxxx  unavail.
                           1011 xxxx
                           1100         1000  4096
                           1101         1400  5120
                           1110         1800  6144
                           1111         1C00  7168

 *                    Usable RAM     Chars    Screen Shape
 * 36869 Start 52,56 Graphic BASIC  8×8 16×8 Square Maximum	
 *  252   4096   16    3.5K   none  256  224  21×21  16×28
 *  253   5120   20    2.5K    1K   256  160  17×18  16×20
 *  254	  6144   24    1.5K    2K   192   96  13×14  12×16
 *  255	  7168   28    0.5K    3K    64   32   8×8    8×8
 */

static byte *memory;
static word dirty_registers = 0;
struct vic_registers vic;

 
byte vic_read_register(word address) {
    byte value = 0;
    word flag = (1 << (address & 0xF));

//    if(address == 0x9003 || address == 0x9004)
//        printf("Read $%04X", address);
    if(dirty_registers & flag) {
//        putchar('*');
        // Only vic.scan_line registers can change and the rest is saved with vic_write_register in memory
        // Therefore implementation of only the scan-line registers is necessary
        switch(address) {
            case 0x9003:
                value = (value & 0x7F) | ((vic.scan_line & 1) << 7);
                break;
            case 0x9004:
                value = (vic.scan_line >> 1);
                break;
        }
        memory[address] = value;
        dirty_registers &= ~flag;
    }
    else
        value = memory[address];
//    if(address == 0x9003 || address == 0x9004)
//        printf(" = $%02X\n", (unsigned)value);

    return value;
}

void vic_write_register(word address, byte value) {
    word temp_w;

    switch(address) {
        case 0x9000:
            vic.offset_x          = value & 0x7F;
//            interlace_mode    = (value & 0x80) ? true : false;
            break;
        case 0x9001:
            vic.offset_y          = value;
            break;
        case 0x9002:
            temp_w = ((value & 0x80) << 2);
            vic.screen_address    = (vic.screen_address & 0xFDFF) | temp_w;
            vic.color_address     = 0x9400 | temp_w;
            vic.columns           = value & 0x7F;
            break;
        case 0x9003:
            vic.wide_characters   = (value & 1) ? true : false;
            vic.rows              = (value & 0x7F) >> 1;
//            vic.scan_line        = (value & 0x80) >> 7;
            break;
        case 0x9004:
//            vic.scan_line        = (vic.scan_line | value << 1);
            break;
        case 0x9005:
            vic.character_address = (value & 0x07) << 10;
            if(!(value & 0x08))
                vic.character_address |= 0x8000;
            vic.screen_address = (vic.screen_address & 0x0200) | (value & 0x70) << 6;
            if(!(value & 0x80))
                vic.screen_address |= 0x8000;
            break;
        case 0x9006:
            break;
        case 0x9007:
            break;
        case 0x9008:
            break;
        case 0x9009:
            break;
        case 0x900A:
            break;
        case 0x900B:
            break;
        case 0x900C:
            break;
        case 0x900D:
            break;
        case 0x900E:
            vic.auxilary_color    = color_palette[value >> 4];
            break;
        case 0x900F:
            vic.screen_color      = color_palette[value >> 4];
            vic.border_color      = color_palette[value & 0b111];
            vic.inverted_mode     = (value & 0b1000) ? false : true; // normal mode (1) or inverted mode (0)!!!
            break;
    }
    // save for read
    memory[address] = value;

//    printf("%04X := %02X [char %04X, screen %04X, color %04X]\n", (unsigned)address, (unsigned)value,
//        (unsigned)vic.character_address, (unsigned)vic.screen_address, (unsigned)vic.color_address);
}

void vic_plot_scan_line() {
    unsigned min_y = vic.offset_y << 1;
    unsigned min_x = vic.offset_x << 1;
    unsigned max_y = min_y + (vic.wide_characters ? (vic.rows<<5) : (vic.rows<<4));
    unsigned max_x = min_x + (vic.columns<<5);
    unsigned line = vic.scan_line << 1;
    
//    if(!vic.scan_line)
//        printf("chars=$%04X screen=$%04X color=$%04X ox=%03u oy=%03u rc=%02ux%02u mmx=%03u-%03u mmy=%03u-%03u bc=%02u sc=%02u ac=%02u\n",
//            (unsigned)vic.character_address,
//            (unsigned)vic.screen_address,
//            (unsigned)vic.color_address,
//            vic.offset_x, vic.offset_y, vic.rows, vic.columns,
//            min_x, max_x, min_y, max_y,
//            (unsigned)(memory[0x900F] & 0b111), (unsigned)(memory[0x900F] >> 4), (unsigned)(memory[0x900E] >> 4));
//    printf("plot(sl=%03u)", vic.scan_line);

    if(line < min_y || line >= max_y) {
        if(line < SCREEN_HEIGHT) {
//            printf("-B\n");
            // border
            draw_line(0, line, SCREEN_WIDTH, line, vic.border_color);
            ++line;
            draw_line(0, line, SCREEN_WIDTH, line, vic.border_color);
        }
//        else
//            printf("-O\n");
    } else {
        // screen
//        printf("-S ");
        
        // Left border
        draw_line(0, line, min_x, line, vic.border_color);        
        draw_line(0, line+1, min_x, line+1, vic.border_color);        
        
        unsigned x = min_x;
        unsigned row = vic.wide_characters ? ((line - min_y) >> 5) : ((line - min_y) >> 4);
//        printf("%02u [", row);
        unsigned line_index = vic.wide_characters ? ((vic.scan_line - vic.offset_y) & 0x0F) : ((vic.scan_line - vic.offset_y) & 0x07);
        for(unsigned column = 0; column < vic.columns; ++column) {            
            unsigned char_index  = row*vic.columns + column;
            unsigned color       = memory[vic.color_address + char_index];
            bool     multi_color = (color & 0x08);
            unsigned character   = memory[vic.screen_address + char_index];
            unsigned pixel_index = line_index + (vic.wide_characters ? (character << 4) : (character << 3));
            unsigned pixels      = memory[vic.character_address + pixel_index];

//            printf("%02X%c%u-%02X|", (unsigned)character, multi_color ? 'M' : 'H', color, pixels);
            color = color_palette[color & 0x07];

            if(multi_color) {
                unsigned multi_color_palette[] = { vic.screen_color, vic.border_color, color, vic.auxilary_color };
                x += 32;
                unsigned x2 = x - 1;
                unsigned x1 = x2 - 7;
                color = pixels & 3;
                draw_line(x1, line,   x2, line,   multi_color_palette[color]);
                draw_line(x1, line+1, x2, line+1, multi_color_palette[color]);
                x2 -= 8;
                x1 -= 8;
                pixels >>= 2;
                color = pixels & 3;
                draw_line(x1, line,   x2, line,   multi_color_palette[color]);
                draw_line(x1, line+1, x2, line+1, multi_color_palette[color]);
                x2 -= 8;
                x1 -= 8;
                pixels >>= 2;
                color = pixels & 3;
                draw_line(x1, line,   x2, line,   multi_color_palette[color]);
                draw_line(x1, line+1, x2, line+1, multi_color_palette[color]);
                x2 -= 8;
                x1 -= 8;
                pixels >>= 2;
                color = pixels & 3;
                draw_line(x1, line,   x2, line,   multi_color_palette[color]);
                draw_line(x1, line+1, x2, line+1, multi_color_palette[color]);
            } else {
                unsigned back_color = vic.screen_color;
                if(vic.inverted_mode) {
                    back_color = color;
                    color = vic.screen_color;
                }
                unsigned x2 = x + 31;

                // background
                draw_line(x, line,   x2, line,   back_color);
                draw_line(x, line+1, x2, line+1, back_color);

                x2 = x + 3;
                if(pixels & 0x80) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x40) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x20) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x10) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x08) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x04) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x02) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
                x2 += 4;
                if(pixels & 0x01) {
                    draw_line(x, line,   x2, line,   color);
                    draw_line(x, line+1, x2, line+1, color);
                }
                x  += 4;
            }
        }

        // right border
        draw_line(max_x, line, SCREEN_WIDTH, line, vic.border_color);        
        draw_line(max_x, line+1, SCREEN_WIDTH, line+1, vic.border_color);
//        printf("]\n");
    }

    if(++vic.scan_line >= FRAME_LINES) {
        frame_counter++;
        draw_screen();
        vic.scan_line = 0;
    }
    dirty_registers |= 0x18;    // set registers (bits) 3 and 4 as dirty

    counter[COUNTER_VIC].counter = VIC_COUNTER_CYCLES;
}

void vic_reset() {
    memory = memory_get_ptr(0);
    dirty_registers = 0;
    vic.offset_x = 0;
//  interlace_mode;
    vic.offset_y = 0;
    vic.screen_address = 0;
    vic.color_address = 0;
    vic.character_address = 0;
    vic.columns = 0;
    vic.wide_characters = 0;
    vic.rows = 0;
    vic.scan_line = 0;
    vic.auxilary_color = 0;
    vic.screen_color = 0;
    vic.border_color = 0;
    vic.inverted_mode = 0;
    counter[COUNTER_VIC].counter = VIC_COUNTER_CYCLES;
    counter[COUNTER_VIC].signal = vic_plot_scan_line;
//    memory[0x9000] = 12; // NTSC: 5
//    memory[0x9001] = 38; // NTSC: 25
//    memory[0x9002] = 150;
//    memory[0x9003] = 46;
//    memory[0x9004] = 0;
//    memory[0x9005] = 240;
//    memory[0x9006] = 0;
//    memory[0x9007] = 0;
//    memory[0x9008] = 255;
//    memory[0x9009] = 255;
//    memory[0x900A] = 0;
//    memory[0x900B] = 0;
//    memory[0x900C] = 0;
//    memory[0x900D] = 0;
//    memory[0x900E] = 0;
//    memory[0x900F] = 27;
}

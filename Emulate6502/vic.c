#include "vic.h"
#include <stdio.h>

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
//const rgb565_t palette[16] = {
//    RGB565(0, 0, 0), /* black */
//    RGB565(255, 255, 255), /* white */
//    RGB565(182, 31, 33), /* red */
//    RGB565(77, 240, 255), /* cyan */
//    RGB565(180, 63, 255), /* purple */
//    RGB565(68, 226, 55), /* green */
//    RGB565(26, 52, 255), /* blue */
//    RGB565(220, 215, 27), /* yellow */
//    RGB565(202, 84, 0), /* orange */
//    RGB565(233, 176, 114), /* light orange */
//    RGB565(231, 146, 147), /* light red */
//    RGB565(154, 247, 253), /* light cyan */
//    RGB565(224, 159, 255), /* light purple */
//    RGB565(143, 228, 147), /* light green */
//    RGB565(130, 144, 255), /* light blue */
//    RGB565(229, 222, 133) /* light yellow */
//};

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


/*
 * Location       Bits   Default value	Function
 *  hex   dec
 * $9000 36864	ABBBBBBB $0C Interlace mode / Screen origin (horizontal)
 * $9001 36865	CCCCCCCC $26 Screen origin (vertical)
 * $9002 36866	HDDDDDDD $16 Screen memory location / Number of columns
 * $9003 36867	GEEEEEEF $2E Raster value (lowest bit)/ Number of rows / Double character size
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
void vic_plot_scan_line() {
    //read registers from memory
    byte *memory = memory_get_ptr(0);
    unsigned offset_x = memory[0x9000] & 0x7F;
//    bool interlace_mode = (memory[0x9000] & 0x80) ? true : false;
    unsigned offset_y = memory[0x9001];
    word screen_address = (memory[0x9002] & 0x80) << 2;
    word color_address = screen_address | 0x9400;
    unsigned columns = memory[0x9002] & 0x7F;
    bool wide_characters = (memory[0x9003] & 1) ? true : false;
    unsigned rows = (memory[0x9003] & 0x7F) >> 1;
    unsigned scan_line = (memory[0x9003] & 0x80) >> 7;
    scan_line = (scan_line | memory[0x9004] << 1);
    word character_address = (memory[0x9005] & 0x07) << 10;
    if(!(memory[0x9005] & 0x08))
        character_address |= 0x8000;
    screen_address |= (memory[0x9005] & 0x70) << 6;
    if(!(memory[0x9005] & 0x80))
        screen_address |= 0x8000;
    unsigned auxilary_color = color_palette[memory[0x900E] >> 4];
    unsigned screen_color   = color_palette[memory[0x900F] >> 4];
    unsigned border_color   = color_palette[memory[0x900F] & 0b111];
//    bool inverted_mode = (memory[0x900F] & 0b1000) ? true : false;

    unsigned min_y = offset_y << 1;
    unsigned min_x = offset_x << 1;
    unsigned max_y = min_y + (wide_characters ? (rows<<5) : (rows<<4));
    unsigned max_x = min_x + (columns<<5);
    unsigned line = scan_line << 1;

    if(!scan_line)
        printf("chars=$%04X screen=$%04X color=$%04X ox=%03u oy=%03u rc=%02ux%02u mmx=%03u-%03u mmy=%03u-%03u bc=%02u sc=%02u ac=%02u\n",
            (unsigned)character_address,
            (unsigned)screen_address,
            (unsigned)color_address,
            offset_x, offset_y, rows, columns,
            min_x, max_x, min_y, max_y,
            (unsigned)(memory[0x900F] & 0b111), (unsigned)(memory[0x900F] >> 4), (unsigned)(memory[0x900E] >> 4));
    
    printf("plot(sl=%03u)", scan_line);

    if(line < min_y || line > max_y) {
        if(line < SCREEN_HEIGHT) {
            printf("-B\n");
            // border
            draw_line(0, line, SCREEN_WIDTH, line, border_color);
            ++line;
            draw_line(0, line, SCREEN_WIDTH, line, border_color);
        } else
            printf("-O\n");
    } else {
        // screen
        printf("-S [");
        
        // Left border
        draw_line(0, line, min_x, line, border_color);        
        draw_line(0, line+1, min_x, line+1, border_color);        
        
        unsigned x = min_x;
        unsigned row = wide_characters ? ((line - min_y) >> 5) : ((line - min_y) >> 4);
        unsigned index = wide_characters ? (line - min_y + (row << 5)) : (line - min_y + (row << 4));
        for(unsigned column = 0; column < columns; ++column) {
            // background
            draw_line(x, line,   x+31, line,   screen_color);
            draw_line(x, line+1, x+31, line+1, screen_color);
            
            unsigned char_index = row*columns + column;
            unsigned color = color_palette[memory[color_address + char_index] & 0x0F];
            unsigned character = memory[screen_address + char_index];
            unsigned pixel_index = index + (wide_characters ? (character << 4) : (character << 3));
            unsigned pixels = memory[character_address + pixel_index];

            printf("%c%02X|", character, pixels);

            if(pixels & 0x80) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x40) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x20) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x10) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x08) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x04) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x02) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
            if(pixels & 0x01) {
                draw_line(x, line,   x+3, line,   color);
                draw_line(x, line+1, x+3, line+1, color);
            } x += 4;
        }

        // right border
        draw_line(max_x, line, SCREEN_WIDTH, line, border_color);        
        draw_line(max_x, line+1, SCREEN_WIDTH, line+1, border_color);
        printf("]\n");
    }

    if(++scan_line >= FRAME_LINES) {
        draw_screen();
        scan_line = 0;
    }

    //copy registers to memory
    memory[0x9003] = (memory[0x9003] & 0x7F) | ((scan_line & 1) << 7);
    memory[0x9004] = scan_line >> 1;
}

void vic_set_defaults() {
    byte *memory = memory_get_ptr(0);
    memory[0x9000] = 12; // NTSC: 5
    memory[0x9001] = 38; // NTSC: 25
    memory[0x9002] = 150;
    memory[0x9003] = 46;
    memory[0x9004] = 0;
    memory[0x9005] = 240;
    memory[0x9006] = 0;
    memory[0x9007] = 0;
    memory[0x9008] = 255;
    memory[0x9009] = 255;
    memory[0x900A] = 0;
    memory[0x900B] = 0;
    memory[0x900C] = 0;
    memory[0x900D] = 0;
    memory[0x900E] = 0;
    memory[0x900F] = 27;
}
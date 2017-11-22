#ifndef GRAPHICS_H
#define GRAPHICS_H

#define SCREEN_WIDTH  768
#define SCREEN_HEIGHT 576

#define FRAME_LINES 312

extern unsigned frame_counter;

extern void draw_pixel(unsigned x, unsigned y, unsigned rgb24_color);
extern void draw_line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned rgb24_color);
extern void draw_screen();

#endif
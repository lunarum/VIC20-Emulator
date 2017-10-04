#include <stdio.h>
#include <SDL.h>
#include "VIC-20.h"
#include "graphics.h"

// VICE palette without dither
#define BLACK           0x000000
#define WHITE           0xffffff
#define RED             0xB61F21
#define CYAN            0x4DF0FF
#define PURPLE          0xB43FFF
#define GREEN           0x44E237
#define BLUE            0x1A34FF
#define YELLOW          0xDCD71B
#define ORANGE          0xCA5400
#define LIGHT_ORANGE    0xE9B072
#define LIGHT_RED       0xE79293
#define LIGHT_CYAN      0x9AF7FD
#define LIGHT_PURPLE    0xE09FFF
#define LIGHT_GREEN     0x8FE493
#define LIGHT_BLUE      0x8290FF
#define LIGHT_YELLOW    0xE5DE85
//#define BLACK           0x000000
//#define WHITE           0xffffff
//#define RED             0x782922
//#define CYAN            0x87d6dd
//#define PURPLE          0xa95fb6
//#define GREEN           0x56a049
//#define BLUE            0x40318e
//#define YELLOW          0xbfce73
//#define ORANGE          0xaa7448
//#define LIGHT_ORANGE    0xeab488
//#define PINK            0xb86962
//#define LIGHT_CYAN      0xc7ffff
//#define LIGHT_PURPLE    0xe99ff6
//#define LIGHT_GREEN     0x94e088
//#define LIGHT_BLUE      0x8071cc
//#define LIGHT_YELLOW    0xfeffb3

SDL_Renderer *renderer;


void draw_pixel(unsigned x, unsigned y, unsigned rgb24_color) {
    SDL_SetRenderDrawColor(renderer, (rgb24_color >> 16) & 0xFF, (rgb24_color >> 8) & 0xFF, rgb24_color & 0xFF, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned rgb24_color) {
    SDL_SetRenderDrawColor(renderer, (rgb24_color >> 16) & 0xFF, (rgb24_color >> 8) & 0xFF, rgb24_color & 0xFF, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void draw_screen() {
    //Update the screen
    SDL_RenderPresent(renderer);
}


//Comparison of the known VIC-20 video chips (summary)
//Chip		    6560-101	6561-101
//System		NTSC-M		PAL-B
//Cycles/line	65		    71
//Lines/frame	261		    312
//Crystal		14318181 Hz	4433618 Hz
//Bus clock     crystal/14	crystal/4
//Screen width	201		    229             (208 visible, standard usable 22*8=176)
//Screen height	252		    303             (272 visible, standard usable 23*8=184)
//	768×576 graphics, max 26x33 cols/rows, points of 3x2 pixels

int main(int argc, char* argv[]){
	//First we need to start up SDL, and make sure it went ok
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		fprintf(stderr, "SDL INIT ERROR: %s\n", SDL_GetError());
		return 1;
	}
	
	//Now create a window with title "Hello World" at 100, 100 on the screen with w:640 h:480 and show it
	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//Make sure creating our window went ok
	if (win == NULL){
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return 1;
	}

	//Create a renderer that will draw to the window, -1 specifies that we want to load whichever
	//video driver supports the flags we're passing
	//Flags: SDL_RENDERER_ACCELERATED: We want to use hardware accelerated rendering
	//SDL_RENDERER_PRESENTVSYNC: We want the renderer's present function (update screen) to be
	//synchronized with the monitor's refresh rate
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL){
		SDL_DestroyWindow(win);
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	
    //First clear the renderer
    SDL_RenderClear(renderer);

    // Default 5K VIC-20
    vic20_config(0);
    // Fill with characters
    byte *screen_memory = memory_get_ptr(0x1E00);
    for(int i = 0; i < 256; ++i)
        *screen_memory++ = (byte)i;
    // Fill with colors
    byte *color_memory = memory_get_ptr(0x9600);
    byte color = 0;
    for(int r = 0; r < 23; ++r) {
        for(int c = 0; c < 22; ++c) {
            *color_memory++ = color;
        }
        color = (color + 1) & 0x07;
        if(color == 1)
            ++color; // Skip white as it's the default background color
    }

    // draw screen
    for(int scan = 0; scan < FRAME_LINES; ++scan) {
        vic_plot_scan_line();
    }


    //Take a quick break after all that hard work
//    SDL_Delay(5000);

    int keypress = 0;
    while(!keypress) 
    {
        SDL_Event event;
         while(SDL_PollEvent(&event)) 
         {      
              switch (event.type) 
              {
                  case SDL_QUIT:
	              keypress = -1;
	              break;
                  case SDL_KEYDOWN:
                       keypress = -1;
                       break;
              }
         }
    }

	//Clean up our objects and quit
//	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	
	return 0;
}
//
// VICE ADC/ SBC implementations
//
//#define ADC(get_func, pc_inc)                                                                      \
//    do {                                                                                           \
//        unsigned int tmp_value;                                                                    \
//        unsigned int tmp;                                                                          \
//                                                                                                   \
//        get_func(tmp_value);                                                                       \
//                                                                                                   \
//        if (LOCAL_DECIMAL()) {                                                                     \
//            tmp = (reg_a_read & 0xf) + (tmp_value & 0xf) + (reg_p & 0x1);                          \
//            if (tmp > 0x9) {                                                                       \
//                tmp += 0x6;                                                                        \
//            }                                                                                      \
//            if (tmp <= 0x0f) {                                                                     \
//                tmp = (tmp & 0xf) + (reg_a_read & 0xf0) + (tmp_value & 0xf0);                      \
//            } else {                                                                               \
//                tmp = (tmp & 0xf) + (reg_a_read & 0xf0) + (tmp_value & 0xf0) + 0x10;               \
//            }                                                                                      \
//            LOCAL_SET_ZERO(!((reg_a_read + tmp_value + (reg_p & 0x1)) & 0xff));                    \
//            LOCAL_SET_SIGN(tmp & 0x80);                                                            \
//            LOCAL_SET_OVERFLOW(((reg_a_read ^ tmp) & 0x80) && !((reg_a_read ^ tmp_value) & 0x80)); \
//            if ((tmp & 0x1f0) > 0x90) {                                                            \
//                tmp += 0x60;                                                                       \
//            }                                                                                      \
//            LOCAL_SET_CARRY((tmp & 0xff0) > 0xf0);                                                 \
//        } else {                                                                                   \
//            tmp = tmp_value + reg_a_read + (reg_p & P_CARRY);                                      \
//            LOCAL_SET_NZ(tmp & 0xff);                                                              \
//            LOCAL_SET_OVERFLOW(!((reg_a_read ^ tmp_value) & 0x80) && ((reg_a_read ^ tmp) & 0x80)); \
//            LOCAL_SET_CARRY(tmp > 0xff);                                                           \
//        }                                                                                          \
//        reg_a_write = tmp;                                                                         \
//        INC_PC(pc_inc);                                                                            \
//    } while (0)
//#define SBC(get_func, pc_inc)                                                               \
//    do {                                                                                    \
//        WORD src, tmp;                                                                      \
//                                                                                            \
//        get_func(src)                                                                       \
//        tmp = reg_a_read - src - ((reg_p & P_CARRY) ? 0 : 1);                               \
//        if (reg_p & P_DECIMAL) {                                                            \
//            unsigned int tmp_a;                                                             \
//            tmp_a = (reg_a_read & 0xf) - (src & 0xf) - ((reg_p & P_CARRY) ? 0 : 1);         \
//            if (tmp_a & 0x10) {                                                             \
//                tmp_a = ((tmp_a - 6) & 0xf) | ((reg_a_read & 0xf0) - (src & 0xf0) - 0x10);  \
//            } else {                                                                        \
//                tmp_a = (tmp_a & 0xf) | ((reg_a_read & 0xf0) - (src & 0xf0));               \
//            }                                                                               \
//            if (tmp_a & 0x100) {                                                            \
//                tmp_a -= 0x60;                                                              \
//            }                                                                               \
//            LOCAL_SET_CARRY(tmp < 0x100);                                                   \
//            LOCAL_SET_NZ(tmp & 0xff);                                                       \
//            LOCAL_SET_OVERFLOW(((reg_a_read ^ tmp) & 0x80) && ((reg_a_read ^ src) & 0x80)); \
//            reg_a_write = (BYTE) tmp_a;                                                     \
//        } else {                                                                            \
//            LOCAL_SET_NZ(tmp & 0xff);                                                       \
//            LOCAL_SET_CARRY(tmp < 0x100);                                                   \
//            LOCAL_SET_OVERFLOW(((reg_a_read ^ tmp) & 0x80) && ((reg_a_read ^ src) & 0x80)); \
//            reg_a_write = (BYTE) tmp;                                                       \
//        }                                                                                   \
//        INC_PC(pc_inc);                                                                     \
//    } while (0)
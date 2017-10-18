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
	SDL_Window *win = SDL_CreateWindow("Hello World!", 0, 1080-SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

//    // Fill with characters
//    byte *screen_memory = memory_get_ptr(0x1E00);
//    for(int i = 0; i < 256; ++i)
//        *screen_memory++ = (byte)i;
//    // Fill with colors
//    byte *color_memory = memory_get_ptr(0x9600);
//    byte color = 0;
//    for(int r = 0; r < 23; ++r) {
//        for(int c = 0; c < 22; ++c) {
//            *color_memory++ = color;
//        }
//        color = (color + 1) & 0x07;
//        if(color == 1)
//            ++color; // Skip white as it's the default background color
//    }
//
//    // draw screen
//    for(int scan = 0; scan < FRAME_LINES; ++scan) {
//        vic_plot_scan_line();
//    }
//
//
//    //Take a quick break after all that hard work
//    SDL_Delay(5000);

    cpu_result result;
    SDL_Event event;
    for(;;) {
        result = cpu_run();
//        cpu_logStatus(result);
        if(SDL_PollEvent(&event)) {      
            if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
               break;
        }
    }

	//Clean up our objects and quit
//	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	
	return 0;
}
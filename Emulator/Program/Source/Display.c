#include "Display.h"

const int VIDEO_MEMORY_START = 0x2400;
const int VIDEO_MEMORY_END = 0x3FFF;
const int SCREEN_WIDTH = 224;
const int SCREEN_HEIGHT = 256;

SDL_Window* window = NULL;
SDL_Surface* window_surface = NULL;
SDL_Surface* surface_buffer = NULL;

int init_sdl()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! %s\n", SDL_GetError());
		return -1;
	} 
	else 
	{
		window = SDL_CreateWindow("Space invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 2*SCREEN_WIDTH, 2*SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(window == NULL) 
        {
			printf("Window could not be created! %s\n", SDL_GetError());
			return -1;
		} 
        else 
        {
			window_surface = SDL_GetWindowSurface(window);
			surface_buffer = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
			return 0;
		}
	}
}

void drawPixel(uint32_t color, int x, int y) 
{
	uint32_t* pixel = (uint32_t*) surface_buffer->pixels + x + y * SCREEN_WIDTH; 
	*pixel = color;
}

void render_video_memory(uint8_t* video_memory)
{
    for(int i = 0; i < SCREEN_WIDTH; i++) 
    {
		for(int j = 31; j >= 0; j--) 
        {
			uint8_t x = video_memory[0x2400+i*32+j];
			
            for(int k = 0; k < 8; k++) 
            {
				if((x << k) & 0x80) 
                {
					drawPixel(0xFFFFFF, i, (31 - j)*8 + k);
				} 
                else 
                {
					drawPixel(0x000000, i, (31 - j)*8 + k);
				}
			}
		}
	}
	SDL_BlitScaled(surface_buffer, NULL, window_surface, NULL); 
	if (SDL_UpdateWindowSurface(window)) 
		puts(SDL_GetError());
}
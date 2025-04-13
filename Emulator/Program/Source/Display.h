#pragma once
#include "SDL2/SDL.h"
#include <stdint.h>

extern const int VIDEO_MEMORY_START;
extern const int VIDEO_MEMORY_END;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern SDL_Window* window;
extern SDL_Surface* window_surface;
extern SDL_Surface* surface_buffer;

int init_sdl();
void drawPixel(uint32_t color, int x, int y);
void render_video_memory(uint8_t* video_memory);
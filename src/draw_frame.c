#include "draw_frame.h"
#include "logging.h"

#include <stdio.h>

#include <SDL2/SDL.h>

// SDL2 GPU
SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture* frame_texture;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void free_draw(void)
{
	SDL_DestroyRenderer(rend);
	rend = NULL;

	SDL_DestroyWindow(win);
	win = NULL;

	SDL_Quit();
}

void init_draw(void)
{
	// startup SDL
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        	printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	// create SDL window
        win = SDL_CreateWindow("Camera switcher", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

        if (win == NULL) {
                printf("SDL has failed to create a window with error: %s\n", SDL_GetError());
		return;
        }

        rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        if (rend == NULL) {
                printf("SDL has failed to create a renderer with error: %s\n", SDL_GetError());
		return;
        }

	if (frame_texture == NULL) {
		printf("the camera has failed to initialise");
	}

	// make the screen black
	SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);

	frame_texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_BGR24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_frame(unsigned char* src, int width, int height)
{
	SDL_UpdateTexture(frame_texture, NULL, src, width * 3);

	//SDL_RenderClear(rend);
	SDL_RenderCopy(rend, frame_texture, NULL, NULL );
	SDL_RenderPresent(rend);
}
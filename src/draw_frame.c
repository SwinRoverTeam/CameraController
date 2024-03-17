#include "draw_frame.h"
#include "logging.h"

#include <stdio.h>
#include <SDL2/SDL.h>

// HARDCODED FRAME SIZE
#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

// SDL2 GPU
SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture* frame_texture;

void free_draw(void)
{
	SDL_DestroyRenderer(rend);
	rend = NULL;

	SDL_DestroyWindow(win);
	win = NULL;

	//SDL_Quit();
}

void init_draw(int width, int height)
{
	// startup SDL
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        	printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	// create SDL window
        win = SDL_CreateWindow("Camera switcher", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
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

	frame_texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_YUY2,
        SDL_TEXTUREACCESS_STREAMING,
        FRAME_WIDTH, FRAME_HEIGHT);
}

int draw_frame(unsigned char* src, int width, int height)
{
	SDL_Event* e;

	SDL_PollEvent(e);
	if (e->type == SDL_QUIT) {
		return 1;
	}

	SDL_UpdateTexture(frame_texture, NULL, src, FRAME_WIDTH * 2);

	// create smaller rect for grabbing a frame
	SDL_Rect src_rect;
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.w = width;
	src_rect.h = height;

	SDL_RenderCopy(rend, frame_texture, NULL, &src_rect);
	SDL_RenderPresent(rend);

	return 0;
}
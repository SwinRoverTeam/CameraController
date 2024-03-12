#include "draw_frame.h"
#include "logging.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <SDL2/SDL.h>

// screen settings
static int fd = -1;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
static char* fbp = NULL;
static unsigned int screensize = 0;

// SDL2 GPU
SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture* frame_texture;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void free_frame(void)
{
	free_frame();
	close(fd);

	SDL_DestroyRenderer(rend);
	rend = NULL;

	SDL_DestroyWindow(win);
	win = NULL;

	SDL_Quit();
}

void init_frame(void)
{
	// open frame buffer
	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1) {
		perror("Error opening framebuffer device");
		exit(EXIT_FAILURE);
	}

	// get fixed screen information
	if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		exit(EXIT_FAILURE);
	}

	// get variable screen information
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		exit(EXIT_FAILURE);
	}

	screensize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel) >> 3; /* (>>3): bits to bytes */

	printf("xoffset: %d, yoffset: %d\nxres: %d, yres: %d\nbits_per_pixel: %d, line_length: %d\n",
			vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.line_length);

	// read the screen size from framebuffer device
	vinfo.xres = 800;
	vinfo.yres = 600;
	if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
		perror("Error puting variable information");
		exit(EXIT_FAILURE);
	}

	screensize = finfo.line_length * vinfo.yres;

	printf("xoffset: %d, yoffset: %d\nxres: %d, yres: %d\nbits_per_pixel: %d, line_length: %d\n",
			vinfo.xoffset, vinfo.yoffset, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, finfo.line_length);

	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (fbp == (char *)-1) {
		perror("Error mapping framebuffer device to memory");
		exit(EXIT_FAILURE);
	}

	// startup SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
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

	// make the screen black
	SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);

	frame_texture = SDL_CreateTexture(rend, SDL_PIXELFORMAT_BGR24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_frame(unsigned char* src, int width, int height)
{
	// draw with CPU
	/*
	int x, y;
	unsigned int location = 0;
	int i = 0;
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel >> 3) + (y + vinfo.yoffset) * finfo.line_length;
            		*(fbp + location) = src[i*3];           //B
            		*(fbp + location + 1) = src[i*3 + 1];	//G
            		*(fbp + location + 2) = src[i*3 + 2];	//R
			i++;
		}
	}
	*/

	SDL_UpdateTexture(frame_texture, NULL, src, width * 3);

	//SDL_RenderClear(rend);
	SDL_RenderCopy(rend, frame_texture, NULL, NULL );
	SDL_RenderPresent(rend);
}
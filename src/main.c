#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "draw_framebuffer.h"
#include "video_capture.h"

int main(int argc, char** argv)
{
	int width = 1920, height = 1080;

	printf("Using size: %dx%d.\n", width, height);
	usleep(1 * 1000000); // sleep one second

	unsigned char src_image[width * height * 3];
	init_framebuffer();
	init_video_capture(width, height);
	char key = 0;

	for(;;) {
		key = video_capture(src_image, width, height);
		draw_framebuffer(src_image, width, height);
		if(key == 'q') {
			break;
		}
	}

	free_video_capture();
	free_framebuffer();

	return EXIT_SUCCESS;
}

#include "../src/video_capture.h"
#include "../src/draw_frame.h"
#include "../src/logging.h"

#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(int argc, char** argv)
{
	char* device_paths[] = {"/dev/video0", "/dev/video2", "/dev/video4"};

	init_video_capture(device_paths[0], SCREEN_WIDTH, SCREEN_HEIGHT);
        init_draw(SCREEN_WIDTH, SCREEN_HEIGHT);

	for (;;) {
                unsigned char raw_image[SCREEN_WIDTH * SCREEN_HEIGHT * 3];
                video_capture(raw_image, SCREEN_WIDTH, SCREEN_HEIGHT);

		if (draw_frame(raw_image, SCREEN_WIDTH, SCREEN_HEIGHT) > 0) {
			break;
		}
	}

	free_draw();
	free_video_capture();
	return 1;
}
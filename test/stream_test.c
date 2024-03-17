#include "../src/video_capture.h"
#include "../src/draw_frame.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void signal_handler(int signum)
{
	if (signum == SIGKILL | signum == SIGTERM) {
		free_video_capture();
		free_draw();
		exit(0);
	}
}

int main(int argc, char** argv)
{
        // create signal handler
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		puts("Failed to create signal handler");
		return 1;
	}

	char* device_paths[] = {"/dev/video0", "/dev/video2", "/dev/video4"};

	init_video_capture(device_paths[0], SCREEN_WIDTH, SCREEN_HEIGHT);
        init_draw(SCREEN_WIDTH, SCREEN_HEIGHT);

	for (;;) {
                unsigned char src_raw_image[(SCREEN_WIDTH * 2) * SCREEN_HEIGHT];
                video_capture(src_raw_image, SCREEN_WIDTH, SCREEN_HEIGHT);

		if (draw_frame(src_raw_image, SCREEN_WIDTH, SCREEN_HEIGHT) > 0) {
			break;
		}
	}

        signal_handler(SIGTERM);
}
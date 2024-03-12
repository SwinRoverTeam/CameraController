#include "../src/video_capture.h"
#include "../src/draw_frame.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// compile with
// gcc test/stream_test.c src/video_capture.c src/draw_frame.c -lc -lSDL2

device_info_t* device;

void signal_handler(int signum)
{
	if (signum == SIGKILL | signum == SIGTERM) {
                free_video_capture(device);
                free_frame();
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

	init_video_capture(device, device_paths[0], SCREEN_WIDTH, SCREEN_HEIGHT);
        init_frame();

	for (;;) {
                unsigned char src_raw_image[SCREEN_WIDTH * SCREEN_HEIGHT * 3];
                video_capture(device, src_raw_image, SCREEN_WIDTH, SCREEN_HEIGHT);
		draw_frame(src_raw_image, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

        signal_handler(SIGTERM);
}

#include "logging.h"
#include "serial_ports.h"
#include "video_capture.h"
#include "draw_frame.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 720

int fd;

void quit()
{
	free_video_capture();
	free_draw();
	exit(1);
}

void signal_handler(int signum)
{
	if (signum == SIGKILL | signum == SIGTERM) {
		quit();
	}
}

int check_video_device(char* msg)
{
	char* compare_string = "/dev/video";

	if (strncmp(compare_string, msg, strlen(compare_string)) == 0)
	{
		return 1;
	}

	return 0;
}

char* current_cam = NULL;
void change_cam(char* new_cam)
{
	if (current_cam != NULL)
	{
		free_video_capture();
	}
	
	current_cam = new_cam;
	init_video_capture(current_cam);

	unsigned char* str = "ok";
	send_buffer_port(fd, str, 3);
}

#define BUFFER_SIZE 12
unsigned char buffer[BUFFER_SIZE];
static int msg_len = 0;

void process_cmd(char* cmd)
{
	if (check_video_device(cmd) == 1) {
		change_cam(cmd);
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

	//init_video_capture(device_paths[0], SCREEN_WIDTH, SCREEN_HEIGHT);
	change_cam(device_paths[0]); // /dev/video0 is default cam
        init_draw(SCREEN_WIDTH, SCREEN_HEIGHT);

	// open serial port
	fd = open_port("/dev/ttyACM0", 115200, "8N1", 0);

	if (fd == 1) {
		LOG_ERROR("failed to read from port :(\n");
		close_port(fd);
		//return 0;
	}

	for (;;) {
                unsigned char src_raw_image[(FRAME_WIDTH * 2) * FRAME_HEIGHT];
                video_capture(src_raw_image);

		if (draw_frame(src_raw_image, SCREEN_WIDTH, SCREEN_HEIGHT) > 0) {
			quit();
		}

		if (fd != 1)
		{

			int err = read_port(fd, buffer, BUFFER_SIZE);

			if (err == -1) {
				LOG_ERROR("failed to read from port :(\n");
				close_port(fd);
				return 0;
			} else if (err > 0) {
				msg_len += err;
				// add null terminator inplace of newline
				buffer[err - 1] = 0;
				LOG_INFO("serial read: %s\n", buffer);
				process_cmd(buffer);
			}
		}
	}

        quit();
}
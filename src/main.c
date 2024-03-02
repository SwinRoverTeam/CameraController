#include "logging.h"
#include "draw_frame.h"
#include "video_capture.h"
#include "serial_ports.h"
#include "panels.h"
#include "linux_devices.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// file descriptor for serial port
int fd;

void initialise_serial(int &fd)
{
	// open a new serial port to cube
	fd = open_port("/dev/ttyTHS1", 115200, "8N1", 0);

	if (fd == 1) {
		LOG_ERROR("Failed to open serial port\n");
	}
}

void list_cameras(void)
{
	// list video devices in dev
	device_tree_t video_devices;
	list_video_devices(&video_devices);

	if (video_devices.num_devices == 0) {
		LOG_INFO("Could not find any connected video devices\n");
		return;
	}

	for (size_t i = 0; i < video_devices.num_devices; i++) {
		LOG_INFO("%s\n", video_devices.device_name[i]);
		int buffer_len = strlen(video_devices.device_name);
		send_buffer_port(fd, video_devices.device_name, buffer_len);
	}

	clear_device_list(video_devices);
}

void change_camera(char* device)
{
}

void start_stream(void)
{
}

void end_stream(void)
{
}

void reset_application(void)
{
}

void stack_stream(char* stack_direction, char* device)
{
	
}

void shutdown(void)
{
	LOG_INFO("Application shutdown by user\n");

	free_video_capture();
	free_framebuffer();
}

void signal_handler(int signum)
{
	if (signum == SIGKILL | signum == SIGTERM) {
		shutdown();
		exit(0);
	}
}

int main(int argc, char** argv)
{
	// create signal handler
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		LOG_ERROR("Failed to create signal handler\n");
		return 1;
	}

	// set video framebuffer size
	int width = 640;
	int height = 480;

	printf("Using size: %dx%d.\n", width, height);
	usleep(1 * 1000000); // sleep one second

	unsigned char src_image[width * height * 3];

	init_framebuffer();
	init_video_capture(width, height);

	for (;;) {
		key = video_capture(src_image, width, height);
		draw_framebuffer(src_image, width, height);
	}

	return EXIT_SUCCESS;
}

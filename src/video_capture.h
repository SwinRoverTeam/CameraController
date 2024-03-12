#ifndef VIDEO_CAPTURE_H_
#define VIDEO_CAPTURE_H_

#include <stddef.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
	void * start;
	size_t length;
};

typedef struct device_info {
	int fd;
	int n_buffers;
	struct buffer* buffers;
} device_info_t;

int init_video_capture(device_info_t* dev, char* dev_name, int width, int height);
char video_capture(device_info_t* dev, unsigned char* dst, int width, int height);
void free_video_capture(device_info_t* dev);

#endif // VIDEO_CAPTURE_H_

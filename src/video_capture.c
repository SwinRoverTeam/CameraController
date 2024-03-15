#include "video_capture.h"
#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>              /* low-level i/o control (open)*/
#include <errno.h>
#include <string.h>             /* strerror show errno meaning */
#include <sys/stat.h>           /* getting information about files attributes */
#include <linux/videodev2.h>    /* v4l2 structure */
#include <sys/mman.h>           /* memory mapping */
#include <unistd.h>             /* read write close */
#include <sys/time.h>           /* for select time */
#include <limits.h>             /* for UCHAR_MAX */
#include <dirent.h>				/* list files in dev */
#include <string.h>				/* string compare */


/* wrapped errno display function by v4l2 API */
static void errno_exit(const char * s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

/* wrapped ioctrl function by v4l2 API */
static int xioctl(device_info_t* dev, int request, void * arg)
{
	int r;
	do{
		r = ioctl(dev->fd, request, arg);
	}
	while(-1 == r && EINTR == errno);
	return r;
}

static void open_device(device_info_t* dev, char* dev_name)
{
	dev->fd = -1; // camera file descriptor

	struct stat st;
	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}
	dev->fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
	if (-1 == dev->fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/*
 * 1. Memory mapped buffers are located in device memory and must be allocated with this ioctl
 * 	  before they can be mapped into the application's address space
 * 2. set four images in buffer
 */
static void init_mmap(device_info_t* dev)
{
	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count = 2;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	/* Initiate Memory Mapping */
	if (-1 == xioctl(dev, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "does not support memory mapping\n");
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	/* video output requires at least two buffers, one displayed and one filled by the application */
	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory\n");
		exit(EXIT_FAILURE);
	}
	dev->buffers = calloc(req.count, sizeof(struct buffer*));

	if (!dev->buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (dev->n_buffers = 0; dev->n_buffers < req.count; ++dev->n_buffers) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = dev->n_buffers;

		/* Query the status of a buffer */
		if (-1 == xioctl(dev, VIDIOC_QUERYBUF, &buf)){
			errno_exit("VIDIOC_QUERYBUF");
		}

		dev->buffers[dev->n_buffers].length = buf.length; //640 * 480 * 2 = 614400
		dev->buffers[dev->n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
		PROT_READ | PROT_WRITE /* required */,
		MAP_SHARED /* recommended */, dev->fd, buf.m.offset);

		if (MAP_FAILED == dev->buffers[dev->n_buffers].start){
			errno_exit("mmap");
		}
	}
}

/* set video streaming format here(width, height, pixel format, cropping, scaling) */
void init_device(device_info_t* dev, int width, int height)
{
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(dev, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "no V4L2 device\n");
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "no video capture device\n");
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}

	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if (-1 == xioctl(dev, VIDIOC_S_FMT, &fmt)) {
		errno_exit("VIDIOC_S_FMT");
	}

	/* YUYV sampling 4 2 2, so bytes per pixel is 2*/
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min) {
		fmt.fmt.pix.bytesperline = min;
	}

	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min) {
		fmt.fmt.pix.sizeimage = min;
	}

	init_mmap(dev);
}

static void start_capturing(device_info_t* dev)
{
	unsigned int i;
	enum v4l2_buf_type type;
	
	for (i = 0; i < dev->n_buffers; ++i) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		/* enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue */
		if (-1 == xioctl(dev, VIDIOC_QBUF, &buf)) {
			errno_exit("VIDIOC_QBUF");
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	/* Start streaming I/O */
	if (-1 == xioctl(dev, VIDIOC_STREAMON, &type)){
		errno_exit("VIDIOC_STREAMON");
	}
}

static void close_device(device_info_t* dev)
{
	if (-1 == close(dev->fd))
		errno_exit("close");
	dev->fd = -1;
}

static void stop_capturing(device_info_t* dev)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(dev, VIDIOC_STREAMOFF, &type)){
		errno_exit("VIDIOC_STREAMOFF");
	}
}

static void uninit_device(device_info_t* dev)
{
	unsigned int i;
	for (i = 0; i < dev->n_buffers; ++i){
		if (-1 == munmap(dev->buffers[i].start, dev->buffers[i].length)){
			errno_exit("munmap");
		}
	}
	
	free(dev->buffers);
}

// convert a YUY2 format into BGR24
static void parse_im(const unsigned char *im_yuv, unsigned char *dst, int width, int height)
{
    const int IM_SIZE = width * height;

    unsigned char Y = 0;
    unsigned char U = 0;
    unsigned char V = 0;

    int B = 0;
    int G = 0;
    int R = 0;

    int i;
    for(i = 0; i < IM_SIZE; ++i) {

	// pixel:  0   1   2   3  ,  4   5   6   7
	//	  [Y0][U0][Y1][V0], [Y2][U1][Y3][V1]

        if(!(i & 1)) {
            U = im_yuv[2 * i + 1];
            V = im_yuv[2 * i + 3];
        }
        Y = im_yuv[2 * i];

        B = Y + 1.773 * (U - 128);
        G = Y - 0.344 * (U - 128) - (0.714 * (V - 128));
        R = Y + 1.403 * (V - 128);

        if(B > UCHAR_MAX) {
            B = UCHAR_MAX;
        }
        if(G > UCHAR_MAX) {
            G = UCHAR_MAX;
        }
        if(R > UCHAR_MAX) {
            R = UCHAR_MAX;
        }

        dst[3*i] = B;
        dst[3*i+1] = G;
        dst[3*i+2] = R;
    }
}

int init_video_capture(device_info_t* dev, char* dev_name, int width, int height)
{
	open_device(dev, dev_name);
	init_device(dev, width, height);
	start_capturing(dev);
}

char video_capture(device_info_t* dev, unsigned char* dst, int width, int height)
{
	struct v4l2_buffer buf_in_while_loop;
	struct timeval tv;
	static fd_set fds;


	FD_ZERO(&fds);
	FD_SET(dev->fd, &fds);
	FD_SET(fileno(stdin), &fds);

	/* Timeout. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	select(dev->fd + 1, &fds, NULL, NULL, &tv);

	if(FD_ISSET(dev->fd, &fds)) {
		CLEAR(buf_in_while_loop);
		buf_in_while_loop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf_in_while_loop.memory = V4L2_MEMORY_MMAP;

		/* dequeue from buffer */
		if(-1 == xioctl(dev, VIDIOC_DQBUF, &buf_in_while_loop)) {
			switch(errno) {
			case EAGAIN:
				return 0;
			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}
        	
		// convert to BGR24 format
		unsigned char* im_from_cam = (unsigned char*)dev->buffers[buf_in_while_loop.index].start;
		parse_im(im_from_cam, dst, width, height);
		
		// copy without convert (faster but doesn't work with sdl2???)
		// dst = (unsigned char*)buffers[buf_in_while_loop.index].start;

		/* queue-in buffer */
		if(-1 == xioctl(dev, VIDIOC_QBUF, &buf_in_while_loop)) {
			errno_exit("VIDIOC_QBUF");
		}
	}

	return 0;
}

void free_video_capture(device_info_t* dev)
{
	stop_capturing(dev);
	uninit_device(dev);
	close_device(dev);
}

#define DEVICE_DIR "/dev"
#define DEIVCE_PREFIX "video"

int video_capability(struct v4l2_capability* cap, char* file_path)
{
	int f = open(file_path, O_RDWR);

	// no such file
	if (!f) {
		return 0;
	}

	if (ioctl(f, VIDIOC_QUERYCAP, &cap) < 0) {
		close(f);
		return 0;
	}

	// print names
	printf("device info: %s\n", cap->driver);

	close(f);
	return 1;
}
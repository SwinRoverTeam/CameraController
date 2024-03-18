#include "draw_frame.h"
#include "logging.h"

#include <stdio.h>
#include <limits.h>

// GPU stuff
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

Display *display;
Window window;
XVisualInfo *visualInfo;
Colormap colormap;
GLXContext glxContext;

void free_draw(void)
{
	if (glxContext){
		glXDestroyContext(display, glxContext);
	}

	if (window){
		XDestroyWindow(display, window);
	}

	if (colormap){
		XFreeColormap(display, colormap);
	}

	if (visualInfo){
		XFree(visualInfo);
	}

	if (display){
		XCloseDisplay(display);
	}
}

void init_draw(int width, int height)
{
	display = XOpenDisplay(NULL);
	if (!display)
	{
		LOG_ERROR("Failed to open X display\n");
		return;
	}

	int screen = DefaultScreen(display);

	static int visualAttribs[] = {
	    GLX_RGBA,
	    GLX_DEPTH_SIZE, 24,
	    GLX_DOUBLEBUFFER,
	    None};

	visualInfo = glXChooseVisual(display, screen, visualAttribs);
	if (!visualInfo)
	{
		LOG_ERROR("Failed to find suitable visual\n");
		return;
	}

	colormap = XCreateColormap(display, RootWindow(display, visualInfo->screen), visualInfo->visual, AllocNone);

	XSetWindowAttributes windowAttribs;
	windowAttribs.colormap = colormap;
	windowAttribs.event_mask = ExposureMask | KeyPressMask;

	window = XCreateWindow(display, RootWindow(display, visualInfo->screen), 0, 0, width, height, 0,
			       visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &windowAttribs);

	XMapWindow(display, window);

	const char *glxExtensions = glXQueryExtensionsString(display, screen);
	if (!glXQueryExtension(display, NULL, NULL) || !glXQueryVersion(display, NULL, NULL))
	{
		LOG_ERROR("GLX extension not available\n");
		return;
	}

	glxContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
	if (!glxContext)
	{
		LOG_ERROR("Failed to create GLX context\n");
		return;
	}

	// select buffers
	glXMakeCurrent(display, window, glxContext);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void draw(unsigned char *data, int w, int h)
{
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Set up 2D projection
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render the provided pixel data
	glRasterPos2i(0, h);
	glPixelZoom(1, -1);
	glDrawPixels(w, h, GL_BGR, GL_UNSIGNED_BYTE, data);

	// Swap buffers
	glXSwapBuffers(display, window);
}

int draw_frame(unsigned char *src, int w, int h)
{
	XEvent event;

	draw(src, w, h);	
	
	// check event if available
	if (XPending(display)) {
		XNextEvent(display, &event);
		if (event.type == KeyPress)
		{
			return 1;
		}
	}

	return 0;
}
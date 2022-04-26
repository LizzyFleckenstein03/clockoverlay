#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static inline unsigned long _RGB(int r, int g, int b)
{
	return b + (g << 8) + (r << 16);
}

int main(void) {	
	Display *display = XOpenDisplay(NULL);
	assert(display);

	int fd = ConnectionNumber(display);

	XVisualInfo vinfo;
	XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

	XSetWindowAttributes attribs;
	attribs.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
	attribs.override_redirect = 1;
	attribs.border_pixel = 0;
	attribs.background_pixel = 0;

	Window window = XCreateWindow(display, DefaultRootWindow(display), 10, 10, 24 / 2 * 8, 24, 0,
		vinfo.depth, InputOutput, vinfo.visual,
		CWColormap | CWOverrideRedirect | CWBorderPixel | CWBackPixel, &attribs);
	XMapWindow(display, window);

	Font font = XLoadFont(display, "*x24");

	GC ctx = XCreateGC(display, window, 0, NULL);
	XSetForeground(display, ctx, _RGB(255, 255, 255));
	XSetFont(display, ctx, font);

	for (;;) {
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);

		char buf[9];
		strftime(buf, 9, "%H:%M:%S", tm);
		XClearWindow(display, window);
		XDrawString(display, window, ctx, 0, 24, buf, 8);

		XEvent event;
		while (XPending(display))
			XNextEvent(display, &event);

		fd_set set;
		FD_ZERO(&set);
		FD_SET(fd, &set);
		select(fd + 1, &set, NULL, NULL, &(struct timeval) {1, 0});
	}
}

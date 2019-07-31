//
// (c) 2019 Antmicro <www.antmicro.com>
//

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define WIDTH 640
#define HEIGHT 480

static Display *display = NULL;
static Window window;
static XImage *img;
static int width = 0;
static int height = 0;

// get framebuffer data
uint32_t *fb_getdata() {
	return (uint32_t*)img->data;
}

// redraw fb
void fb_redraw() {
	XPutImage(display,window,DefaultGC(display,DefaultScreen(display)),img,0,0,0,0,width,height);
	XFlush(display);
}

// create fb window
void init_fb_window(int w, int h) {
	static int init = 1;
	if (init) {
		display = XOpenDisplay(NULL);
		Window root = RootWindow(display,DefaultScreen(display));
        	window = XCreateSimpleWindow(display,root,50,50,WIDTH,HEIGHT,1,0,0); // TODO: should be centered?
        	XSelectInput(display,window,/*ExposureMask |*/ ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask);
        	XMapWindow(display,window);
		width = w;
		height = h;
		init = 0;
		img = XCreateImage(display,DefaultVisual(display,DefaultScreen(display)),DefaultDepth(display,DefaultScreen(display)),ZPixmap, 0,(char*)malloc(w*h*4),w,h,32,0);
	}
}

// cleanup
void free_fb_window() {
	// TODO
}

// get ms epoch
unsigned long long msepoch() {
	struct timeval tvl;
	gettimeofday(&tvl, NULL);

	return 
    (unsigned long long)(tvl.tv_sec) * 1000 +
    (unsigned long long)(tvl.tv_usec) / 1000;
}

int main(int argc,char **argv)
{
        init_fb_window(WIDTH, HEIGHT);

        XEvent event;
	printf("Press ESC or 'q' to exit.\n");

	// main loop
        while(1) {
		unsigned long long initial_ms = msepoch();
		// generate some noise
		uint32_t *data = fb_getdata();
		for (int x = 0; x < WIDTH; x++) for (int y = 0; y < HEIGHT; y++) data[x+y*WIDTH] = rand() % 0xFFFFFF;
		fb_redraw();
		if(XCheckWindowEvent(display, window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask, &event)) {
                    if(event.type == KeyPress) {
		    	if (event.xkey.keycode == 0x9 || event.xkey.keycode == 0x18) break;
			printf("Key press, code 0x%x\n", event.xkey.keycode);
		    }
			printf("key / mouse event.\n");
                }
		int64_t sleep_time = 16 - msepoch() + initial_ms;
		if (sleep_time > 0) usleep(1000*sleep_time); // wait to 1/60s
        }
	free_fb_window();
        return 0;
}

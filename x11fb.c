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
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

static Display *display = NULL;
static Window window;
static int width = 0;
static int height = 0;

void fb_redraw(uint32_t *data) {
	char *_data = (char*)malloc(width*height*4);
	memcpy(_data, data, width*height*4);
	XImage *img = XCreateImage(display,DefaultVisual(display,DefaultScreen(display)),DefaultDepth(display,DefaultScreen(display)),ZPixmap, 0,_data,width,height,32,0);
	XPutImage(display,window,DefaultGC(display,DefaultScreen(display)),img,0,0,0,0,width,height);
	XDestroyImage(img); // frees _data
}

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
	}
}

int main(int argc,char **argv)
{
        init_fb_window(WIDTH, HEIGHT);

        XEvent event;
	uint32_t data[WIDTH * HEIGHT * 4];
	printf("Press ESC or 'q' to exit.\n");
	uint64_t fps = 0;
	uint64_t old_time = time(NULL);
        while(1) {
	        // generate some noise
		for (int x = 0; x < WIDTH; x++) for (int y = 0; y < HEIGHT; y++) data[x+y*WIDTH] = rand() % 0xFFFFFF;
		fb_redraw(data);
		if(XCheckWindowEvent(display, window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask, &event)) {
                    if(event.type == KeyPress) {
		    	if (event.xkey.keycode == 0x9 || event.xkey.keycode == 0x18) break;
			printf("Key press, code 0x%x\n", event.xkey.keycode);
		    }
			printf("key / mouse event.\n");
                }
		usleep(8500); // TODO: should be clocked precisely
		fps++;
		uint64_t new_time = time(NULL);
		if (new_time - old_time) { // TODO
			printf("fps per %lus: %.2f\n", new_time-old_time, fps / (double)(new_time - old_time));
			fps = 0;
			old_time = new_time;
		}
        }
        return 0;
}

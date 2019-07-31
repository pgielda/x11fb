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

#define WIDTH 640
#define HEIGHT 480

Display *display = NULL;

void draw_image(Window window, uint32_t *data, int w, int h) {
	char *_data = (char*)malloc(w*h*4);
	memcpy(_data, data, w*h*4);
	XImage *img = XCreateImage(display,DefaultVisual(display,DefaultScreen(display)),DefaultDepth(display,DefaultScreen(display)),ZPixmap, 0,_data,w,h,32,0);
	XPutImage(display,window,DefaultGC(display,DefaultScreen(display)),img,0,0,0,0,WIDTH,HEIGHT);
	XDestroyImage(img); // frees _data
}

Window create_fb_window(int w, int h) {
	if (!display) display = XOpenDisplay(NULL);
	Window root = RootWindow(display,DefaultScreen(display));
        Window win = XCreateSimpleWindow(display,root,50,50,WIDTH,HEIGHT,1,0,0);
        XSelectInput(display,win,/*ExposureMask |*/ ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask);
        XMapWindow(display,win);
	return win;
}

int main(int argc,char **argv)
{
        Window win = create_fb_window(WIDTH, HEIGHT);

        XEvent event;
	uint32_t data[WIDTH * HEIGHT * 4];
	printf("Press ESC to exit.\n");
        while(1) {
	        // generate some noise
		for (int x = 0; x < WIDTH; x++) for (int y = 0; y < HEIGHT; y++) data[x+y*WIDTH] = rand() % 0xFFFFFF;
		draw_image(win, data, WIDTH, HEIGHT);
		if(XCheckWindowEvent(display, win, ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask, &event)) {
                    if(event.type == KeyPress) {
		    	if (event.xkey.keycode == 0x9)	 break;
			printf("Key press, code 0x%x\n", event.xkey.keycode);
		    }
			printf("key / mouse event.\n");
                }
		usleep(8500); // TODO: should be clocked precisely
        }
        return 0;
}

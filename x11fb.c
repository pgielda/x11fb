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

typedef struct {
	Window window;
	XImage *image;
	int width;
	int height;
} _internal_surface;

#define MAX_SURFACES 255

static Display *display = NULL;
static _internal_surface surfaces[MAX_SURFACES];
int surface_count = 0;

// get framebuffer data
uint32_t *fb_getdata(int surface_id) {
	return (uint32_t*)surfaces[surface_id].image->data;
}

// partial redraw
void fb_redraw_rect(int surface_id, int x, int y, int w, int h) {
        XPutImage(display,surfaces[surface_id].window,DefaultGC(display,DefaultScreen(display)),surfaces[surface_id].image,x,y,x,y,w,h);
        XFlush(display);
}

// redraw whole fb
void fb_redraw(int surface_id) {
	fb_redraw_rect(surface_id, 0, 0, surfaces[surface_id].width,surfaces[surface_id].height);
}

int create_fb_window(int w, int h) {
	if (!display) {
		display = XOpenDisplay(NULL);
	}
	Window root = RootWindow(display,DefaultScreen(display));
	surfaces[surface_count].width = w;
	surfaces[surface_count].height = h;
	surfaces[surface_count].window = XCreateSimpleWindow(display,root,50,50,surfaces[surface_count].width,surfaces[surface_count].height,1,0,0); // TODO: should be centered?
        XSelectInput(display,surfaces[surface_count].window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask);
        XMapWindow(display,surfaces[surface_count].window);
	surfaces[surface_count].image = XCreateImage(display,DefaultVisual(display,DefaultScreen(display)),DefaultDepth(display,DefaultScreen(display)),ZPixmap, 0,(char*)malloc(w*h*4),w,h,32,0);
	surface_count++;
	return (surface_count - 1);
}

// cleanup
void free_fb_window(int surface_id) {
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
        int sid = create_fb_window(WIDTH, HEIGHT);

        XEvent event;
	printf("Press ESC or 'q' to exit.\n");

	// main loop
        while(1) {
		unsigned long long initial_ms = msepoch();
		// generate some noise
		uint32_t *data = fb_getdata(sid);
		for (int x = 0; x < WIDTH; x++) for (int y = 0; y < HEIGHT; y++) data[x+y*WIDTH] = rand() % 0xFFFFFF;
		fb_redraw(sid);
		if(XCheckWindowEvent(display, surfaces[sid].window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask, &event)) {
                    if(event.type == KeyPress) {
		    	if (event.xkey.keycode == 0x9 || event.xkey.keycode == 0x18) break;
			printf("Key press, code 0x%x\n", event.xkey.keycode);
		    }
			printf("key / mouse event.\n");
                }
		int64_t sleep_time = 16 - msepoch() + initial_ms;
		if (sleep_time > 0) usleep(1000*sleep_time); // wait to 1/60s
        }
	free_fb_window(sid);
        return 0;
}

// gfx.c
#include "gfx.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Display *gfx_display = 0;
static Window gfx_window;
static GC gfx_gc;
static Colormap gfx_colormap;
static int gfx_width, gfx_height;
static XFontStruct *gfx_font;

void gfx_open(int width, int height, const char *title) {
    gfx_display = XOpenDisplay(0);
    if (!gfx_display) {
        fprintf(stderr, "Unable to open display.\n");
        exit(1);
    }

    int screen = DefaultScreen(gfx_display);
    gfx_colormap = DefaultColormap(gfx_display, screen);
    gfx_window = XCreateSimpleWindow(gfx_display, RootWindow(gfx_display, screen),
                                     0, 0, width, height, 0,
                                     BlackPixel(gfx_display, screen),
                                     WhitePixel(gfx_display, screen));
    XSetStandardProperties(gfx_display, gfx_window, title, title, None, NULL, 0, NULL);
    XSelectInput(gfx_display, gfx_window, ExposureMask | KeyPressMask);
    gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);
    XSetBackground(gfx_display, gfx_gc, WhitePixel(gfx_display, screen));
    XSetForeground(gfx_display, gfx_gc, BlackPixel(gfx_display, screen));

    gfx_font = XLoadQueryFont(gfx_display, "fixed");
    if (!gfx_font) {
        fprintf(stderr, "Unable to load font.\n");
        exit(1);
    }
    XSetFont(gfx_display, gfx_gc, gfx_font->fid);

    XMapRaised(gfx_display, gfx_window);
    gfx_width = width;
    gfx_height = height;
}

void gfx_close() {
    XCloseDisplay(gfx_display);
}

void gfx_clear() {
    XClearWindow(gfx_display, gfx_window);
}

void gfx_flush() {
    XFlush(gfx_display);
}

void gfx_color(unsigned short r, unsigned short g, unsigned short b) {
    XColor color;
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(gfx_display, gfx_colormap, &color);
    XSetForeground(gfx_display, gfx_gc, color.pixel);
}

void gfx_clear_color(unsigned short r, unsigned short g, unsigned short b) {
    XColor color;
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(gfx_display, gfx_colormap, &color);
    XSetWindowBackground(gfx_display, gfx_window, color.pixel);
}

void gfx_circle(int x, int y, int radius) {
    XDrawArc(gfx_display, gfx_window, gfx_gc, x - radius, y - radius,
             radius * 2, radius * 2, 0, 360 * 64);
}

void gfx_line(int x1, int y1, int x2, int y2) {
    XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

void gfx_text(int x, int y, const char *text) {
    XDrawString(gfx_display, gfx_window, gfx_gc, x, y, text, strlen(text));
}

int gfx_event_waiting() {
    XEvent event;
    while (XPending(gfx_display) > 0) {
        XPeekEvent(gfx_display, &event);
        if (event.type == KeyPress) return 1;
        XNextEvent(gfx_display, &event);
    }
    return 0;
}

char gfx_wait() {
    XEvent event;
    XNextEvent(gfx_display, &event);
    if (event.type == KeyPress)
        return XLookupKeysym(&event.xkey, 0);
    return 0;
}


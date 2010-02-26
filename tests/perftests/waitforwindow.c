#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    Display *dpy;
    int screen = 0;
    Window rootw;
    XEvent event;
    char *DISPLAY = NULL;

    DISPLAY = (char*)getenv("DISPLAY");
    if (DISPLAY == NULL) {
        printf("Cannot open display. DISPLAY variable not set.\n");
        exit(1);
    }

    dpy = XOpenDisplay(DISPLAY);
    screen = XDefaultScreen(dpy);
    rootw = RootWindow(dpy, screen);
    
    XSelectInput(dpy, rootw, SubstructureNotifyMask | VisibilityChangeMask);

    while (1) {
        XNextEvent(dpy, &event);
        if (event.type == CreateNotify) {
            printf("CreateNotify on window 0x%x\n", 
                   (unsigned int)(((XCreateWindowEvent*)&event)->window));
        } 
        else if (event.type == MapNotify) {
            printf("MapNotify on window 0x%x\n", 
                   (unsigned int)(((XMapEvent*)&event)->window));            
            printf("Quit!\n"); break;
        } 
        else if (event.type == VisibilityNotify) {
            printf("VisibilityNotify on window 0x%x\n", 
                   (unsigned int)(((XVisibilityEvent*)&event)->window));
        }
    }
    return 0;
}

/*
 * waitforwindow.c
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

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

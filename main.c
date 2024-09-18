#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define true 1
#define false 0

Display *dpy;
Window root;

void
DiesOfDeath(char *DeathMessage) {
	fprintf(stderr, "%s", DeathMessage);
	exit(1);
}

void
InitialChecks() {
	if (!(dpy = XOpenDisplay(NULL))) DiesOfDeath("Can't open X Display\n");
	else puts("Opened X Display");

	puts("Passed Initial Checks");
}

void
Loop() {
	XButtonEvent start;
	XEvent ev;

	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("F1")), Mod1Mask,
			DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);

    XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True,
            ButtonPressMask|ButtonReleaseMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    start.subwindow = None;

	puts("Entered main Loop");
	for (;;)
	{
		XNextEvent(dpy, &ev);
	}
}

int
main(int argc, char **argv) {



	InitialChecks();
	Loop();

	
	XCloseDisplay(dpy);
	return 0;
}

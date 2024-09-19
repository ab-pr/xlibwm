#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Display *dpy;

void Error(char*, int, int);
void InitialChecks(int, char*);
void Run();
void Cases(XEvent*, KeyCode);

void HandleConfigureRequest(XWindowChanges *changes, XEvent *e);

void InitialChecks(int argc, char *argv) {
	/* Initial checks to make sure the program can run */
	if (!(dpy = XOpenDisplay(NULL)))
		Error("Can't open the display\n", EXIT_FAILURE, True);
}

void
Error(char *ErrorMessage, int ERROR_CODE, int EXIT) {
	/* Outputs error message and dies 💀 */
	fprintf(stderr, "%s", ErrorMessage);

	if (EXIT)
		exit(ERROR_CODE);
}

void Configure() {

}

void
Run() {
	/* Main loop of program */
	Window root = DefaultRootWindow(dpy);
	int windowCount;
	windowCount = 0;
	++windowCount;
    // Capture the "Alt+Return" key combination
    KeyCode keycode = XKeysymToKeycode(dpy, XStringToKeysym("Return"));
    XGrabKey(dpy, keycode, Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);


	XSelectInput(dpy, root, SubstructureNotifyMask | SubstructureRedirectMask);
	for(;;) {
		
		XEvent e;
		XNextEvent(dpy, &e);
		Cases(&e, keycode);

	}
}

void
Cases(XEvent *e, KeyCode keycode) {
    switch (e->type) {

		default: break;
        case MapRequest: { XMapWindow(dpy, e -> xmaprequest.window); break; }
        case DestroyNotify: { XDestroyWindow(dpy, e -> xdestroywindow.window); break; }

        case ConfigureRequest: {
            XWindowChanges changes;
            HandleConfigureRequest(&changes, e);
            break;
        }

        case KeyPress: {
            if (e->xkey.keycode == keycode && (e->xkey.state & Mod1Mask)) {
                if (fork() == 0) {
                    execlp("st", "", NULL);
                    exit(0);
                }
            }
            break;
        }
    }
}

void
HandleConfigureRequest(XWindowChanges *changes, XEvent *e) {
	if (e->xconfigurerequest.width > 0 && e->xconfigurerequest.height > 0) {
	changes -> x = 			e -> xconfigurerequest.x;
	changes -> y = 			e -> xconfigurerequest.y;
	changes -> width = 1000;		//e -> xconfigurerequest.width;
	changes -> height = 700;		//e -> xconfigurerequest.height;
	changes -> border_width = 	e -> xconfigurerequest.border_width;
	changes -> stack_mode = 	e -> xconfigurerequest.detail;
	changes -> sibling = 		e -> xconfigurerequest.above;
	} else {
		Error("Invalid width or height in ConfigurationRequest event", -1, False);
	}

}

int
main(int argc, char *argv[]) {
	InitialChecks(argc, *argv);
	Configure(); /* TODO */
	Run();

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

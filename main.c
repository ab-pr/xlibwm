#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "structs.h"
#include "config.h"

#define SCREEN_WIDTH    XDisplayWidth(dpy, DefaultScreen(dpy))
#define SCREEN_HEIGHT   XDisplayHeight(dpy, DefaultScreen(dpy))
#define TITLEBAR_HEIGHT	5


/* Variables */
Display *dpy;

int GAPS_SIZE = 15;
int BORDER_THICKNESS = 3;
int openWindows = 0;


/* Function Declarations*/

// Main Loop
void Cases(XEvent *, KeyCode *);
void Configure(void);
void Run(void);
void InitialChecks(int, char *);

// Requests
void ConfigureWindowRequest(XEvent *);
void HandleConfigureRequest(XWindowChanges *, XConfigureRequestEvent *);

// Misc
void Error(char *, int, int);


/* Functions */
void
InitialChecks(int argc, char *argv)
{
	/* Initial checks to make sure the program can run */
	if (!(dpy = XOpenDisplay(NULL)))
		Error("Can't open the display\n", EXIT_FAILURE, True);
}

void
Error(char *ErrorMessage, int ERROR_CODE, int EXIT)
{
	/* Outputs error message and dies 💀 */
	fprintf(stderr, "%s", ErrorMessage);
		exit(ERROR_CODE);
}

void
Configure()
{
	/* Configures the custom settings set by the user */
}

void
Run()
{
	/* Main loop of program */
	Window root = DefaultRootWindow(dpy);

	KeyCode enter = XKeysymToKeycode(dpy, XStringToKeysym("Return"));
    XGrabKey(dpy, enter, Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);

	/* Configure();  TODO */
	XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

	for(;;) {
		XEvent e;
		XNextEvent(dpy, &e);
		Cases(&e, &enter);
	}
}

void
ConfigureWindowRequest(XEvent *e)
{
    XWindowChanges changes;
    XConfigureRequestEvent *configRequest = &e->xconfigurerequest;

    configRequest->height = SCREEN_HEIGHT - ((GAPS_SIZE * 2) + (BORDER_THICKNESS * 2)) - TITLEBAR_HEIGHT;
    configRequest->width = SCREEN_WIDTH - ((GAPS_SIZE * 2) + (BORDER_THICKNESS * 2));

    configRequest->x = (SCREEN_WIDTH / 2) - (configRequest->width / 2);
    configRequest->y = (SCREEN_HEIGHT / 2) - (configRequest->height / 2) + TITLEBAR_HEIGHT;

    configRequest->border_width = BORDER_THICKNESS;
    configRequest->above = None;
    configRequest->detail = Above;
    configRequest->value_mask = CWX | CWY | CWWidth | CWHeight;

	XSetWindowBorderWidth(dpy, e->xmaprequest.window, BORDER_THICKNESS);

	XColor color;
	color.red = 255 * 256;
	color.green = 0 * 256;
	color.blue = 0 * 256;
	color.flags = DoRed | DoGreen | DoBlue;

	Colormap colormap = DefaultColormap(dpy, DefaultScreen(dpy));
	if (XAllocColor(dpy, colormap, &color)) {
		XSetWindowBorder(dpy, e->xmaprequest.window, color.pixel);
	} else
		fprintf(stderr, "Error: Failed to allocate color for window border\n");

    HandleConfigureRequest(&changes, configRequest);
}

void
HandleConfigureRequest(XWindowChanges *changes, XConfigureRequestEvent *ev)
{
    changes->x = ev->x;
    changes->y = ev->y;
    changes->width = ev->width;
    changes->height = ev->height;
    changes->border_width = ev->border_width;
    changes->sibling = ev->above;
    changes->stack_mode = ev->detail;

            /* Set border width */
    XConfigureWindow(dpy, ev->window, ev->value_mask, changes);
}

void
HandleKeyPress(XEvent *e, KeyCode key)
{
	if ((e->xkey.keycode == key && (e->xkey.state & Mod1Mask)) && fork() == 0) {
		execlp("st", "", NULL);
	//	exit(0);
	}
}

void
Cases(XEvent *e, KeyCode *enter)
{
    switch (e->type) {

		default: break;
        case DestroyNotify: { XDestroyWindow(dpy, e->xdestroywindow.window); break; }

        case KeyPress: {
			HandleKeyPress(e, *enter);
	  	}

        case MapRequest: {
            ConfigureWindowRequest(e);
            XMapWindow(dpy, e->xmaprequest.window);
            break;
		}
    }
}

int
main(int argc, char *argv[])
{
	InitialChecks(argc, *argv);
	Run();


	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "structs.h"

#define SCREEN_WIDTH    XDisplayWidth(dpy, DefaultScreen(dpy))
#define SCREEN_HEIGHT   XDisplayHeight(dpy, DefaultScreen(dpy))

/* Variables */
Display *dpy;

int GAPS_SIZE = 15;
XColor BORDER_COLOUR;
int BORDER_THICKNESS = 3;
int TITLEBAR_HEIGHT = 5;
int openWindows = 0;

unsigned int NumLockMask = 0;


/* Function Declarations */
// Main Loop
void Cases(XEvent *);
void Configure(void);
void Run(void);
void InitialChecks(int, char *);
void GrabKeys(void);

// Requests
void ConfigureWindowRequest(XEvent *);
void HandleConfigureRequest(XWindowChanges *, XConfigureRequestEvent *);

// Misc
void Error(const char *, int, int);

/* Functions */

void
InitialChecks(int argc, char *argv)
{
    /* Initial checks to make sure the program can run */
    if (!(dpy = XOpenDisplay(NULL)))
        Error("Can't open the display\n", EXIT_FAILURE, true);
}

void
Error(const char *ErrorMessage, int ERROR_CODE, int EXIT)
{
    fprintf(stderr, "%s", ErrorMessage);
    if (EXIT)
        exit(ERROR_CODE);
}

void
Configure(void)
{
    /* Configures the custom settings set by the user */
}

void
Run(void)
{
    /* Main loop of the program */
    Window root = DefaultRootWindow(dpy);

	GrabKeys();

    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

    for (;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        Cases(&e);
    }
}

void
ConfigureWindowRequest(XEvent *e)
{
	/* Configures the window before it is mapped */
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

    BORDER_COLOUR.red = 255 * 256;
    BORDER_COLOUR.green = 0;
    BORDER_COLOUR.blue = 0;
    BORDER_COLOUR.flags = DoRed | DoGreen | DoBlue;

    Colormap colormap = DefaultColormap(dpy, DefaultScreen(dpy));
    if (XAllocColor(dpy, colormap, &BORDER_COLOUR)) {
        XSetWindowBorder(dpy, e->xmaprequest.window, BORDER_COLOUR.pixel);
    } else {
        fprintf(stderr, "Error: Failed to allocate color for window border\n");
    }

    HandleConfigureRequest(&changes, configRequest);
}

void
HandleConfigureRequest(XWindowChanges *changes, XConfigureRequestEvent *ev)
{
	/* Sets the changes */
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
SpawnWindow(const Arg *arg)
{
	if (!fork()) {
		setsid();
		execvp(((char **)arg->cmd)[0], (char **) arg->cmd);
		fprintf(stderr, "execvp %s err", ((char **) arg->cmd)[0]);
		exit(EXIT_SUCCESS);
	}
}

/*
void
CloseWindow(const Arg *arg)
{
    Window focused;
    int revert;

    XGetInputFocus(dpy, &focused, &revert);
    if (focused != None && focused != PointerRoot && focused != DefaultRootWindow(dpy)) {
        XDestroyWindow(dpy, focused);
    }
}
*/

void
GrabKeys(void)
{
    XUngrabKey(dpy, AnyKey, AnyModifier, DefaultRootWindow(dpy));

    for (unsigned int i = 0; i < LENGTH(keys); ++i) {
        KeyCode code = XKeysymToKeycode(dpy, keys[i].key);
        XGrabKey(dpy, code, keys[i].mod, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);

		// Grabbing numcock
        if (NumLockMask) {
            XGrabKey(dpy, code, keys[i].mod | NumLockMask, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
        }
    }
}

unsigned int
CleanMask(unsigned int mask)
{
    return mask & ~(NumLockMask | LockMask);
}


void
HandleKeyPress(XEvent *e)
{
	/*  Handles the key presses */
    KeySym keysym = XLookupKeysym(&e->xkey, 0);
    unsigned int mod = CleanMask(e->xkey.state);

    for (int i = 0; i < LENGTH(keys); ++i) {
        if (keysym == keys[i].key && mod == CleanMask(keys[i].mod)) {
            if (keys[i].fn) {
                keys[i].fn(&(keys[i].arg));
            }
            break;
        }
    }
}

void
Cases(XEvent *e)
{
	/* Switch case for the events, notifications ect */
    switch (e->type) {

        case KeyPress:
            HandleKeyPress(e);
            break;

        case MapRequest:
            ConfigureWindowRequest(e);
            XMapWindow(dpy, e->xmaprequest.window);
            break;

        default:
            break;
    }
}

void
Setup(void)
{
	/* Main setup which gets the numlock keycode through bitwise operations */

    XModifierKeymap *modmap = XGetModifierMapping(dpy);
    KeyCode numlock_keycode = XKeysymToKeycode(dpy, XK_Num_Lock);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < modmap->max_keypermod; ++j) {
            if (modmap->modifiermap[i * modmap->max_keypermod + j] == numlock_keycode) {
                NumLockMask = (1 << i);
                break;
            }
        }
    }

    XFreeModifiermap(modmap);
}

int
main(int argc, char *argv[])
{
    InitialChecks(argc, *argv);
	Setup();

    Run();

    XCloseDisplay(dpy);
    return EXIT_SUCCESS;
}



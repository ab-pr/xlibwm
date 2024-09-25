#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
<<<<<<< HEAD
#include <math.h>
#include <time.h>
=======
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
#include <unistd.h>
#include <math.h>

#include "structs.h"

/* Constants */
#define SCREEN_WIDTH    XDisplayWidth(dpy, DefaultScreen(dpy))
#define SCREEN_HEIGHT   XDisplayHeight(dpy, DefaultScreen(dpy))

/* Data Structures */

/* Variables */
Display *dpy;
int running = 1;

XColor BORDER_COLOUR;
unsigned short GAPS_SIZE = 15;
unsigned short BORDER_THICKNESS = 3;
unsigned short TITLEBAR_HEIGHT = 5;
unsigned short openWindows = 0;

Client *clients = NULL;

unsigned int NumLockMask = 0;

<<<<<<< HEAD
bool running = 0;

/* Function Declarations */
// Main Loop
void Cases(XEvent *);
void Parse(void);
void Run(void);
=======
/* Function Declarations */
// Main Loop
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
void InitialChecks(int, char *);
void Setup(void);
void Run(void);
void Cases(XEvent *);
void GrabKeys(void);
unsigned int CleanMask(unsigned int mask);
void HandleKeyPress(XEvent *);

// Client Management
void AddClient(Window w);
void RemoveClient(Window w);

// Tiling and Window Management
void TileWindows(void);
void ConfigureWindowRequest(XEvent *);
void HandleConfigureRequest(XWindowChanges *, XConfigureRequestEvent *);

// Keybindings
void SpawnWindow(const Arg *);
void CloseWindowUnderPointer(const Arg *);
void Quit(const Arg *);

// Utility Functions
void Error(const char *, int, int);
Window GetWindowUnderPointer(void);

/* Functions */

void Quit(const Arg *arg) {
    running = 0;
}

void
AddClient(Window w) {
    Client *c = malloc(sizeof(Client));
    c->window = w;
    c->next = clients;
    clients = c;
    openWindows++;
}

void
RemoveClient(Window w) {
    Client **curr = &clients; // Current window
    while (*curr) {
        if ((*curr)->window == w) {
            Client *tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            openWindows--;
            break;
        }
        curr = &(*curr)->next;
    }
}

void
TileWindows() {
    if (openWindows == 0) return;

    int cols = (int) sqrt((double) openWindows);
    int rows = (openWindows + cols - 1) / cols;
    int winWidth = SCREEN_WIDTH / cols;
    int winHeight = SCREEN_HEIGHT / rows;

        BORDER_COLOUR.red   = 255 * 256;
        BORDER_COLOUR.green = 000 * 256;
        BORDER_COLOUR.blue  = 000 * 256;
        BORDER_COLOUR.flags = DoRed | DoGreen | DoBlue;

        Colormap colormap = DefaultColormap(dpy, DefaultScreen(dpy));
		if (!XAllocColor(dpy, colormap, &BORDER_COLOUR))
			fprintf(stderr, "Error: Failed to allocate color for window border\n");

		else
			XSetWindowBorder(dpy, clients->window, BORDER_COLOUR.pixel);

    int i = 0;
    Client *c = clients;
    while (c) {
        int x = (i % cols) * winWidth + GAPS_SIZE;
        int y = (i / cols) * winHeight + GAPS_SIZE + TITLEBAR_HEIGHT;

        XWindowChanges changes;
        changes.x = x;
        changes.y = y;
        changes.width = winWidth - (GAPS_SIZE * 2) - (BORDER_THICKNESS * 2);
        changes.height = winHeight - (GAPS_SIZE * 2) - (BORDER_THICKNESS * 2) - TITLEBAR_HEIGHT;
        changes.border_width = BORDER_THICKNESS;

        XConfigureWindow(dpy, c->window, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &changes);
        i++;
        c = c->next;
    }
}


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
<<<<<<< HEAD
Parse(void)
=======
Setup(void)
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
{
    /* Main setup which gets the NumLock keycode through bitwise operations */
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

    /* Initialize BORDER_COLOUR */
    BORDER_COLOUR.red = 255 * 256;    // Red color
    BORDER_COLOUR.green = 0;
    BORDER_COLOUR.blue = 0;
    BORDER_COLOUR.flags = DoRed | DoGreen | DoBlue;

    Colormap colormap = DefaultColormap(dpy, DefaultScreen(dpy));
    if (!XAllocColor(dpy, colormap, &BORDER_COLOUR)) {
        fprintf(stderr, "Error: Failed to allocate color for window border\n");
    }
}

void
Run(void)
{
    /* Main loop of the program */
    Window root = DefaultRootWindow(dpy);

<<<<<<< HEAD
	GrabKeys();
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

	running = true;
=======
    GrabKeys();

    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask | DestroyNotify);

>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
    while (running) {
        XEvent e;
        XNextEvent(dpy, &e);
        Cases(&e);
    }
}

void
Cases(XEvent *e)
{
    /* Switch case for the events, notifications, etc. */
    switch (e->type) {
        case KeyPress:
            HandleKeyPress(e);
            break;

        case MapRequest:
            ConfigureWindowRequest(e);
            XMapWindow(dpy, e->xmaprequest.window);
            break;

        case DestroyNotify:
            RemoveClient(e->xdestroywindow.window);
            TileWindows();
            break;

        default:
            break;
    }
}

void
GrabKeys(void)
{
    XUngrabKey(dpy, AnyKey, AnyModifier, DefaultRootWindow(dpy));

    for (unsigned int i = 0; i < LENGTH(keys); ++i) {
        KeyCode code = XKeysymToKeycode(dpy, keys[i].key);
        XGrabKey(dpy, code, keys[i].mod, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);

        // Grabbing NumLock
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
    /* Handles the key presses */
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
AddClient(Window w)
{
    Client *c = malloc(sizeof(Client));
    c->window = w;
    c->next = clients;
    clients = c;
    openWindows++;
}

void
RemoveClient(Window w)
{
    Client **curr = &clients;
    while (*curr) {
        if ((*curr)->window == w) {
            Client *tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            openWindows--;
            break;
        }
        curr = &(*curr)->next;
    }
}

void
TileWindows(void)
{
    if (openWindows == 0) return;

    int cols = (int) sqrt((double) openWindows);
    int rows = (openWindows + cols - 1) / cols;
    int winWidth = SCREEN_WIDTH / cols;
    int winHeight = SCREEN_HEIGHT / rows;

    int i = 0;
    Client *c = clients;
    while (c) {
        int x = (i % cols) * winWidth + GAPS_SIZE;
        int y = (i / cols) * winHeight + GAPS_SIZE + TITLEBAR_HEIGHT;

        XWindowChanges changes;
        changes.x = x;
        changes.y = y;
        changes.width = winWidth - (GAPS_SIZE * 2) - (BORDER_THICKNESS * 2);
        changes.height = winHeight - (GAPS_SIZE * 2) - (BORDER_THICKNESS * 2) - TITLEBAR_HEIGHT;
        changes.border_width = BORDER_THICKNESS;

        XConfigureWindow(dpy, c->window, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &changes);

        // Set the border color for the window
        XSetWindowBorder(dpy, c->window, BORDER_COLOUR.pixel);

        i++;
        c = c->next;
    }
}

void
ConfigureWindowRequest(XEvent *e)
{
<<<<<<< HEAD
    XConfigureRequestEvent *configRequest = &e->xconfigurerequest;

    AddClient(configRequest->window);
    TileWindows();
    XMapWindow(dpy, configRequest->window);

=======
    /* Configures the window before it is mapped */
    XWindowChanges changes;
    XConfigureRequestEvent *configRequest = &e->xconfigurerequest;

    AddClient(configRequest->window);

    TileWindows();

    // Allow the window to configure itself if needed
    HandleConfigureRequest(&changes, configRequest);
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
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
        fprintf(stderr, "execvp %s error\n", ((char **) arg->cmd)[0]);
        exit(EXIT_SUCCESS);
    }
}

<<<<<<< HEAD
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

        case DestroyNotify:
            RemoveClient(e->xdestroywindow.window);
            TileWindows();
            break;

        default:
            break;
    }
}


Window
GetWindowUnderPointer(void) {
    Window rootReturn,
		   childReturn;
=======
Window
GetWindowUnderPointer(void)
{
    Window root_return, child_return;
    int root_x, root_y;
    int win_x, win_y;
    unsigned int mask_return;
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)

    int rootX,
		rootY;

    int winX,
		winY;

    unsigned int maskReturn;

    if (XQueryPointer(dpy, DefaultRootWindow(dpy), &rootReturn, &childReturn,
                      &rootX, &rootY, &winX, &winY, &maskReturn)) {
        if (childReturn != None) {
            return childReturn;
        }
    }
    return None;
}

void
CloseWindowUnderPointer(const Arg *arg)
{
    Window w = GetWindowUnderPointer();
    if (w == None) {
        fprintf(stderr, "No window under pointer to close.\n");
        return;
    }

    Atom *protocols;
    int n;
    Atom WMDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    Atom WMProtocols = XInternAtom(dpy, "WM_PROTOCOLS", False);

    // Check if the window supports WM_DELETE_WINDOW
    if (XGetWMProtocols(dpy, w, &protocols, &n)) {
        bool supports_delete = False;
        for (int i = 0; i < n; ++i) {
            if (protocols[i] == WMDeleteWindow) {
                supports_delete = True;
                break;
            }
        }
        XFree(protocols);

        if (supports_delete) {
            // Send WM_DELETE_WINDOW message
            XEvent ev;
            ev.type = ClientMessage;
            ev.xclient.window = w;
            ev.xclient.message_type = WMProtocols;
            ev.xclient.format = 32;
            ev.xclient.data.l[0] = WMDeleteWindow;
            ev.xclient.data.l[1] = CurrentTime;
            XSendEvent(dpy, w, False, NoEventMask, &ev);
        } else {
            // If WM_DELETE_WINDOW is not supported, forcefully kill the window
            XKillClient(dpy, w);
        }
    } else {
        // If the window doesn't have WM_PROTOCOLS property, forcefully kill it
        XKillClient(dpy, w);
    }

<<<<<<< HEAD
	RemoveClient(w);
    TileWindows();

	XSync(dpy, False);
=======
    // Remove the client and retile windows
    RemoveClient(w);
    TileWindows();
>>>>>>> 258faa0 (Tiling works, you can exit the program safely, border colours work,)
}

void
Quit(const Arg *arg)
{
    running = 0;

    // Free all clients
    Client *c = clients;
    while (c) {
        Client *next = c->next;
        free(c);
        c = next;
    }
    clients = NULL;
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

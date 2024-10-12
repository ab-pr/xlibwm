#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <math.h>
#include <time.h>
#include <unistd.h>

#include "structs.h"

#define SCREEN_WIDTH    XDisplayWidth(dpy, DefaultScreen(dpy))
#define SCREEN_HEIGHT   XDisplayHeight(dpy, DefaultScreen(dpy))

/* Variables */
Display *dpy;

XColor BORDER_COLOUR;
unsigned short GAPS_SIZE = 15;
unsigned short BORDER_THICKNESS = 3;
unsigned short TITLEBAR_HEIGHT = 5;
XColor normalColor;
XColor focusColor;
Colormap colormap;

unsigned short openWindows = 0;
Client *clients = NULL;

unsigned int NumLockMask = 0;
bool running = 0;

/* Function Declarations */

// Initialization Functions
void InitialChecks(int , char *);
void Parse(void);
void Setup(void);

// Main Loop Function
void Run(void);

// Event Handling Functions
void Cases(XEvent *);
void ConfigureWindowRequest(XEvent *);
void HandleKeyPress(XEvent *);
void HandleEnterNotify(XEvent *);
void HandleLeaveNotify(XEvent *);
void HandleConfigureRequest(XWindowChanges *, XConfigureRequestEvent *);

// Key Handling Functions
void GrabKeys(void);
unsigned int CleanMask(unsigned int);

// Window Management Functions
void TileWindows(void);
void CloseWindowUnderPointer(const Arg *);
void SpawnWindow(const Arg *);
void Quit(const Arg *);

// Client Management Functions
void AddClient(Window);
void RemoveClient(Window);
Client* FindClient(Window);

// Utility Functions
Window GetWindowUnderPointer(void);

// Error Handling Function
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

    // Initialize the colormap
    colormap = DefaultColormap(dpy, DefaultScreen(dpy));

    // Initialize normal border color (red)
    normalColor.red   = 65535;
    normalColor.green = 0;
    normalColor.blue  = 0;
    normalColor.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(dpy, colormap, &normalColor)) {
        fprintf(stderr, "Error: Failed to allocate normal border color\n");
    }

    // Initialize focus border color (green)
    focusColor.red   = 0;
    focusColor.green = 65535;
    focusColor.blue  = 0;
    focusColor.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(dpy, colormap, &focusColor)) {
        fprintf(stderr, "Error: Failed to allocate focus border color\n");
    }
}

void
Parse(void)
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

	running = true;
    while (running) {
        XEvent e;
        XNextEvent(dpy, &e);
        Cases(&e);
    }
}

void
Cases(XEvent *e)
{
    switch (e->type) {

        case KeyPress:
            HandleKeyPress(e);
            break;

        case MapRequest:
            ConfigureWindowRequest(e);
            break;

        case EnterNotify:
            HandleEnterNotify(e);
            break;

        case LeaveNotify:
            HandleLeaveNotify(e);
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
ConfigureWindowRequest(XEvent *e)
{
    XConfigureRequestEvent *configRequest = &e->xconfigurerequest;

    AddClient(configRequest->window);
    TileWindows();
    XMapWindow(dpy, configRequest->window);
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
HandleEnterNotify(XEvent *e)
{
    XEnterWindowEvent *ev = &e->xcrossing;
    Window w = ev->window;

    // Check if the window is still in the client list
    if (!FindClient(w)) {
        // The window is no longer valid; ignore the event
        return;
    }

    // Set the border color to indicate focus
    XSetWindowBorder(dpy, w, focusColor.pixel);

    // Optionally, set input focus to the window
    XSetInputFocus(dpy, w, RevertToPointerRoot, CurrentTime);
}

void
HandleLeaveNotify(XEvent *e)
{
    XLeaveWindowEvent *ev = &e->xcrossing;
    Window w = ev->window;

    // Check if the window is still in the client list
    if (!FindClient(w)) {
        // The window is no longer valid; ignore the event
        return;
    }

    // Reset the border color to normal
    XSetWindowBorder(dpy, w, normalColor.pixel);
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
TileWindows(void)
{
    if (openWindows == 0) return;

    Client *c;
    int i = 0;

    int masterWidth;
    int stackWidth;
    int masterHeight;
    int stackWinHeight;
    int stackWindows = openWindows - 1; // Number of windows in the stack

    int totalGapsWidth = GAPS_SIZE * 3; // Left, middle, and right gaps
    int totalBordersWidth = BORDER_THICKNESS * 4; // Borders on both sides of master and stack areas

    int availableHeight = SCREEN_HEIGHT - TITLEBAR_HEIGHT - (GAPS_SIZE * 2);

    if (openWindows == 1) {
        // Only one window, it takes the full screen minus gaps and borders
        masterWidth = SCREEN_WIDTH - (GAPS_SIZE * 2) - (BORDER_THICKNESS * 2);
        masterHeight = availableHeight - (BORDER_THICKNESS * 2);
    } else {
        // Master and stack areas split the screen width equally
        masterWidth = (SCREEN_WIDTH - totalGapsWidth - totalBordersWidth) / 2;
        stackWidth = masterWidth;

        masterHeight = availableHeight - (BORDER_THICKNESS * 2);

        // Calculate total gaps and borders height for stack windows
        int totalGapsHeight = GAPS_SIZE * (stackWindows > 0 ? stackWindows - 1 : 0);
        int totalBordersHeight = BORDER_THICKNESS * 2 * stackWindows;
        int totalWindowAreaHeight = availableHeight - totalGapsHeight - totalBordersHeight;

        // Calculate the height of each stack window
        stackWinHeight = stackWindows > 0 ? totalWindowAreaHeight / stackWindows : 0;
    }

    // Positions
    int masterX = GAPS_SIZE + BORDER_THICKNESS;
    int masterY = GAPS_SIZE + TITLEBAR_HEIGHT + BORDER_THICKNESS;
    int stackX = masterX + masterWidth + GAPS_SIZE + (BORDER_THICKNESS * 2);
    int stackY = GAPS_SIZE + TITLEBAR_HEIGHT;

    c = clients;
    while (c) {
        XWindowChanges changes;
        changes.border_width = BORDER_THICKNESS;

        if (i == 0) {
            // Master window
            changes.x = masterX;
            changes.y = masterY;
            changes.width = masterWidth;
            changes.height = masterHeight;
        } else {
            // Stack windows
            changes.x = stackX;
            changes.y = stackY + BORDER_THICKNESS; // Adjust for border

            changes.width = stackWidth;
            changes.height = stackWinHeight;

            // Adjust the last window to fill any remaining space
            if (i == openWindows - 1) {
                int usedHeight = stackY - (GAPS_SIZE + TITLEBAR_HEIGHT) + stackWinHeight + (BORDER_THICKNESS * 2);
                int remainingHeight = availableHeight - usedHeight;
                changes.height += remainingHeight;
            }

            // Update y position for next window
            stackY += stackWinHeight + (BORDER_THICKNESS * 2) + GAPS_SIZE;
        }

        // Set window border color to normal color
        XSetWindowBorder(dpy, c->window, normalColor.pixel);

        // Apply window changes
        XConfigureWindow(dpy, c->window, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &changes);

        i++;
        c = c->next;
    }
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

	RemoveClient(w);
    TileWindows();

	XSync(dpy, False);
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

void
Quit(const Arg *arg)
{
    running = 0;
}

void
AddClient(Window w)
{
    Client *c = malloc(sizeof(Client));
    if (!c) {
        fprintf(stderr, "Error: Could not allocate memory for client\n");
        return;
    }
    c->window = w;
    c->next = clients;
    clients = c;
    openWindows++;

    // Select Enter and Leave events for the window
    XSelectInput(dpy, w, EnterWindowMask | LeaveWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask);

    // Bring the new window to the front
    XRaiseWindow(dpy, w);
}

void
RemoveClient(Window w)
{
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

Client *
FindClient(Window w)
{
    Client *c = clients;
    while (c) {
        if (c->window == w) {
            return c;
        }
        c = c->next;
    }
    return NULL;
}

Window
GetWindowUnderPointer(void)
{
    Window rootReturn,
		   childReturn;

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
Error(const char *ErrorMessage, int ERROR_CODE, int EXIT)
{
    fprintf(stderr, "%s", ErrorMessage);
    if (EXIT)
        exit(ERROR_CODE);
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

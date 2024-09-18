#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

Display *dpy;
Window root;
Window *windows = NULL;
int window_count = 0;

void tile_windows()
{
    int screen = DefaultScreen(dpy);
    int width = DisplayWidth(dpy, screen);
    int height = DisplayHeight(dpy, screen);
    int i;

    for(i = 0; i < window_count; i++)
    {
        XMoveResizeWindow(dpy, windows[i],
                          0, i * height / window_count,
                          width, height / window_count);
    }
}

void grab_keys()
{
    XGrabKey(dpy, XKeysymToKeycode(dpy, XK_Return),
             Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
}

int main()
{
    XEvent ev;

    if(!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, "ERROR: Could not open display\n");
        exit(1);
    }

    root = DefaultRootWindow(dpy);

    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | KeyPressMask);

    grab_keys();

    /* Manage existing windows */
    Window returned_root, returned_parent;
    Window *top_level_windows;
    unsigned int num_top_level_windows;
    if (XQueryTree(dpy, root, &returned_root, &returned_parent,
                   &top_level_windows, &num_top_level_windows))
    {
        windows = malloc(sizeof(Window) * num_top_level_windows);
        for(unsigned int i = 0; i < num_top_level_windows; i++)
        {
            windows[window_count++] = top_level_windows[i];
            XSelectInput(dpy, top_level_windows[i], StructureNotifyMask);
        }
        XFree(top_level_windows);
    }

    tile_windows();

    /* Main event loop */
    for(;;)
    {
        XNextEvent(dpy, &ev);

        if(ev.type == MapRequest)
        {
            XMapWindow(dpy, ev.xmaprequest.window);
            windows = realloc(windows, sizeof(Window) * (window_count + 1));
            windows[window_count++] = ev.xmaprequest.window;
            XSelectInput(dpy, ev.xmaprequest.window, StructureNotifyMask);
            tile_windows();
        }
        else if(ev.type == DestroyNotify)
        {
            /* Remove the window from our list */
            for(int i = 0; i < window_count; i++)
            {
                if(windows[i] == ev.xdestroywindow.window)
                {
                    for(int j = i; j < window_count - 1; j++)
                    {
                        windows[j] = windows[j+1];
                    }
                    window_count--;
                    windows = realloc(windows, sizeof(Window) * window_count);
                    tile_windows();
                    break;
                }
            }
        }
        else if(ev.type == KeyPress)
        {
            /* Check if Mod1 + Enter was pressed */
            if(ev.xkey.keycode == XKeysymToKeycode(dpy, XK_Return) &&
               (ev.xkey.state & Mod1Mask))
            {
                if(fork() == 0)
                {
                    execlp("st", "st", (char *)NULL);
                    exit(0);
                }
            }
        }
    }

    /* Clean up (unreachable in this code) */
    XCloseDisplay(dpy);
    free(windows);

    return 0;
}


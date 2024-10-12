#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define MOD Mod1Mask
#define LENGTH(X) (sizeof X / sizeof X[0])

typedef struct {
    const char **cmd;
    int ops;
} Arg;

typedef struct {
    int mod;
    KeySym key;
    void (*fn) (const Arg *);
    Arg arg;
} Key;

typedef struct Client {
    Window window;
    struct Client *next;
} Client;

extern void SpawnWindow(const Arg *arg);
extern void CloseWindowUnderPointer(const Arg *arg);

static const char *terminal[] = {"st", NULL};
static const char *discord[] = {"discord", NULL};

static const Key keys[] = {
    {MOD, XK_Return, SpawnWindow, {.cmd = terminal}},
    {MOD, XK_e, SpawnWindow, {.cmd = discord}},
    {MOD | ShiftMask, XK_q, CloseWindowUnderPointer, {NULL}},
};

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define MOD Mod4Mask
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

extern void SpawnWindow(const Arg *arg);
// extern void CloseWindow(const Arg *arg);

static const char *terminal[] = {"st", NULL};
static const char *discord[] = {"discord", NULL};

static const Key keys[] = {
	{ MOD, XK_Return, SpawnWindow, {.cmd = terminal}},
	{ MOD, XK_e, SpawnWindow, {.cmd = discord}},
//	{ MOD | ShiftMask, XK_Q, CloseWindow, {NULL}},

};

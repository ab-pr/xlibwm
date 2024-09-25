#include <X11/Xutil.h>

#define MOD Mod1Mask
#define LENGTH(X) (sizeof X / sizeof X[0])

typedef union {
    const char **cmd;
} Arg;

typedef struct {
    unsigned int mod;
    KeySym key;
    void (*fn)(const Arg *);
    const Arg arg;
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
    {MOD | ShiftMask, XK_c, CloseWindowUnderPointer, {NULL}},
};


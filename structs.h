typedef struct _CustomCommand {
	char *name;
	char *args;
} CustomCommand;

typedef struct _CustomBind {
	bool mod;
	bool shiftmask;
	int	 key;
	CustomCommand spawn;
} CustomBind;

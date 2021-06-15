#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *dpy;

/* show usage */
static void
usage(void)
{
	fprintf(stderr, "usage: xclimsg [-mp] [-t window] [-w window] type data...\n");
	exit(1);
}

/* get data from string */
static long
getdata(char *s)
{
	long n;
	char *endp;

	if (strcmp(s, "None") == 0)
		return None;
	if (strcmp(s, "CurrentTime") == 0)
		return CurrentTime;
	n = strtoul(s, &endp, 0);
	if (endp == s || *endp != '\0')
		return (long)XInternAtom(dpy, s, False);
	return n;
}

/* xclimsg: send xclient message */
int
main(int argc, char *argv[])
{
	XEvent ev;
	Window towin = None;
	Window win = None;
	Bool propagate = False;
	long mask = NoEventMask;
	int i, ch;

	while ((ch = getopt(argc, argv, "mpt:w:")) != -1) {
		switch (ch) {
		case 'm':
			mask = SubstructureRedirectMask | SubstructureNotifyMask;
			break;
		case 'p':
			propagate = True;
			break;
		case 't':
			towin = strtoul(optarg, NULL, 0);
			break;
		case 'w':
			win = strtoul(optarg, NULL, 0);
			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc < 2 || argc > 6)
		usage();
	if ((dpy = XOpenDisplay(NULL)) == NULL)
		errx(1, "could not open display");
	if (win == None)
		 win = DefaultRootWindow(dpy);
	if (towin == None)
		 towin = DefaultRootWindow(dpy);
	ev.xclient.type = ClientMessage;
	ev.xclient.format = 32;
	ev.xclient.send_event = True;
	ev.xclient.serial = 0;
	ev.xclient.window = win;
	ev.xclient.message_type = XInternAtom(dpy, *(argv++), False);
	for (i = 0; i < argc - 1; i++)
		ev.xclient.data.l[i] = getdata(argv[i]);
	XSendEvent(dpy, towin, propagate, mask, &ev);
	XFlush(dpy);
	XCloseDisplay(dpy);
	return 0;
}

#include <u.h>
#include <libc.h>

#define	TEMP	"/n/temp"

void
usage(void)
{
	fprint(2, "usage: pipefile [-r command] [-w command] file\n");
	exits("usage");
}

void
connect(char *cmd, int fd0, int fd1)
{
	switch(rfork(RFPROC|RFFDG|RFREND|RFNOWAIT)){
	case -1:
		sysfatal("fork %s: %r", cmd);
		break;
	default:
		return;
	case 0:
		if(fd0 != 0)
			dup(fd0, 0);
		if(fd1 != 1)
			dup(fd1, 1);
		execl("/bin/rc", "rc", "-c", cmd, nil);
		sysfatal("exec %s: %r", cmd);
		break;
	}
}

void
main(int argc, char *argv[])
{
	char *file;
	char *rcmd, *wcmd;
	int fd0, fd1, ifd0, ifd1;

	rcmd = wcmd = nil;
	ARGBEGIN{
	case 'r':
		rcmd = EARGF(usage());
		break;
	case 'w':
		wcmd = EARGF(usage());
		break;
	default:
		usage();
	}ARGEND

	if(argc!=1 || (rcmd==nil && wcmd==nil))
		usage();
	if(rcmd == nil)
		rcmd = "/bin/cat";
	if(wcmd == nil)
		wcmd = "/bin/cat";

	file = argv[0];
	ifd0 = open(file, OREAD);
	if(ifd0 < 0)
		sysfatal("open %s: %r", file);
	ifd1 = open(file, OWRITE);
	if(ifd1 < 0)
		sysfatal("open %s: %r", file);

	if(bind("#|", TEMP, MREPL) < 0)
		sysfatal("bind pipe %s: %r", TEMP);
	if(bind(TEMP "/data", file, MREPL) < 0)
		sysfatal("bind %s %s: %r", TEMP "/data", file);

	fd0 = open(TEMP "/data1", OREAD);
	if(fd0 < 0)
		sysfatal("open %s: %r", TEMP "/data1");
	connect(wcmd, fd0, ifd1);
	fd1 = open(TEMP "/data1", OWRITE);
	if(fd1 < 0)
		sysfatal("open %s: %r", TEMP "/data1");
	connect(rcmd, ifd0, fd1);
	unmount(nil, TEMP);
	exits(nil);
}



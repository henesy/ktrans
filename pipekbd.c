#include <u.h>
#include <libc.h>
#include <bio.h>

void
main(int argc, char *argv[])
{
	int n, kinfd, koutfd, fd[2];
	char buf[128];
	int kbd;

	kbd = 1;
	if((kinfd = open("/dev/kbd", OREAD)) < 0){
		kbd = 0;
		if((kinfd = open("/dev/cons", OREAD)) < 0)
			sysfatal("open kbd: %r");
	}
	if(bind("#|", "/n/temp", MREPL) < 0)
		sysfatal("bind /n/temp: %r");
	if((koutfd = open("/n/temp/data1", OWRITE)) < 0)
		sysfatal("open kbd pipe: %r");
	if(bind("/n/temp/data", kbd? "/dev/kbd": "/dev/cons", MREPL) < 0)
		sysfatal("bind kbd pipe: %r");
	unmount(nil, "/n/temp");
	if(fork())
		exits(nil);	/* parent exits */
	if(!kbd){
		dup(kinfd, 0);
		dup(koutfd, 1);
	Run:
		close(kinfd);
		close(koutfd);
		if(argv[1][0] != '/' || strncmp(argv[1], "./", 2) != 0 || strncmp(argv[1], "../", 3) != 0)
			argv[1] = smprint("/bin/%s", argv[1]);
		exec(argv[1], argv+1);
		sysfatal("exec: %r");
	}
	if(pipe(fd) < 0)
		sysfatal("pipe: %r");
	if(fork()){
		dup(fd[0], 0);
		dup(fd[0], 1);
		close(fd[0]);
		close(fd[1]);
		goto Run;
	}
	close(fd[0]);
	if(fork()){
		Biobuf b;
		long r;

		Binit(&b, fd[1], OREAD);
		while((r = Bgetrune(&b)) >= 0){
			n = snprint(buf, sizeof(buf), "c%C", (Rune)r)+1;
			write(koutfd, buf, n);	/* pass on result */
		}
	} else {
		while((n = read(kinfd, buf, sizeof(buf))) > 0){
			buf[n-1] = 0;
			if(n < 2 || buf[0] != 'c')
				write(koutfd, buf, n);		/* pass on */
			else
				write(fd[1], buf+1, n-2);	/* to translator */
		}
	}
	exits(nil);
}

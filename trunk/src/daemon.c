#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>

#include <dprint.h>
#include <errno.h>

extern int errno;

/* true if BSD system */
#ifdef	SIGTSTP
#include <sys/file.h>
#include <sys/ioctl.h>
#define _USE_BSD
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

void ss_chld(int x) {
  /* Use the wait3() system call with the WNOHANG option. */
  union wait status;
  while(wait3(&status, WNOHANG, NULL) > 0);
}
#endif /* SIGTSTP */

/* Detach a daemon process from login session context.
 * nonzero -> handle SIGCLDs so zombies don't clog */
void daemon_start(int ignsigcld) {
  register int childpid, fd;
  DBG(">Daemon> daemon_start pid=%d\n",getppid());
  if(getppid() == 1)
    goto out;
#ifdef SIGTTOU
  signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
  signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
  signal(SIGTSTP, SIG_IGN);
#endif
  if((childpid = fork()) < 0) {
    LOG(L_ERR, ">Daemon> can't fork first child\n");
  } else if(childpid > 0) {
    DBG(">Daemon> exit 0-parent\n");
    exit(0);	/* parent */
  }
#ifdef	SIGTSTP
  /* BSD */
  if(setpgid(0, getpid()) == -1)
    LOG(L_ERR, ">Daemon> can't change process group\n");
  DBG(">Daemon> process group changed\n");
  if((fd = open("/dev/tty", O_RDWR)) >= 0) {
    ioctl(fd, TIOCNOTTY, (char *)NULL); /* lose controlling tty */
    DBG(">Daemon> lose controlling tty\n");
    close(fd);
  }
#else /* SIGTSTP */
  /* System V */
  if(setpgrp() == -1)
    LOG(L_ERR, ">Daemon> can't change process group\n");
  signal(SIGHUP, SIG_IGN);	/* immune from pgrp leader death */
  if((childpid = fork()) < 0) {
    LOG(L_ERR, ">Daemon> can't fork second child\n");
  } else if(childpid > 0) {
    DBG(">Daemon> exit 1-parent\n");
    exit(0);	/* first child */
  }
#endif /* SIGTSTP */
out:
  for (fd=0; fd<NOFILE; ++fd) {
    close(fd);
  }
  errno = 0;  /* probably got set to EBADF from a close */
  chdir("/tmp");
  umask(0);
  if(ignsigcld) {
#ifdef	SIGTSTP
    signal(SIGCLD, ss_chld);  /* BSD */
#else
    signal(SIGCLD, SIG_IGN);  /* System V */
#endif
  }
}

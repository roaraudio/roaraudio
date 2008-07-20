//lib.c:

#include "roard.h"

int lib_run_bg(char * cmd, int infh, int outfh, int errfh) {
 pid_t child = fork();
 int fh[3] = {-1, -1, -1};
 int i;

 if ( child == -1 ) {
  ROAR_ERR("lib_run_bg(*): Can not fork: %s", strerror(errno));
  return -1;
 }

 if ( child ) {
  // we are the parent!
  return child;
 }

 // we are the child.
 // first we need to close a lot of open files!

 // before we do this we need to keep backups of our handles:
 fh[0] = dup(infh);
 fh[1] = dup(outfh);
 fh[2] = dup(errfh);

 // TODO: test for errors here.

 close(g_listen_socket); // listen socket.

 clients_free(); // delete all clients!, this allso delets all streams

 // TODO: what aout the output driver?

 // next we need to remap our stdio:
 // stdio: fh 0..2

 for (i = 0; i < 3; i++) {
  close(i);
  dup2(fh[i], i); // todo test if this is ok.
 }

 // OK, now we should have set up all our fh properbly, exec our command:

 execl("/bin/sh", "sh", "-c", cmd, NULL);

 // still alive? BAD!
 ROAR_ERR("lib_run_bg(*): We are still alive! BAD!");
 _exit(3);
 return -1;
}

//ll

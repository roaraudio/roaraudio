//signals.c:

#include "roard.h"

void on_sig_int (int signum) {
 ROAR_DBG("got SIGINT");

 alive = 0;
 clean_quit();
}

void on_sig_chld (int signum) {
 ROAR_DBG("got SIGCHLD");

 wait(NULL);

 signal(SIGCHLD, on_sig_chld);
}


//ll

//signals.c:

#include "roard.h"

void on_sig_int (int signum) {
 ROAR_DBG("got SIGINT");

 alive = 0;
 clean_quit();
}

//ll

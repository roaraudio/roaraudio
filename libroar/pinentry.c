//pinentry.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_pinentry_open (struct roar_pinentry * pe, int flags, char * display, char * tty, char * term) {
 int in[2], out[2];

 if ( pe == NULL )
  return -1;

 memset(pe, 0, sizeof(struct roar_pinentry));
 pe->in  = -1;
 pe->out = -1;

#ifdef ROAR_HAVE_BIN_PINENTRY
 if ( display == NULL )
  display = getenv("DISPLAY");

 if ( display == NULL )
  display = "(NONE)";

 if ( tty == NULL )
  tty = ttyname(ROAR_STDIN);

 if ( tty == NULL )
  tty = "/dev/tty"; // TODO: make compile time config parameter out of this

 if ( term == NULL )
  term = getenv("TERM");

 if ( term == NULL )
  term = "dumb";

 // open some pipes...
 if ( pipe(in) != 0 )
  return -1;

 if ( pipe(out) != 0 ) {
  close(in[0]);
  close(in[1]);
  return -1;
 }

 pe->pid = fork();

 switch (pe->pid) {
  case -1:
    close(in[0]);
    close(in[1]);
    close(out[0]);
    close(out[1]);
    return -1;
   break;
  case 0:
    close(in[0]);
    close(out[1]);
    close(ROAR_STDIN);
    close(ROAR_STDOUT);

    if ( dup2(out[0], ROAR_STDIN) == -1 )
     _exit(1);

    if ( dup2(in[1], ROAR_STDOUT) == -1 )
     _exit(1);

    execlp(ROAR_HAVE_BIN_PINENTRY, "--display", display, "--ttytype", term, "--ttyname", tty, NULL);

    _exit(1);
   break;
 }

 close(in[1]);
 close(out[0]);

 pe->in  = in[0];
 pe->out = out[1];

 pe->fin = fdopen(in[0], "r");

 return 0;
#else
 return -1;
#endif
}

int roar_pinentry_simple_open(struct roar_pinentry * pe) {
 return roar_pinentry_open(pe, 0, NULL, NULL, NULL);
}

int roar_pinentry_close(struct roar_pinentry * pe) {
 if ( pe == NULL )
  return -1;

 if ( pe->opened == 0 )
  return 0;

 if ( pe->out != -1 )
  close(pe->out);

 if ( pe->fin != NULL )
  fclose(pe->fin);

 if ( pe->in  != -1 )
  close(pe->in);

 memset(pe, 0, sizeof(struct roar_pinentry));

 return 0;
}

int roar_pinentry_send (struct roar_pinentry * pe, char * cmd,  char * args) {
 size_t len;

 if ( pe == NULL )
  return -1;

 if ( cmd == NULL )
  return -1;

 len = strlen(cmd);

 if ( write(pe->out, cmd, len) != len )
  return -1;

 if ( args != NULL ) {
  len = strlen(args);

  if ( write(pe->out, args, len) != len )
   return -1;
 }

 if ( write(pe->out, "\n", 1) != 1 )
  return -1;

 return 0;
}


#define MAX_LINE_SIZE 2048
int roar_pinentry_recv (struct roar_pinentry * pe, char ** line, char ** opts) {
 char realbuf[MAX_LINE_SIZE];
 char * tp;

 if ( pe == NULL )
  return -1;

 if ( pe->fin == NULL )
  return -1;

 if ( fgets(realbuf, MAX_LINE_SIZE, pe->fin) == NULL )
  return -1;

 tp = realbuf + strlen(realbuf) - 1;

 for (; *tp != '\r' && *tp != '\n'; tp--)
  *tp = 0;

 if ( (tp = strstr(realbuf, " ")) == NULL ) {
  if ( line != NULL )
   *line = strdup(realbuf);

  if ( opts != NULL )
   *opts = NULL;

  if ( !strcmp(realbuf, "OK") ) {
   return 0;
  } else if ( !strcmp(realbuf, "ERR") ) {
   return 1;
  } else {
   return -1;
  }
 } else {
  *tp = 0;

  if ( !strcmp(realbuf, "D") ) {
   if ( opts != NULL )
    *opts = strdup(tp+1);

   return roar_pinentry_recv(pe, line, NULL);
  }

  if ( line != NULL )
   *line = strdup(realbuf);

  if ( opts != NULL )
   *opts = NULL;

  if ( !strcmp(realbuf, "OK") ) {
   return 0;
  } else if ( !strcmp(realbuf, "ERR") ) {
   return 1;
  } else {
   return -1;
  }
 }

 return -1;
}

int roar_pinentry_req  (struct roar_pinentry * pe, char * cmd,  char * args, char ** line, char ** opts) {
 if ( pe == NULL )
  return -1;

 if ( roar_pinentry_send(pe, cmd, args) != 0 )
  return -1;

 return roar_pinentry_recv(pe, line, opts);
}

int roar_pinentry_set_desc (struct roar_pinentry * pe, char * desc) {
 return roar_pinentry_set(pe, "DESC", desc);
}

int roar_pinentry_set_prompt(struct roar_pinentry * pe, char * prompt) {
 return roar_pinentry_set(pe, "PROMPT", prompt);
}

int roar_pinentry_set_yes  (struct roar_pinentry * pe, char * yes) {
 return roar_pinentry_set(pe, "YES", yes);
}

int roar_pinentry_set_no   (struct roar_pinentry * pe, char * no) {
 return roar_pinentry_set(pe, "NO", no);
}

int roar_pinentry_set      (struct roar_pinentry * pe, char * obj, char * text) {
 char req[80] = "SET";

 if ( pe == NULL )
  return -1;

 if ( obj == NULL )
  return -1;

 if ( strlen(obj) > (80-1 /* \0 */ + 3 /* "SET" */ + 1 /* " " */) )
  return -1;

 strncat(req, obj, 80-5);
 strncat(req, " ", 2);

 return roar_pinentry_req(pe, req, text, NULL, NULL);
}

int roar_pinentry_getpin   (struct roar_pinentry * pe, char ** pw, char * desc, char * prompt) {
 if ( pe == NULL )
  return -1;

 if ( pw == NULL )
  return -1;

 if ( desc != NULL )
  if ( roar_pinentry_set_desc(pe, desc) != 0 )
   return -1;

 if ( prompt != NULL )
  if ( roar_pinentry_set_prompt(pe, prompt) != 0 )
   return -1;

 return roar_pinentry_req(pe, "GETPIN", NULL, NULL, pw);
}

int roar_pinentry_confirm  (struct roar_pinentry * pe, char * desc, char * yes, char * no) {
 if ( pe == NULL )
  return -1;

 if ( desc != NULL )
  if ( roar_pinentry_set_desc(pe, desc) != 0 )
   return -1;

 if ( yes != NULL )
  if ( roar_pinentry_set_yes(pe, yes) != 0 )
   return -1;

 if ( no != NULL )
  if ( roar_pinentry_set_no(pe, no) != 0 )
   return -1;

 return roar_pinentry_req(pe, "CONFIRM", NULL, NULL, NULL);
}

//ll

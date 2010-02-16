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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

// TODO: need to check: && defined(ROAR_HAVE_TTYNAME)
#if defined(ROAR_HAVE_FORK) && defined(ROAR_HAVE_PIPE)
#define _CAN_POSIX
#endif

#if defined(ROAR_HAVE_BIN_PINENTRY) && defined(ROAR_SUPPORT_PASSWORD_API) && defined(_CAN_POSIX)
#define _CAN_OPERATE
#endif

int roar_pinentry_open (struct roar_pinentry * pe, int flags, char * display, char * tty, char * term) {
#ifdef _CAN_OPERATE
 int in[2], out[2];

 if ( pe == NULL )
  return -1;

 memset(pe, 0, sizeof(struct roar_pinentry));
 pe->in  = -1;
 pe->out = -1;

 if ( display == NULL )
  display = getenv("DISPLAY");

 if ( display == NULL )
  display = "(NONE)";

 if ( tty == NULL )
  tty = ttyname(ROAR_STDIN);

 if ( tty == NULL )
#ifdef ROAR_DEFAULT_TTY
  tty = ROAR_DEFAULT_TTY;
#else
  return -1;
#endif

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

    execlp(ROAR_HAVE_BIN_PINENTRY, "RoarAudio", "--display", display, "--ttytype", term, "--ttyname", tty, NULL);

    _exit(1);
   break;
 }

 close(in[1]);
 close(out[0]);

 pe->in  = in[0];
 pe->out = out[1];

 pe->fin = fdopen(in[0], "r");

 roar_pinentry_recv(pe, NULL, NULL);

 return 0;
#else
 return -1;
#endif
}

int roar_pinentry_simple_open(struct roar_pinentry * pe) {
 return roar_pinentry_open(pe, 0, NULL, NULL, NULL);
}

int roar_pinentry_close(struct roar_pinentry * pe) {
#ifdef _CAN_OPERATE
 int status;

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

 waitpid(pe->pid, &status, 0);

 memset(pe, 0, sizeof(struct roar_pinentry));

 return 0;
#else
 return -1;
#endif
}

int roar_pinentry_send (struct roar_pinentry * pe, char * cmd,  char * args) {
#ifdef _CAN_OPERATE
 size_t len;

 if ( pe == NULL )
  return -1;

 if ( cmd == NULL )
  return -1;

 len = strlen(cmd);

 if ( write(pe->out, cmd, len) != len )
  return -1;

 if ( args != NULL ) {
  if ( write(pe->out, " ", 1) != 1 )
   return -1;

  len = strlen(args);

  if ( write(pe->out, args, len) != len )
   return -1;
 }

 if ( write(pe->out, "\n", 1) != 1 )
  return -1;

 return 0;
#else
 return -1;
#endif
}


#define MAX_LINE_SIZE 2048
int roar_pinentry_recv (struct roar_pinentry * pe, char ** line, char ** opts) {
#ifdef _CAN_OPERATE
 char realbuf[MAX_LINE_SIZE];
 char * tp;

 if ( pe == NULL )
  return -1;

 if ( pe->fin == NULL )
  return -1;

 if ( fgets(realbuf, MAX_LINE_SIZE, pe->fin) == NULL )
  return -1;

 tp = realbuf + strlen(realbuf) - 1;

 for (; *tp == '\r' || *tp == '\n'; tp--)
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
#else
 return -1;
#endif
}

int roar_pinentry_req  (struct roar_pinentry * pe, char * cmd,  char * args, char ** line, char ** opts) {
#ifdef ROAR_SUPPORT_PASSWORD_API
 if ( pe == NULL )
  return -1;

 if ( roar_pinentry_send(pe, cmd, args) != 0 )
  return -1;

 return roar_pinentry_recv(pe, line, opts);
#else
 return -1;
#endif
}

int roar_pinentry_set_desc (struct roar_pinentry * pe, char * desc) {
 return roar_pinentry_set(pe, "DESC", desc);
}

int roar_pinentry_set_prompt(struct roar_pinentry * pe, char * prompt) {
 return roar_pinentry_set(pe, "PROMPT", prompt);
}

int roar_pinentry_set_yes  (struct roar_pinentry * pe, char * yes) {
 return roar_pinentry_set(pe, "OK", yes);
}

int roar_pinentry_set_no   (struct roar_pinentry * pe, char * no) {
 return roar_pinentry_set(pe, "CANCEL", no);
}

int roar_pinentry_set      (struct roar_pinentry * pe, char * obj, char * text) {
#ifdef ROAR_SUPPORT_PASSWORD_API
 char req[80] = "SET";

 if ( pe == NULL )
  return -1;

 if ( obj == NULL )
  return -1;

 if ( strlen(obj) > (80-(1 /* \0 */ + 3 /* "SET" */)) )
  return -1;

 strncat(req, obj, 80-4);

 return roar_pinentry_req(pe, req, text, NULL, NULL);
#else
 return -1;
#endif
}

int roar_pinentry_getpin   (struct roar_pinentry * pe, char ** pw, char * desc, char * prompt) {
#ifdef ROAR_SUPPORT_PASSWORD_API
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

 if ( roar_pinentry_req(pe, "GETPIN", NULL, NULL, pw) == -1 )
  return -1;

 return 0;
#else
 return -1;
#endif
}

int roar_pinentry_confirm  (struct roar_pinentry * pe, char * desc, char * yes, char * no) {
#ifdef ROAR_SUPPORT_PASSWORD_API
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
#else
 return -1;
#endif
}

//ll

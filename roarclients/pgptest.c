//pgptest.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <roaraudio.h>

#define BUF_MAX 1024
#define FILENAME "/tmp/bla.asc"
#define PASSWORD "test"
#define STRING   "Hello world!\n"

int main (void) {
 char buf[BUF_MAX];
 size_t len;
 struct roar_vio_calls file, pgp;

 if ( roar_vio_open_file(&file, FILENAME, O_CREAT|O_TRUNC|O_WRONLY, 0666) == -1 ) {
  ROAR_ERR("roar_vio_open_file(&file, FILENAME, O_CREAT|O_TRUNC|O_WRONLY, 0666) = -1");
  return 1;
 }

 if ( roar_vio_open_pgp_encrypt_sym(&pgp, &file, PASSWORD, ROAR_VIO_PGP_OPTS_ASCII) == -1 ) {
  ROAR_ERR("roar_vio_open_pgp_encrypt_sym(&pgp, &file, PASSWORD, ROAR_VIO_PGP_OPTS_ASCII) = -1");
  return 1;
 }

 roar_vio_write(&pgp, STRING, strlen(STRING));

 roar_vio_close(&pgp);


 if ( roar_vio_open_file(&file, FILENAME, O_RDONLY, 0666) == -1 ) {
  ROAR_ERR("roar_vio_open_file(&file, FILENAME, O_RDONLY, 0666) = -1");
  return 1;
 }

 if ( roar_vio_open_pgp_decrypt(&pgp, &file, PASSWORD) == -1 ) {
  ROAR_ERR("roar_vio_open_pgp_decrypt(&pgp, &file, PASSWORD) = -1");
  return 1;
 }

 while ((len = roar_vio_read(&pgp, buf, BUF_MAX)))
  write(ROAR_STDOUT, buf, len);

 roar_vio_close(&pgp);

 return 0;
}

//ll

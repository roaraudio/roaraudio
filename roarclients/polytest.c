//polytest.c:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include <roaraudio.h>
#include <libroardsp/libroardsp.h>

#define BUF_MAX 1024

int main (void) {
 int16_t  in[BUF_MAX];
 int16_t out[BUF_MAX];
 size_t il = 10;
 size_t ol = 20;
 int i;
 int tmp;

 for (i = 0; i < il; i++) {
  scanf("%i", &tmp);
  in[i] = tmp;
 }

 roar_conv_poly4_16(out, in, ol, il);

 for (i = 0; i < ol; i++)
  printf("%i\n", out[i]);

 return 0;
}

//ll

//colors.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroarlight.h"

int roar_color_new        (struct roar_color * c) {
 if ( c == NULL )
  return -1;

 memset(c, 0, sizeof(struct roar_color));

 c->system = ROAR_COLORSYSTEM_NONE;

 return 0;
}

int roar_color_new_gray   (struct roar_color * c, unsigned char k) {
 if ( roar_color_new(c) == -1 )
  return -1;

 c->system = ROAR_COLORSYSTEM_GRAY;

 c->color.gray.k = k;

 return 0;
}

int roar_color_new_rgb    (struct roar_color * c, unsigned char r, unsigned char g, unsigned char b) {
 if ( roar_color_new(c) == -1 )
  return -1;

 c->system = ROAR_COLORSYSTEM_RGB;

 c->color.rgb.r = r;
 c->color.rgb.g = g;
 c->color.rgb.b = b;

 return 0;
}

int roar_color_copy       (struct roar_color * dst, struct roar_color * src) {
 if ( dst == NULL || src == NULL )
  return -1;

 memcpy(dst, src, sizeof(struct roar_color));

 return 0;
}

int roar_color_conv       (struct roar_color * c, uint32_t system) {
 if ( c == NULL )
  return -1;

 switch (c->system) {
  case ROAR_COLORSYSTEM_GRAY:
    return roar_color_conv_gray(c, system);
   break;
  case ROAR_COLORSYSTEM_RGB:
    return roar_color_conv_rgb(c, system);
   break;
 }

 return -1;
}

int roar_color_conv_gray  (struct roar_color * c, uint32_t system) {
 unsigned char k;

 if ( c == NULL )
  return -1;

 if ( c->system != ROAR_COLORSYSTEM_GRAY )
  return -1;

 switch (system) {
  case ROAR_COLORSYSTEM_RGB:
    k = c->color.gray.k;
    c->color.rgb.r = k;
    c->color.rgb.g = k;
    c->color.rgb.b = k;
    c->system = ROAR_COLORSYSTEM_RGB;
    return 0;
   break;
 }

 return -1;
}

int roar_color_conv_rgb   (struct roar_color * c, uint32_t system) {
 if ( c == NULL )
  return -1;

 if ( c->system != ROAR_COLORSYSTEM_GRAY )
  return -1;

 switch (system) {
 }

 return -1;
}

int roar_color_to_string  (struct roar_color * c, char * str, size_t len) {
 size_t needlen;

 if ( c == NULL || str == NULL || len == 0 )
  return -1;

 // just to be sure:
 if ( len >= 1 )
  *str = 0;

 switch (c->system) {
  case ROAR_COLORSYSTEM_NONE: needlen = 6; break; // '(none)'
  case ROAR_COLORSYSTEM_RGB:  needlen = 7; break; // '#RRGGBB'
  default:
    return -1;
 }

 needlen++; // terminating \0

 if ( needlen > len )
  return -1;

 switch (c->system) {
  case ROAR_COLORSYSTEM_NONE:
    strcpy(str, "(none)");
   break;
  case ROAR_COLORSYSTEM_RGB:
    snprintf(str, 8, "#%.2X%.2X%.2X", c->color.rgb.r, c->color.rgb.g, c->color.rgb.b);
   break;
 }

 return 0;
}

int roar_color_to_blob    (struct roar_color * c, char * blob, size_t len);
int roar_color_from_blob  (struct roar_color * c, char * blob, size_t len);

//ll

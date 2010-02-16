//meta.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
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

#ifndef _META_H_
#define _META_H_

#include <roaraudio.h>

#ifdef ROAR_SUPPORT_META
int stream_meta_set     (int id, int type, char * name, char * val);
int stream_meta_add     (int id, int type, char * name, char * val);
int stream_meta_get     (int id, int type, char * name, char * val, size_t len);
int stream_meta_list    (int id, int * types, size_t len);
int stream_meta_clear   (int id);
int stream_meta_finalize(int id);
#endif

#endif

//ll

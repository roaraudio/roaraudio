/*
 *
 *  ao_roar.c
 *
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - May 2008
 *      based on ao_esd.c of libao by Stan Seibert - July 2000, July 2001
 *
 *  This file is part of RoarAudio, a cross-platform sound server. See
 *  README for a history of this source code.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <roaraudio.h>
#include <ao/ao.h>
#include <ao/plugin.h>

static char * ao_roar_options[] = {"host"};

/*
typedef struct ao_info {
        int  type; // live output or file output?
        char *name; // full name of driver
        char *short_name; // short name of driver
        char *author; // driver author
        char *comment; // driver comment
        int  preferred_byte_format;
        int  priority;
        char **options;
        int  option_count;
} ao_info;
*/

static ao_info ao_roar_info =
{
 AO_TYPE_LIVE,
 "RoarAudio output",
 "roar",
 "Philipp 'ph3-der-loewe' Schafft, Based on code by: Stan Seibert <volsung@asu.edu>",
 "Outputs to the RoarAudio Sound Daemon.",
 AO_FMT_NATIVE,
 41,
 ao_roar_options,
 1
};


typedef struct ao_roar_internal {
 int    sock;
 char * host;
} ao_roar_internal;


int ao_plugin_test(void) {
 struct roar_connection con;

 if ( roar_simple_connect(&con, NULL, "libao client") == -1 )
  return 0;

 if (roar_get_standby(&con)) {
  roar_disconnect(&con);
  return 0;
 }

 roar_disconnect(&con);
 return 1;
}


ao_info * ao_plugin_driver_info(void) {
 return &ao_roar_info;
}


int ao_plugin_device_init(ao_device * device) {
 ao_roar_internal * internal;

 internal = (ao_roar_internal *) malloc(sizeof(ao_roar_internal));

 if (internal == NULL)	
  return 0;

 internal->host = NULL;

 device->internal = internal;

 return 1;
}

int ao_plugin_set_option(ao_device * device, const char * key, const char * value) {
 ao_roar_internal * internal = (ao_roar_internal *) device->internal;

 if ( strcmp(key, "host") == 0 ) {
  if(internal->host) free(internal->host);
   internal->host = strdup(value);
 }

 return 1;
}

int ao_plugin_open(ao_device * device, ao_sample_format * format) {
 ao_roar_internal * internal = (ao_roar_internal *) device->internal;

 internal->sock = roar_simple_play(format->rate, format->channels, format->bits,
                                   ROAR_CODEC_DEFAULT, internal->host, "libao client");

 if ( internal->sock == -1 )
  return 0;

 device->driver_byte_format = AO_FMT_NATIVE;

 return 1;
}

int ao_plugin_play(ao_device * device, const char * output_samples, uint_32 num_bytes) {
 ao_roar_internal * internal = (ao_roar_internal *) device->internal;

 if (write(internal->sock, output_samples, num_bytes) == -1) {
  return 0;
 } else {
  return 1;
 }
}


int ao_plugin_close(ao_device * device) {
 ao_roar_internal * internal = (ao_roar_internal *) device->internal;

 roar_simple_close(internal->sock);

 return 1;
}


void ao_plugin_device_clear(ao_device * device) {
 ao_roar_internal * internal = (ao_roar_internal *) device->internal;

 if( internal->host != NULL )
  free(internal->host);

 free(internal);
}

//ff_ssdp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroareio a part of RoarAudio,
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroareio.h"

void roar_ff_ssdp_init (struct roar_ff_ssdp * c) {
 memset(c, 0, sizeof(struct roar_ff_ssdp));

 c->method        = ROAR_FF_SSDP_M_NOTIFY;
 c->server        = roar_mm_strdup("RoarAudio libroareio/ff_ssdp.c");
 c->max_age       = 1800;
 c->location      = NULL;
 c->nt            = NULL;
 c->usn           = NULL;
 c->usn_nt_suffix = 1;
 c->host          = roar_mm_strdup(ROAR_FF_SSDP_HOST_UPNP);
}

void roar_ff_ssdp_free (struct roar_ff_ssdp * c) {
 if ( c->server != NULL )
  roar_mm_free(c->server);

 if ( c->location != NULL )
  roar_mm_free(c->location);

 if ( c->nt != NULL )
  roar_mm_free(c->nt);

 if ( c->usn != NULL )
  roar_mm_free(c->usn);

 if ( c->host != NULL )
  roar_mm_free(c->host);

 memset(c, 0, sizeof(struct roar_ff_ssdp));
}

int  roar_ff_ssdp_write(struct roar_vio_calls * vio, struct roar_ff_ssdp * c) {
 char * met;
 char * nts;

 switch (c->method) {
  case ROAR_FF_SSDP_M_NOTIFY:   met = ROAR_FF_SSDP_MS_NOTIFY;   break;
  case ROAR_FF_SSDP_M_M_SEARCH: met = ROAR_FF_SSDP_MS_M_SEARCH; break;
  default: return -1;
 }

 switch (c->nst) {
  case ROAR_FF_SSDP_A_ALIVE:  nts = ROAR_FF_SSDP_AS_ALIVE;  break;
  case ROAR_FF_SSDP_A_BYEBYE: nts = ROAR_FF_SSDP_AS_BYEBYE; break;
  default: return -1;
 }

 roar_vio_printf(vio, "%s * HTTP/1.1\r\n", met);
 roar_vio_printf(vio, "SERVER: %s\r\n", c->server);
 roar_vio_printf(vio, "CACHE-CONTROL: max-age=%i\r\n", c->max_age);
 roar_vio_printf(vio, "LOCATION: %s\r\n", c->location);
 roar_vio_printf(vio, "NTS: %s\r\n", nts);
 roar_vio_printf(vio, "NT: %s\r\n", c->nt);

 if ( c->usn_nt_suffix ) {
  roar_vio_printf(vio, "USN: %s::%s\r\n", c->usn, c->nt);
 } else {
  roar_vio_printf(vio, "USN: %s\r\n", c->usn);
 }

 roar_vio_printf(vio, "HOST: %s\r\n", c->host);
 roar_vio_printf(vio, "\r\n");

 return 0;
}
int  roar_ff_ssdp_read (struct roar_vio_calls * vio, struct roar_ff_ssdp * c) {
 return -1;
}


//ll

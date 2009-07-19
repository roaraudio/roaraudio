//roarmixer.c:
/* GStreamer
 * Copyright (C) <2005> Arwed v. Merkatz <v.merkatz@gmx.net>
 * Copyright (C) <2008> Philipp 'ph3-der-loewe' Schafft
 * Copyright (C) <2009> Philipp 'ph3-der-loewe' Schafft
 *                            <lion@lion.leolix.org>
 *
 * roarmixer.h: an RoarAudio audio mixer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "roarmixer.h"

GstRoarMixer*    gst_roarmixer_new                (const gchar *device,
                                                 GstRoarMixerDirection dir) {
  GstRoarMixer *ret = NULL;

  g_return_val_if_fail(device != NULL, NULL);

  ret = g_new0(GstRoarMixer, 1);

  ret->device = g_strdup(device);
  ret->dir    = dir;

/*
  if (!gst_ossmixer_open (ret))
    goto error;
*/

  if ( roar_simple_connect(&(ret->con), NULL, "gstroarmixer") == -1 )
   goto error;

  return ret;

error:
  if (ret)
    gst_roarmixer_free (ret);

  return NULL;
}

void            gst_roarmixer_free               (GstRoarMixer *mixer) {
  g_return_if_fail(mixer != NULL);

  if (mixer->device) {
    g_free(mixer->device);
    mixer->device = NULL;
  }

  if (mixer->cardname) {
    g_free(mixer->cardname);
    mixer->cardname = NULL;
  }

  if (mixer->tracklist) {
    g_list_foreach(mixer->tracklist, (GFunc) g_object_unref, NULL);
    g_list_free(mixer->tracklist);
    mixer->tracklist = NULL;
  }

  roar_disconnect(&(mixer->con));

  g_free (mixer);
}

const GList*    gst_roarmixer_list_tracks        (GstRoarMixer * mixer) {
 return (const GList *) mixer->tracklist;
}

void            gst_roarmixer_set_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes) {
}
void            gst_roarmixer_get_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes) {
}
void            gst_roarmixer_set_record         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean record) {
}
void            gst_roarmixer_set_mute           (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean mute) {
}

//ll

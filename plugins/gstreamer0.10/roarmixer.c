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

GST_DEBUG_CATEGORY_EXTERN (roar_debug);
#define GST_CAT_DEFAULT roar_debug

enum
{
  PROP_DEVICE_NAME = 1
};


static const GstElementDetails gst_roar_mixer_element_details =
GST_ELEMENT_DETAILS ("RoarAudio Mixer",
    "Generic/Audio",
    "Control sound input and output levels with RoarAudio",
    "Philipp 'ph3-der-loewe' Schafft <lion@lion.leolix.org>");

GST_BOILERPLATE_WITH_INTERFACE (GstRoarMixerElement, gst_roar_mixer_element,
    GstElement, GST_TYPE_ELEMENT, GstMixer, GST_TYPE_MIXER,
    gst_roar_mixer_element);

GST_IMPLEMENT_ROAR_MIXER_METHODS (GstRoarMixerElement, gst_roar_mixer_element);

static GstStateChangeReturn gst_roar_mixer_element_change_state (GstElement *
    element, GstStateChange transition);

static void gst_roar_mixer_element_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static void
gst_roar_mixer_element_base_init (gpointer klass)
{
  gst_element_class_set_details (GST_ELEMENT_CLASS (klass),
      &gst_roar_mixer_element_details);
}

static void
gst_roar_mixer_element_class_init (GstRoarMixerElementClass * klass)
{
  GstElementClass *element_class;
  GObjectClass *gobject_class;

  element_class = (GstElementClass *) klass;
  gobject_class = (GObjectClass *) klass;

  gobject_class->get_property = gst_roar_mixer_element_get_property;

  g_object_class_install_property (gobject_class, PROP_DEVICE_NAME,
      g_param_spec_string ("device-name", "Device name",
          "Human-readable name of the sound device", "", G_PARAM_READABLE));

  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_roar_mixer_element_change_state);
}

static void
gst_roar_mixer_element_init (GstRoarMixerElement * this,
    GstRoarMixerElementClass * g_class)
{
  this->mixer = NULL;
}

static void
gst_roar_mixer_element_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstRoarMixerElement *this = GST_ROAR_MIXER_ELEMENT (object);

  switch (prop_id) {
    case PROP_DEVICE_NAME:
      if (this->mixer) {
        g_value_set_string (value, this->mixer->cardname);
      } else {
        g_value_set_string (value, NULL);
      }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_roar_mixer_element_change_state (GstElement * element,
    GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstRoarMixerElement *this = GST_ROAR_MIXER_ELEMENT (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (!this->mixer) {
        this->mixer = gst_roarmixer_new ("/dev/mixer", GST_ROAR_MIXER_ALL);
      }
      break;
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (this->mixer) {
        gst_roarmixer_free (this->mixer);
        this->mixer = NULL;
      }
      break;
    default:
      break;
  }

  return ret;
}


gboolean gst_roarmixer_factory_init (GstPlugin * plugin) {
  if (!gst_element_register(plugin, "roarmixer", GST_RANK_MARGINAL,
                            GST_TYPE_ROARMIXER))
    return FALSE;

  return TRUE;
}

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

/* unused with G_DISABLE_* */
static G_GNUC_UNUSED gboolean gst_roarmixer_contains_track (GstRoarMixer * mixer,
                                                            GstRoarMixerTrack * roartrack) {
  const GList *item;

  for (item = mixer->tracklist; item != NULL; item = item->next)
    if (item->data == roartrack)
      return TRUE;

  return FALSE;
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

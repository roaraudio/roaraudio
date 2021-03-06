//roarmixer.c:
/* GStreamer
 * Copyright (C) <2005>      Arwed v. Merkatz <v.merkatz@gmx.net>
 * Copyright (C) <2008-2010> Philipp 'ph3-der-loewe' Schafft
 *                                    <lion@lion.leolix.org>
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
 * Free Software Foundation, 51 Franklin Street, Fifth Floor,
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
        this->mixer = gst_roarmixer_new(NULL, GST_ROAR_MIXER_ALL);
      }
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (this->mixer) {
        gst_roarmixer_free(this->mixer);
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
                            GST_TYPE_ROAR_MIXER_ELEMENT))
    return FALSE;

  return TRUE;
}

G_DEFINE_TYPE (GstRoarMixerTrack, gst_roarmixer_track, GST_TYPE_MIXER_TRACK);

GstRoarMixer*    gst_roarmixer_new                (const gchar *device,
                                                 GstRoarMixerDirection dir) {
  GstRoarMixer *ret = NULL;

  ROAR_WARN("gst_roarmixer_new(device='%s', dir=0x%.4x) = ?", device, dir);

  ret = g_new0(GstRoarMixer, 1);

  ret->device = g_strdup(device);
  ret->dir    = dir;
  ret->cardname = g_strdup("RoarAudio Default Device");

/*
  if (!gst_ossmixer_open (ret))
    goto error;
*/

  if ( roar_simple_connect(&(ret->con), (char*)device, "gstroarmixer") == -1 )
   goto error;

  ROAR_WARN("gst_roarmixer_new(device='%s', dir=0x%.4x) = %p", device, dir, ret);
  return ret;

error:
  if (ret)
    gst_roarmixer_free (ret);

  ROAR_WARN("gst_roarmixer_new(device='%s', dir=0x%.4x) = NULL // Error?", device, dir);
  return NULL;
}

void            gst_roarmixer_free               (GstRoarMixer *mixer) {
  g_return_if_fail(mixer != NULL);

  ROAR_WARN("gst_roarmixer_free(mixer=%p) = ?", mixer);

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

  ROAR_WARN("gst_roarmixer_free(mixer=%p) = (void)", mixer);
}

/* unused with G_DISABLE_* */
static G_GNUC_UNUSED gboolean gst_roarmixer_contains_track (GstRoarMixer * mixer,
                                                            GstRoarMixerTrack * roartrack) {
  const GList *item;

  if ( mixer == NULL || mixer->tracklist == NULL )
   return FALSE;

  for (item = mixer->tracklist; item != NULL; item = item->next)
    if (item->data == roartrack)
      return TRUE;

  return FALSE;
}

void            gst_roarmixer_updatestreamlist   (GstRoarMixer *mixer) {
  gint  i      = 0;
  gint num;
  gint id[ROAR_STREAMS_MAX];
  GstMixerTrack *track;

  ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p) = ? // ########################", mixer);
  ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p): tracklist=%p", mixer, mixer->tracklist);

  if (mixer->tracklist) {
    ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p) = (void)", mixer);
    return;
  }

/*
  if (mixer->tracklist) {
    g_list_foreach(mixer->tracklist, (GFunc) g_object_unref, NULL);
    g_list_free(mixer->tracklist);
    mixer->tracklist = NULL;
  }
*/

  ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p): tracklist=%p", mixer, mixer->tracklist);

  if ( (num = roar_list_streams(&(mixer->con), id, ROAR_STREAMS_MAX)) == -1 ) {
    return;
  }


  for (i = 0; i < num; i++) {
   track = gst_roarmixer_track_new(mixer, id[i]);
   ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p): track=%p", mixer, track);

   if ( track == NULL )
    continue;

   mixer->tracklist = g_list_append(mixer->tracklist, track);
  }

  ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p): tracklist=%p", mixer, mixer->tracklist);

  ROAR_WARN("gst_roarmixer_updatestreamlist(mixer=%p) = (void) // ###################", mixer);
}

const GList*    gst_roarmixer_list_tracks        (GstRoarMixer * mixer) {
 ROAR_WARN("gst_roarmixer_list_tracks(mixer=%p) = ?", mixer);

 gst_roarmixer_updatestreamlist(mixer);

 ROAR_WARN("gst_roarmixer_list_tracks(mixer=%p) = %p", mixer, mixer->tracklist);
 return (const GList *) mixer->tracklist;
}

void            gst_roarmixer_set_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes) {
 GstRoarMixerTrack *roartrack = GST_ROARMIXER_TRACK(track);
 int channels;
 struct roar_mixer_settings m;
 gint i;


 g_return_if_fail(gst_roarmixer_contains_track(mixer, roartrack));

 if ( roar_get_vol(&(mixer->con), roartrack->stream_id, &m, &channels) == -1 ) {
  ROAR_WARN("gst_roarmixer_get_volume(*): can not get mixer infos for stream %i", roartrack->stream_id);
  return;
 }

 if ( channels != track->num_channels ) {
  ROAR_WARN("gst_roarmixer_get_volume(*): numer of channels for stream %i mismatch", roartrack->stream_id);

  if ( track->num_channels < channels )
   channels = track->num_channels;
 }

 for (i = 0; i < channels; i++) {
  m.mixer[i] = volumes[i];
 }

 m.scale = 65535;

 roar_set_vol(&(mixer->con), roartrack->stream_id, &m, channels);
}

void            gst_roarmixer_get_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes) {
 GstRoarMixerTrack *roartrack = GST_ROARMIXER_TRACK(track);
 int channels;
 struct roar_mixer_settings m;
 gint i;


 g_return_if_fail(gst_roarmixer_contains_track(mixer, roartrack));

 if ( roar_get_vol(&(mixer->con), roartrack->stream_id, &m, &channels) == -1 ) {
  ROAR_WARN("gst_roarmixer_get_volume(*): can not get mixer infos for stream %i", roartrack->stream_id);
  return;
 }

 if ( channels != track->num_channels ) {
  ROAR_WARN("gst_roarmixer_get_volume(*): numer of channels for stream %i mismatch", roartrack->stream_id);

  if ( track->num_channels < channels )
   channels = track->num_channels;
 }

 for (i = 0; i < channels; i++) {
  if ( m.scale == 65535 ) {
   volumes[i] = m.mixer[i];
  } else {
   volumes[i] = m.mixer[i] * 65535. / (float) m.scale; // we do not hanle precides scaling here
                                                       // as it does not matter:
                                                       // we never write those values back to roard.
  }
 }

}
void            gst_roarmixer_set_record         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean record) {
}
void            gst_roarmixer_set_mute           (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean mute) {
  GstRoarMixerTrack *roartrack = GST_ROARMIXER_TRACK(track);
  struct roar_stream s;

  g_return_if_fail(gst_roarmixer_contains_track(mixer, roartrack));

  roar_stream_new_by_id(&s, roartrack->stream_id);

  if (mute) {
    roar_stream_set_flags(&(mixer->con), &s, ROAR_FLAG_MUTE, ROAR_SET_FLAG);
  } else {
    roar_stream_set_flags(&(mixer->con), &s, ROAR_FLAG_MUTE, ROAR_RESET_FLAG);
  }
}

// tracks:
#define MASK_BIT_IS_SET(mask, bit) \
  (mask & (1 << bit))

static void
gst_roarmixer_track_class_init (GstRoarMixerTrackClass * klass)
{
  /* nop */
}

static void
gst_roarmixer_track_init (GstRoarMixerTrack * track)
{
  //memset(track, 0, sizeof(*track));
  track->stream_id = -1;
  ROAR_WARN("gst_roarmixer_track_init(track=%p) = (void)", track);
}

GstMixerTrack *
gst_roarmixer_track_new (GstRoarMixer * mixer,
    gint stream_id)
{
  GstRoarMixerTrack *roartrack;
  GstMixerTrack *track;
  struct roar_stream s;
  struct roar_client c;
  struct roar_meta   m;
  gint flags = 0;
  gchar buf[1024];
  int num;
  int id[ROAR_CLIENTS_MAX];
  char * clientname = NULL;
  char * metaname   = NULL;
  char   streamname[1024] = {0};
  gint i, h;

  ROAR_WARN("gst_roarmixer_track_new(mixer=%p, stream_id=%i) = ?", mixer, stream_id);

  if ( roar_get_stream(&(mixer->con), &s, stream_id) == -1 ) {
    return NULL;
  }

 // TODO: find something more efficent:

 if ( (num = roar_list_clients(&(mixer->con), id, ROAR_CLIENTS_MAX)) != -1 ) {
  for (i = 0; i < num; i++) {
   ROAR_DBG("gst_roarmixer_track_new(*): stream %i -->> client %i?", stream_id, id[i]);
   if ( roar_get_client(&(mixer->con), &c, id[i]) != -1 ) {
    for (h = 0; h < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; h++) {
     ROAR_DBG("gst_roarmixer_track_new(*): stream %i <-> %i -->> client %i?", stream_id, c.streams[h], id[i]);
     if ( c.streams[h] == stream_id ) {
      clientname = c.name;
      h = ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT;
      i = num;
     }
    }
   }
  }
 }

 m.type = ROAR_META_TYPE_TITLE;
 if ( roar_stream_meta_get(&(mixer->con), &s, &m) != -1 ) {
  metaname = m.value;
 }

 if ( roar_stream_get_name(&(mixer->con), &s, streamname, 1024) != 0 )
  *streamname = 0;

  switch (s.dir) {
   case ROAR_DIR_PLAY:
     flags |= GST_MIXER_TRACK_OUTPUT;
    break;
   case ROAR_DIR_MIXING:
     flags |= GST_MIXER_TRACK_OUTPUT;
    break;
   case ROAR_DIR_OUTPUT:
     flags |= GST_MIXER_TRACK_MASTER;
   case ROAR_DIR_MONITOR:
     flags |= GST_MIXER_TRACK_OUTPUT;
    break;
   default: return NULL;
  }

#if 0
  if ( !*streamname && metaname == NULL )
   sprintf(streamname, "[Stream %i]", stream_id);

  sprintf(buf, "%s\n%s",  clientname ? clientname : "[Unknown]",
                         *streamname ? streamname : metaname
                         );
#else
  if ( *streamname ) {
   strcpy(buf, streamname);
  } else {
   sprintf(buf, "Stream %i/%s", stream_id, clientname);
  }
#endif

  roartrack = g_object_new(GST_TYPE_ROARMIXER_TRACK, NULL);
  ROAR_WARN("gst_roarmixer_track_new(*): roartrack=%p", roartrack);
  track               = GST_MIXER_TRACK(roartrack);
  track->label        = g_strdup(buf);
  track->num_channels = s.info.channels;
  track->flags        = flags;
  track->min_volume   = 0;
  track->max_volume   = 65535;
  roartrack->stream_id = stream_id;

  /* volume */
  ROAR_WARN("gst_roarmixer_track_new(mixer=%p, stream_id=%i) = %p", mixer, stream_id, track);
  return track;
}

//ll

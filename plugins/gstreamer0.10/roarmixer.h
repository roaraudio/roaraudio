//roarmixer.h:
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


#ifndef __GST_ROARMIXER_H__
#define __GST_ROARMIXER_H__

#include <gst/gst.h>
#include <gst/interfaces/mixer.h>
#include <roaraudio.h>

#define VERSION "0.0.1"
#define PACKAGE "gst-plugins"
#define GST_PACKAGE_ORIGIN "Unknown package origin"
#define GST_PACKAGE_NAME "GStreamer Plug-ins source release"

G_BEGIN_DECLS

#define GST_ROAR_MIXER_ELEMENT(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ROAR_MIXER_ELEMENT,GstRoarMixerElement))
#define GST_ROAR_MIXER_ELEMENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ROAR_MIXER_ELEMENT,GstRoarMixerElementClass))
#define GST_IS_ROAR_MIXER_ELEMENT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ROAR_MIXER_ELEMENT))
#define GST_IS_ROAR_MIXER_ELEMENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ROAR_MIXER_ELEMENT))
#define GST_TYPE_ROAR_MIXER_ELEMENT              (gst_roar_mixer_element_get_type())

#define GST_TYPE_ROARMIXER_TRACK \
  (gst_roarmixer_track_get_type ())
#define GST_ROARMIXER_TRACK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_ROARMIXER_TRACK, \
                               GstRoarMixerTrack))
#define GST_ROARMIXER_TRACK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_ROARMIXER_TRACK, \
                            GstRoarMixerTrackClass))
#define GST_IS_ROARMIXER_TRACK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_ROARMIXER_TRACK))
#define GST_IS_ROARMIXER_TRACK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_ROARMIXER_TRACK))


typedef struct _GstRoarMixerElement GstRoarMixerElement;
typedef struct _GstRoarMixerElementClass GstRoarMixerElementClass;
typedef struct _GstRoarMixer GstRoarMixer;

struct _GstRoarMixerElement {
  GstElement            parent;

  GstRoarMixer           *mixer;
};

struct _GstRoarMixerElementClass {
  GstElementClass       parent;
};


GType           gst_roar_mixer_element_get_type          (void);

typedef enum {
  GST_ROAR_MIXER_CAPTURE = 1<<0,
  GST_ROAR_MIXER_PLAYBACK = 1<<1,
  GST_ROAR_MIXER_ALL = GST_ROAR_MIXER_CAPTURE | GST_ROAR_MIXER_PLAYBACK
} GstRoarMixerDirection;


#define GST_ROAR_MIXER(obj)              ((GstRoarMixer*)(obj))

struct _GstRoarMixer {
  GList *               tracklist;      /* list of available tracks */

  struct roar_connection con;
  int       fd;
  gchar    *host;

  gchar *               device;
  gchar *               cardname;
  GstRoarMixerDirection dir;
};

typedef struct _GstRoarMixerTrack {
  GstMixerTrack parent;

  struct roar_mixer_settings mixer;
  gint          channels;
  gchar       * name;
  gint          stream_id;
} GstRoarMixerTrack;

typedef struct _GstRoarMixerTrackClass {
  GstMixerTrackClass parent;
} GstRoarMixerTrackClass;

GType gst_roarmixer_get_type(void);
gboolean gst_roarmixer_factory_init (GstPlugin *plugin);

GstRoarMixer*    gst_roarmixer_new                (const gchar *device,
                                                 GstRoarMixerDirection dir);
void            gst_roarmixer_free               (GstRoarMixer *mixer);
void            gst_roarmixer_updatestreamlist   (GstRoarMixer *mixer);

const GList*    gst_roarmixer_list_tracks        (GstRoarMixer * mixer);
void            gst_roarmixer_set_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes);
void            gst_roarmixer_get_volume         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gint * volumes);
void            gst_roarmixer_set_record         (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean record);
void            gst_roarmixer_set_mute           (GstRoarMixer * mixer,
                                                 GstMixerTrack * track,
                                                 gboolean mute);

GType           gst_roarmixer_track_get_type     (void);
GstMixerTrack *
gst_roarmixer_track_new (GstRoarMixer * mixer,
    gint stream_id);

#define GST_IMPLEMENT_ROAR_MIXER_METHODS(Type, interface_as_function)            \
static gboolean                                                                 \
interface_as_function ## _supported (Type *this, GType iface_type)              \
{                                                                               \
  g_assert (iface_type == GST_TYPE_MIXER);                                      \
                                                                                \
  return (this->mixer != NULL);                                                 \
}                                                                               \
                                                                                \
static const GList*                                                             \
interface_as_function ## _list_tracks (GstMixer * mixer)                        \
{                                                                               \
  Type *this = (Type*) mixer;                                                   \
                                                                                \
  g_return_val_if_fail (this != NULL, NULL);                                    \
  g_return_val_if_fail (this->mixer != NULL, NULL);                             \
                                                                                \
  return gst_roarmixer_list_tracks (this->mixer);                                \
}                                                                               \
                                                                                \
static void                                                                     \
interface_as_function ## _set_volume (GstMixer * mixer, GstMixerTrack * track,  \
    gint * volumes)                                                             \
{                                                                               \
  Type *this = (Type*) mixer;                                                   \
                                                                                \
  g_return_if_fail (this != NULL);                                              \
  g_return_if_fail (this->mixer != NULL);                                       \
                                                                                \
  gst_roarmixer_set_volume (this->mixer, track, volumes);                        \
}                                                                               \
                                                                                \
static void                                                                     \
interface_as_function ## _get_volume (GstMixer * mixer, GstMixerTrack * track,  \
    gint * volumes)                                                             \
{                                                                               \
  Type *this = (Type*) mixer;                                                   \
                                                                                \
  g_return_if_fail (this != NULL);                                              \
  g_return_if_fail (this->mixer != NULL);                                       \
                                                                                \
  gst_roarmixer_get_volume (this->mixer, track, volumes);                        \
}                                                                               \
                                                                                \
static void                                                                     \
interface_as_function ## _set_record (GstMixer * mixer, GstMixerTrack * track,  \
    gboolean record)                                                            \
{                                                                               \
  Type *this = (Type*) mixer;                                                   \
                                                                                \
  g_return_if_fail (this != NULL);                                              \
  g_return_if_fail (this->mixer != NULL);                                       \
                                                                                \
  gst_roarmixer_set_record (this->mixer, track, record);                         \
}                                                                               \
                                                                                \
static void                                                                     \
interface_as_function ## _set_mute (GstMixer * mixer, GstMixerTrack * track,    \
    gboolean mute)                                                              \
{                                                                               \
  Type *this = (Type*) mixer;                                                   \
                                                                                \
  g_return_if_fail (this != NULL);                                              \
  g_return_if_fail (this->mixer != NULL);                                       \
                                                                                \
  gst_roarmixer_set_mute (this->mixer, track, mute);                             \
}                                                                               \
                                                                                \
static void                                                                     \
interface_as_function ## _interface_init (GstMixerClass * klass)                \
{                                                                               \
  GST_MIXER_TYPE (klass) = GST_MIXER_HARDWARE;                                  \
                                                                                \
  /* set up the interface hooks */                                              \
  klass->list_tracks = interface_as_function ## _list_tracks;                   \
  klass->set_volume = interface_as_function ## _set_volume;                     \
  klass->get_volume = interface_as_function ## _get_volume;                     \
  klass->set_mute = interface_as_function ## _set_mute;                         \
  klass->set_record = interface_as_function ## _set_record;                     \
}


G_END_DECLS

#endif /* __GST_ROARMIXER_H__ */

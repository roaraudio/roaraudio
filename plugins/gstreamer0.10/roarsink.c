//roarsink.c:
/* GStreamer
 * Copyright (C) <2005>      Arwed v. Merkatz <v.merkatz@gmx.net>
 * Copyright (C) <2008,2009> Philipp 'ph3-der-loewe' Schafft
 *                            <lion@lion.leolix.org>
 *
 * Roughly based on the gstreamer 0.8 esdsink plugin:
 * Copyright (C) <2001> Richard Boulton <richard-gst@tartarus.org>
 *
 * roarsink.c: an RoarAudio audio sink
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "roarsink.h"
#include <unistd.h>
#include <errno.h>

#define _(x) (x)

//#include <gst/gst-i18n-plugin.h>

#ifndef ROAR_MAX_WRITE_SIZE
#define ROAR_MAX_WRITE_SIZE (21 * 4096)
#endif
#define ROAR_BUF_SIZE 1764 /* this is dynamic for RoarAudio, use the most common value here for the moment */

//GST_DEBUG_CATEGORY_EXTERN (roar_debug);
//#define GST_CAT_DEFAULT roar_debug

/* elementfactory information */
static const GstElementDetails roarsink_details =
GST_ELEMENT_DETAILS("RoarAudio audio sink",
    "Sink/Audio",
    "Plays audio to an RoarAudio server",
    "Philipp 'ph3-der-loewe' Schafft <lion@lion.leolix.org>");

enum
{
  PROP_0,
  PROP_HOST
};

#define _QM(x) #x
#define  QM(x) _QM(x)

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS(
        "audio/x-alaw, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]; "

        "audio/x-mulaw, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]; "

/*
        "audio/x-raw-int, "
        "endianness = (int) { BIG_ENDIAN, LITTLE_ENDIAN }, "
        "signed = (boolean) { true, false }, "
        "width = (int) 32, "
        "depth = (int) 32, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]; "
*/

/*
        "audio/x-raw-int, "
        "endianness = (int) { BIG_ENDIAN, LITTLE_ENDIAN }, "
        "signed = (boolean) { true, false }, "
        "width = (int) 24, "
        "depth = (int) 24, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]; "
*/

        "audio/x-raw-int, "
        "endianness = (int) { BIG_ENDIAN, LITTLE_ENDIAN }, "
        "signed = (boolean) { true, false }, "
        "width = (int) 16, "
        "depth = (int) 16, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]; "

        "audio/x-raw-int, "
        "signed = (boolean) { true, false }, "
        "width = (int) 8, "
        "depth = (int) 8, "
        "rate = (int) [ 1, MAX ], "
        "channels = (int) [ 1, " QM(ROAR_MAX_CHANNELS) " ]"
       )
    );

#undef _QM
#undef  QM

static void gst_roarsink_finalize (GObject * object);

static GstCaps *gst_roarsink_getcaps (GstBaseSink * bsink);

static gboolean gst_roarsink_open (GstAudioSink * asink);
static gboolean gst_roarsink_close (GstAudioSink * asink);
static gboolean gst_roarsink_prepare (GstAudioSink * asink,
    GstRingBufferSpec * spec);
static gboolean gst_roarsink_unprepare (GstAudioSink * asink);
static guint gst_roarsink_write (GstAudioSink * asink, gpointer data,
    guint length);
static guint gst_roarsink_delay (GstAudioSink * asink);
static void gst_roarsink_reset (GstAudioSink * asink);

static void gst_roarsink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_roarsink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

GST_BOILERPLATE (GstRoarSink, gst_roarsink, GstAudioSink, GST_TYPE_AUDIO_SINK);

static void gst_roarsink_base_init (gpointer g_class) {
  GstElementClass *element_class = GST_ELEMENT_CLASS(g_class);

  gst_element_class_add_pad_template(element_class,
      gst_static_pad_template_get(&sink_factory));
  gst_element_class_set_details(element_class, &roarsink_details);
}

static void gst_roarsink_class_init (GstRoarSinkClass * klass) {
  GObjectClass *gobject_class;
  GstBaseSinkClass *gstbasesink_class;
  GstBaseAudioSinkClass *gstbaseaudiosink_class;
  GstAudioSinkClass *gstaudiosink_class;

  gobject_class          = (GObjectClass *) klass;
  gstbasesink_class      = (GstBaseSinkClass *) klass;
  gstbaseaudiosink_class = (GstBaseAudioSinkClass *) klass;
  gstaudiosink_class     = (GstAudioSinkClass *) klass;

  parent_class = g_type_class_peek_parent(klass);

  gobject_class->finalize = gst_roarsink_finalize;

  gstbasesink_class->get_caps   = GST_DEBUG_FUNCPTR(gst_roarsink_getcaps);

  gstaudiosink_class->open      = GST_DEBUG_FUNCPTR(gst_roarsink_open);
  gstaudiosink_class->close     = GST_DEBUG_FUNCPTR(gst_roarsink_close);
  gstaudiosink_class->prepare   = GST_DEBUG_FUNCPTR(gst_roarsink_prepare);
  gstaudiosink_class->unprepare = GST_DEBUG_FUNCPTR(gst_roarsink_unprepare);
  gstaudiosink_class->write     = GST_DEBUG_FUNCPTR(gst_roarsink_write);
  gstaudiosink_class->delay     = GST_DEBUG_FUNCPTR(gst_roarsink_delay);
  gstaudiosink_class->reset     = GST_DEBUG_FUNCPTR(gst_roarsink_reset);

  gobject_class->set_property   = gst_roarsink_set_property;
  gobject_class->get_property   = gst_roarsink_get_property;

  /* default value is filled in the _init method */
  g_object_class_install_property(gobject_class, PROP_HOST,
      g_param_spec_string("host", "Host",
          "The host running the RoarAudio daemon", NULL, G_PARAM_READWRITE));
}

static void gst_roarsink_init (GstRoarSink * roarsink, GstRoarSinkClass * klass) {
  memset(&(roarsink->con), 0, sizeof(roarsink->con));
  roar_connect_fh(&(roarsink->con), -1);
  roarsink->fd     = -1;
  roarsink->host   = NULL;
}

static void gst_roarsink_finalize (GObject * object) {
  GstRoarSink *roarsink = GST_ROARSINK(object);

  gst_caps_replace(&roarsink->cur_caps, NULL);
  if ( roarsink->host != NULL )
   g_free(roarsink->host);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GstCaps * gst_roarsink_getcaps (GstBaseSink * bsink) {
  GstRoarSink *roarsink;

  roarsink = GST_ROARSINK(bsink);

  /* no fd, we're done with the template caps */
  if (roar_get_connection_fh(&(roarsink->con)) < 0 || roarsink->cur_caps == NULL) {
    GST_LOG_OBJECT(roarsink, "getcaps called, returning template caps");
    return NULL;
  }

  GST_LOG_OBJECT(roarsink, "returning %" GST_PTR_FORMAT, roarsink->cur_caps);

  return gst_caps_ref(roarsink->cur_caps);
}

static gboolean gst_roarsink_open(GstAudioSink * asink) {
  GstPadTemplate *pad_template;
  GstRoarSink *roarsink;
  gint i;
  struct roar_stream oinfo;

  roarsink = GST_ROARSINK(asink);

  GST_DEBUG_OBJECT(roarsink, "open");

  /* now try to connect to any existing/running sound daemons */
  if ( roar_simple_connect(&(roarsink->con), roarsink->host, "gstreamer client") == -1 )
    goto couldnt_connect;

  /* get server info */
  if ( roar_server_oinfo(&(roarsink->con), &oinfo) == -1 )
    goto no_server_info;

  GST_INFO_OBJECT(roarsink, "got server info rate: %i", oinfo.info.rate);

  pad_template = gst_static_pad_template_get(&sink_factory);
  roarsink->cur_caps = gst_caps_copy(gst_pad_template_get_caps(pad_template));

  for (i = 0; i < roarsink->cur_caps->structs->len; i++) {
    GstStructure *s;

    s = gst_caps_get_structure(roarsink->cur_caps, i);
    gst_structure_set(s, "rate", G_TYPE_INT, oinfo.info.rate, NULL);
  }

  GST_INFO_OBJECT(roarsink, "server caps: %" GST_PTR_FORMAT, roarsink->cur_caps);

  return TRUE;

  /* ERRORS */
couldnt_connect:
  {
    GST_ELEMENT_ERROR(roarsink, RESOURCE, OPEN_WRITE,
        (_("Could not establish connection to sound server")),
        ("can't open connection to RoarAudio server"));
    return FALSE;
  }
no_server_info:
  {
    GST_ELEMENT_ERROR(roarsink, RESOURCE, OPEN_WRITE,
        (_("Failed to query sound server capabilities")),
        ("couldn't get server info!"));
    return FALSE;
  }
}

static gboolean gst_roarsink_close (GstAudioSink * asink) {
  GstRoarSink *roarsink = GST_ROARSINK(asink);

  GST_DEBUG_OBJECT(roarsink, "close");

  gst_caps_replace(&roarsink->cur_caps, NULL);

  roar_disconnect(&(roarsink->con));

  if ( roarsink->fd != -1 )
   close(roarsink->fd);

  return TRUE;
}

static gboolean gst_roarsink_prepare (GstAudioSink * asink, GstRingBufferSpec * spec) {
  GstRoarSink *roarsink = GST_ROARSINK(asink);
  int codec = ROAR_CODEC_DEFAULT;
  int bits  = spec->depth;

  GST_DEBUG_OBJECT(roarsink, "prepare");

  
  GST_INFO_OBJECT(roarsink, "attempting to open data connection to RoarAudio server");

  switch (spec->type) {
   case GST_BUFTYPE_LINEAR:
     switch (spec->sign) {
      case TRUE:
        switch (spec->bigend) {
         case TRUE:  codec = ROAR_CODEC_PCM_S_BE; break;
         case FALSE: codec = ROAR_CODEC_PCM_S_LE; break;
         default: return FALSE; break;
        }
       break;
      case FALSE:
        switch (spec->bigend) {
         case TRUE:  codec = ROAR_CODEC_PCM_U_BE; break;
         case FALSE: codec = ROAR_CODEC_PCM_U_LE; break;
         default: return FALSE; break;
        }
       break;
      default: return FALSE; break;
     }
    break;
   case GST_BUFTYPE_A_LAW:
     codec = ROAR_CODEC_ALAW;
     bits  = 8;
    break;
   case GST_BUFTYPE_MU_LAW:
     codec = ROAR_CODEC_MULAW;
     bits  = 8;
    break;
   default:
     return FALSE;
  }

  roarsink->fd = roar_simple_new_stream(&(roarsink->con), spec->rate, spec->channels, bits,
                                        codec, ROAR_DIR_PLAY);

  if ( roarsink->fd == -1 )
    goto cannot_open;

  roarsink->rate = spec->rate;

  spec->segsize  = ROAR_BUF_SIZE;
  spec->segtotal = (ROAR_MAX_WRITE_SIZE / spec->segsize);

  /* FIXME: this is wrong for signed ints (and the
   * audioringbuffers should do it for us anyway) */
  spec->bytes_per_sample = spec->channels * bits / 8;
  memset(&(spec->silence_sample), 0, sizeof(spec->silence_sample));

  GST_INFO_OBJECT(roarsink, "successfully opened connection to RoarAudio server");

  return TRUE;

  /* ERRORS */
cannot_open:
  {
    GST_ELEMENT_ERROR(roarsink, RESOURCE, OPEN_WRITE,
        (_("Could not establish connection to sound server")),
        ("can't open connection to RoarAudio server"));
    return FALSE;
  }
}

static gboolean gst_roarsink_unprepare (GstAudioSink * asink) {
  GstRoarSink *roarsink = GST_ROARSINK(asink);

  if ((roarsink->fd == -1) && (roar_get_connection_fh(&(roarsink->con)) == -1))
    return TRUE;

  close(roarsink->fd);
  roarsink->fd = -1;

  GST_INFO_OBJECT(roarsink, "closed sound device");

  return TRUE;
}


static guint gst_roarsink_write (GstAudioSink * asink, gpointer data, guint length) {
  GstRoarSink *roarsink = GST_ROARSINK(asink);
  gint to_write = 0;

  to_write = length;

  while (to_write > 0) {
    int done;

    done = write(roarsink->fd, data, to_write);

    if (done < 0)
      goto write_error;

    to_write -= done;
    data += done;
  }
  return length;

  /* ERRORS */
write_error:
  {
    GST_ELEMENT_ERROR(roarsink, RESOURCE, WRITE,
        ("Failed to write data to the RoarAudio daemon"), GST_ERROR_SYSTEM);
    return 0;
  }
}

static guint gst_roarsink_delay (GstAudioSink * asink) {
//  GstRoarSink *roarsink = GST_ROARSINK (asink);
  guint latency;

  latency = 441; // compile type depending and link level deppendent,
                 // use default value for local operations here for the moment

  GST_DEBUG_OBJECT(asink, "got latency: %u", latency);

  return latency;
}

static void gst_roarsink_reset (GstAudioSink * asink) {
  GST_DEBUG_OBJECT(asink, "reset called");
}

static void gst_roarsink_set_property (GObject * object, guint prop_id, const GValue * value,
                                       GParamSpec * pspec) {
  GstRoarSink *roarsink = GST_ROARSINK(object);

  switch (prop_id) {
    case PROP_HOST:
      if ( roarsink->host != NULL )
       g_free(roarsink->host);
      roarsink->host = g_value_dup_string(value);
     break;
    default:
      break;
  }
}

static void gst_roarsink_get_property (GObject * object, guint prop_id, GValue * value,
                                       GParamSpec * pspec) {
  GstRoarSink *roarsink = GST_ROARSINK(object);

  switch (prop_id) {
    case PROP_HOST:
      g_value_set_string(value, roarsink->host);
     break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
     break;
  }
}

gboolean gst_roarsink_factory_init (GstPlugin * plugin) {
  if (!gst_element_register(plugin, "roarsink", GST_RANK_MARGINAL,
                            GST_TYPE_ROARSINK))
    return FALSE;

  return TRUE;
}

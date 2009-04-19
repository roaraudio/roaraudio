//roarsink.h:
/* GStreamer
 * Copyright (C) <2005> Arwed v. Merkatz <v.merkatz@gmx.net>
 * Copyright (C) <2008> Philipp 'ph3-der-loewe' Schafft
 *                            <lion@lion.leolix.org>
 *
 * roarsink.h: an RoarAudio audio sink
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


#ifndef __GST_ROARSINK_H__
#define __GST_ROARSINK_H__

#include <gst/gst.h>
#include <gst/audio/gstaudiosink.h>
#include <roaraudio.h>

#define VERSION "0.0.1"
#define PACKAGE "gst-plugins"
#define GST_PACKAGE_ORIGIN "Unknown package origin"
#define GST_PACKAGE_NAME "GStreamer Plug-ins source release"

G_BEGIN_DECLS

#define GST_TYPE_ROARSINK \
  (gst_roarsink_get_type())
#define GST_ROARSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ROARSINK,GstRoarSink))
#define GST_ROARSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ROARSINK,GstRoarSinkClass))
#define GST_IS_ROARSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ROARSINK))
#define GST_IS_ROARSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ROARSINK))

typedef struct _GstRoarSink GstRoarSink;
typedef struct _GstRoarSinkClass GstRoarSinkClass;

struct _GstRoarSink {
  GstAudioSink   sink;

  struct roar_connection con;
  int       fd;
  gchar    *host;

  guint     rate;
  GstCaps  *cur_caps;
};

struct _GstRoarSinkClass {
  GstAudioSinkClass parent_class;
};

GType gst_roarsink_get_type(void);
gboolean gst_roarsink_factory_init (GstPlugin *plugin);


G_END_DECLS

#endif /* __GST_ESDSINK_H__ */

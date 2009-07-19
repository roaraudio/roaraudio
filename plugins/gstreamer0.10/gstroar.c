//gstroar.c:
/* GStreamer
 * Copyright (C) <1999>      Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2003>      David A. Schleef <ds@schleef.org>
 * Copyright (C) <2008,2009> Philipp 'ph3-der-loewe' Schafft
 *                                    <lion@lion.leolix.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "roarsink.h"
#include "roarmixer.h"

//#include "gst/gst-i18n-plugin.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_roarsink_factory_init (plugin))
    return FALSE;
  if (!gst_roarmixer_factory_init (plugin))
    return FALSE;

//  GST_DEBUG_CATEGORY_INIT (roar_debug, "roar", 0, "RoarAudio elements");

#ifdef ENABLE_NLS
  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif /* ENABLE_NLS */

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "roarsink",
    "RoarAudio Element Plugins",
    plugin_init, VERSION, "GPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)


//ll

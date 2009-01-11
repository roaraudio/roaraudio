//gui.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of the XMMS RoarAudio output plugin a part of RoarAudio,
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "all.h"

// ABOUT:

void roar_about(void) {
 static GtkWidget *dialog;

 if (dialog != NULL)
  return;

 dialog = xmms_show_message(
                _("About RoarAudio Plugin"),
                _("RoarAudio XMMS Plugin..."
                 ), _("OK"), FALSE, NULL, NULL);
 gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
                    GTK_SIGNAL_FUNC(gtk_widget_destroyed),
                    &dialog);
}

// CONFIG:

void roar_configure(void) {
}

//ll

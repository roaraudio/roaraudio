//introspect.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

/** Get information about a sink by its name */
pa_operation* pa_context_get_sink_info_by_name(pa_context *c, const char *name, pa_sink_info_cb_t cb, void *userdata);

/** Get information about a sink by its index */
pa_operation* pa_context_get_sink_info_by_index(pa_context *c, uint32_t id, pa_sink_info_cb_t cb, void *userdata);

/** Get the complete sink list */
pa_operation* pa_context_get_sink_info_list(pa_context *c, pa_sink_info_cb_t cb, void *userdata);

/** Get information about a source by its name */
pa_operation* pa_context_get_source_info_by_name(pa_context *c, const char *name, pa_source_info_cb_t cb, void *userdata);

/** Get information about a source by its index */
pa_operation* pa_context_get_source_info_by_index(pa_context *c, uint32_t id, pa_source_info_cb_t cb, void *userdata);

/** Get the complete source list */
pa_operation* pa_context_get_source_info_list(pa_context *c, pa_source_info_cb_t cb, void *userdata);

/** Get some information about the server */
pa_operation* pa_context_get_server_info(pa_context *c, pa_server_info_cb_t cb, void *userdata) {
 struct roar_stream stream;
 struct roar_client client;
 pa_server_info painfo;

 if ( c == NULL )
  return roar_pa_op_new_done();

 if ( roar_server_oinfo(roar_pa_context_get_con(c), &stream) == -1 )
  return roar_pa_op_new_done();

 if ( roar_get_client(roar_pa_context_get_con(c), &client, 0) == -1 )
  return roar_pa_op_new_done();

 memset(&painfo, 0, sizeof(painfo));

 painfo.user_name           = "(none)";
 painfo.host_name           = pa_context_get_server(c);
 painfo.server_version      = pa_get_library_version();
 painfo.server_name         = "pulseaudio";
 painfo.default_sink_name   = NULL;
 painfo.default_source_name = NULL;
 painfo.cookie              = 0x524F4152;
 painfo.cookie             ^= (client.pid & 0xFF) | (client.uid & 0xFF) << 8 | (client.gid & 0xFF) << 16;

 if ( cb != NULL ) {
  cb(c, &painfo, userdata);
 }

 return roar_pa_op_new_done();
}

/** Get some information about a module by its index */
pa_operation* pa_context_get_module_info(pa_context *c, uint32_t idx, pa_module_info_cb_t cb, void *userdata);

/** Get the complete list of currently loaded modules */
pa_operation* pa_context_get_module_info_list(pa_context *c, pa_module_info_cb_t cb, void *userdata);

/** Get information about a client by its index */
pa_operation* pa_context_get_client_info(pa_context *c, uint32_t idx, pa_client_info_cb_t cb, void *userdata);

/** Get the complete client list */
pa_operation* pa_context_get_client_info_list(pa_context *c, pa_client_info_cb_t cb, void *userdata);

/** Get some information about a sink input by its index */
pa_operation* pa_context_get_sink_input_info(pa_context *c, uint32_t idx, pa_sink_input_info_cb_t cb, void *userdata);

/** Get the complete sink input list */
pa_operation* pa_context_get_sink_input_info_list(pa_context *c, pa_sink_input_info_cb_t cb, void *userdata);

/** Get information about a source output by its index */
pa_operation* pa_context_get_source_output_info(pa_context *c, uint32_t idx, pa_source_output_info_cb_t cb, void *userdata);

/** Get the complete list of source outputs */
pa_operation* pa_context_get_source_output_info_list(pa_context *c, pa_source_output_info_cb_t cb, void *userdata);

/** Set the volume of a sink device specified by its index */
pa_operation* pa_context_set_sink_volume_by_index(pa_context *c, uint32_t idx, const pa_cvolume *volume, pa_context_success_cb_t cb, void *userdata);

/** Set the volume of a sink device specified by its name */
pa_operation* pa_context_set_sink_volume_by_name(pa_context *c, const char *name, const pa_cvolume *volume, pa_context_success_cb_t cb, void *userdata);

/** Set the mute switch of a sink device specified by its index \since 0.8 */
pa_operation* pa_context_set_sink_mute_by_index(pa_context *c, uint32_t idx, int mute, pa_context_success_cb_t cb, void *userdata);

/** Set the mute switch of a sink device specified by its name \since 0.8 */
pa_operation* pa_context_set_sink_mute_by_name(pa_context *c, const char *name, int mute, pa_context_success_cb_t cb, void *userdata);

/** Set the volume of a sink input stream */
pa_operation* pa_context_set_sink_input_volume(pa_context *c, uint32_t idx, const pa_cvolume *volume, pa_context_success_cb_t cb, void *userdata);

/** Set the volume of a source device specified by its index \since 0.8 */
pa_operation* pa_context_set_source_volume_by_index(pa_context *c, uint32_t idx, const pa_cvolume *volume, pa_context_success_cb_t cb, void *userdata);

/** Set the volume of a source device specified by its name \since 0.8 */
pa_operation* pa_context_set_source_volume_by_name(pa_context *c, const char *name, const pa_cvolume *volume, pa_context_success_cb_t cb, void *userdata);

/** Set the mute switch of a source device specified by its index \since 0.8 */
pa_operation* pa_context_set_source_mute_by_index(pa_context *c, uint32_t idx, int mute, pa_context_success_cb_t cb, void *userdata);

/** Set the mute switch of a source device specified by its name \since 0.8 */
pa_operation* pa_context_set_source_mute_by_name(pa_context *c, const char *name, int mute, pa_context_success_cb_t cb, void *userdata);

/** Get daemon memory block statistics */
pa_operation* pa_context_stat(pa_context *c, pa_stat_info_cb_t cb, void *userdata);

/** Get information about a sample by its name */
pa_operation* pa_context_get_sample_info_by_name(pa_context *c, const char *name, pa_sample_info_cb_t cb, void *userdata);

/** Get information about a sample by its index */
pa_operation* pa_context_get_sample_info_by_index(pa_context *c, uint32_t idx, pa_sample_info_cb_t cb, void *userdata);

/** Get the complete list of samples stored in the daemon. */
pa_operation* pa_context_get_sample_info_list(pa_context *c, pa_sample_info_cb_t cb, void *userdata);

/** Kill a client. \since 0.5 */
pa_operation* pa_context_kill_client(pa_context *c, uint32_t idx, pa_context_success_cb_t cb, void *userdata);

/** Kill a sink input. \since 0.5 */
pa_operation* pa_context_kill_sink_input(pa_context *c, uint32_t idx, pa_context_success_cb_t cb, void *userdata);

/** Kill a source output. \since 0.5 */
pa_operation* pa_context_kill_source_output(pa_context *c, uint32_t idx, pa_context_success_cb_t cb, void *userdata);

/** Load a module. \since 0.5 */
pa_operation* pa_context_load_module(pa_context *c, const char*name, const char *argument, pa_context_index_cb_t cb, void *userdata);

/** Unload a module. \since 0.5 */
pa_operation* pa_context_unload_module(pa_context *c, uint32_t idx, pa_context_success_cb_t cb, void *userdata);


/** Get info about a specific autoload entry. \since 0.6 */
pa_operation* pa_context_get_autoload_info_by_name(pa_context *c, const char *name, pa_autoload_type_t type, pa_autoload_info_cb_t cb, void *userdata);

/** Get info about a specific autoload entry. \since 0.6 */
pa_operation* pa_context_get_autoload_info_by_index(pa_context *c, uint32_t idx, pa_autoload_info_cb_t cb, void *userdata);

/** Get the complete list of autoload entries. \since 0.5 */
pa_operation* pa_context_get_autoload_info_list(pa_context *c, pa_autoload_info_cb_t cb, void *userdata);

/** Add a new autoload entry. \since 0.5 */
pa_operation* pa_context_add_autoload(pa_context *c, const char *name, pa_autoload_type_t type, const char *module, const char*argument, pa_context_index_cb_t, void* userdata);

/** Remove an autoload entry. \since 0.6 */
pa_operation* pa_context_remove_autoload_by_name(pa_context *c, const char *name, pa_autoload_type_t type, pa_context_success_cb_t cb, void* userdata);

/** Remove an autoload entry. \since 0.6 */
pa_operation* pa_context_remove_autoload_by_index(pa_context *c, uint32_t idx, pa_context_success_cb_t cb, void* userdata);

/** Move the specified sink input to a different sink. \since 0.9.5 */
pa_operation* pa_context_move_sink_input_by_name(pa_context *c, uint32_t idx, char *sink_name, pa_context_success_cb_t cb, void* userdata);

/** Move the specified sink input to a different sink. \since 0.9.5 */
pa_operation* pa_context_move_sink_input_by_index(pa_context *c, uint32_t idx, uint32_t sink_idx, pa_context_success_cb_t cb, void* userdata);

/** Move the specified source output to a different source. \since 0.9.5 */
pa_operation* pa_context_move_source_output_by_name(pa_context *c, uint32_t idx, char *source_name, pa_context_success_cb_t cb, void* userdata);

/** Move the specified source output to a different source. \since 0.9.5 */
pa_operation* pa_context_move_source_output_by_index(pa_context *c, uint32_t idx, uint32_t source_idx, pa_context_success_cb_t cb, void* userdata);

//ll

//roard.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roard a part of RoarAudio,
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_SUPPORT_LISTEN
char * server[ROAR_MAX_LISTEN_SOCKETS];
#endif

#if defined(ROAR_HAVE_IO_POSIX) && defined(ROAR_HAVE_FS_POSIX)
#define SUPPORT_PIDFILE
char * pidfile = NULL;
#endif

#if defined(ROAR_HAVE_SETGID) || defined(ROAR_HAVE_SETUID)
int    setids    = 0;
#endif

#ifdef ROAR_HAVE_LIBX11
char * x11display = NULL;
#endif

int add_output (char * drv, char * dev, char * opts, int prim, int count);

void dbg_notify_cb(struct roar_notify_core * core, struct roar_event * event, void * userdata) {
 char buf[1024] = "";
 char estr[1024] = "/* ROAR_??? */";
 char ttstr[1024] = "/* ROAR_OT_??? */";
 const char * ttname;
 uint32_t ev = ROAR_EVENT_GET_TYPE(event);
 int i;

 ttname = roar_ot2str(event->target_type);
 if ( ttname != NULL ) {
  snprintf(ttstr, sizeof(ttstr)-1, "/* ROAR_OT_%s */", ttname);
  buf[sizeof(ttstr)-1] = 0;
  for (i = 0; ttstr[i] != 0; i++)
   if ( islower(ttstr[i]) )
    ttstr[i] = toupper(ttstr[i]);
 }

 if ( ev == ROAR_NOTIFY_SPECIAL ) {
  snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_SPECIAL */");
 } else if ( ROAR_NOTIFY_IS_CMD(ev) ) {
  if ( command_get_name(ROAR_NOTIFY_EVENT2CMD(ev), &ttname) == -1 ) {
   snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_CMD2EVENT(%i) */", ROAR_NOTIFY_EVENT2CMD(ev));
  } else {
   snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_CMD2EVENT(ROAR_CMD_%s) */", ttname);
   for (i = 0; estr[i] != 0; i++)
    if ( islower(estr[i]) )
     estr[i] = toupper(estr[i]);
  }
 } else if ( ROAR_NOTIFY_IS_EGRP(ev) ) {
  snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_EGRP2EVENT(%i) */", ROAR_NOTIFY_EVENT2EGRP(ev));
 } else if ( ROAR_NOTIFY_IS_OE(ev) ) {
  switch (ev) {
   // OE basics:
   case ROAR_OE_BASICS_CHANGE_STATE:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_BASICS_CHANGE_STATE */");
    break;
   case ROAR_OE_BASICS_CHANGE_FLAGS:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_BASICS_CHANGE_FLAGS */");
    break;
   case ROAR_OE_BASICS_NEW:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_BASICS_NEW */");
    break;
   case ROAR_OE_BASICS_DELETE:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_BASICS_DELETE */");
    break;
   // OE Streams:
   case ROAR_OE_STREAM_CHANGE_VOLUME:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_STREAM_CHANGE_VOLUME */");
    break;
   case ROAR_OE_STREAM_XRUN:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_OE_STREAM_XRUN */");
    break;
   // OE Default:
   default:
     snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_OE2EVENT(%i) */", ROAR_NOTIFY_EVENT2OE(ev));
    break;
  }
 } else if ( ROAR_NOTIFY_IS_USER(ev) ) {
  snprintf(estr, sizeof(estr)-1, "/* ROAR_NOTIFY_USER2EVENT(%i) */", ROAR_NOTIFY_EVENT2USER(ev));
 }

 buf[sizeof(estr)-1] = 0;

 if ( event->flags & ROAR_EVENT_FLAG_PROXYEVENT ) {
  snprintf(buf, sizeof(buf)-1, ".event_proxy=0x%.8x%s, ", (int)event->event_proxy, estr);
  buf[sizeof(buf)-1] = 0;
 }

 ROAR_INFO("dbg_notify_cb(core=%p, event=%p{.flags=0x%.8x, event=0x%.8x%s, %s.emitter=%i, .target=%i, .target_type=%i%s, .arg0=%i, .arg1=%i, .arg2=%p}, userdata=%p) = (void)",
           0 /* always print: minlevel = 0 */,
           core, event,
           (int)event->flags,
           (int)event->event,
           (event->flags & ROAR_EVENT_FLAG_PROXYEVENT ? "" : estr),
           buf,
           event->emitter,
           event->target,
           event->target_type,
           ttstr,
           event->arg0,
           event->arg1,
           event->arg2,
           userdata);
}

void dbg_notify_cb_register (void) {
 struct roar_event event;

 memset(&event, 0, sizeof(event));

 event.event = ROAR_EGRP_ANY_EVENT;

 event.emitter = -1;
 event.target = -1;
 event.target_type = -1;

 roar_notify_core_subscribe(NULL, &event, dbg_notify_cb, NULL);
}

#ifdef ROAR_HAVE_MAIN_ARGS
void usage (void) {
 printf("Usage: roard [OPTIONS]...\n\n");

 printf("Misc Options:\n\n");
 printf(
        " --daemon              - Bring the server into background after init\n"
        " --verbose             - Be more verbose, can be used multiple times\n"
        " --terminate           - Terminate after last client quited\n"
        " --start               - No op parameter (starting roard is default operation)\n"
        " --restart             - Trys to stop an old instance and start a new with new settings\n"
        " --stop                - Stops a running roard (provide --pidfile!)\n"
        " --shutdown            - Terminates a running roard (provide --pidfile!)\n"
        " --realtime            - Trys to get realtime priority,\n"
        "                         give multible times for being more realtime\n"
        " --memlock LEVEL       - Set default memory locking level to LEVEL\n"
        " --chroot DIR          - chroots to the given dir\n"
        " --setgid              - GroupID to the audio group as specified via -G\n"
        " --setuid              - UserID to the audio user as specified via -U\n"
        " --sysclocksync        - calculate exact sample rate using the system clock\n"
        " --location  LOC       - Set lion readable location of server\n"
        " --description  DESC   - Set lion readable description of server\n"
#ifdef SUPPORT_PIDFILE
        " --pidfile PIDFILE     - Write a pidfile at PIDFILE\n"
#endif
#ifdef ROAR_HAVE_SYSLOG
        " --log-syslog          - Log Warnings, Errors, ... to syslog\n"
#endif
       );

 printf("\nPlugin Options:\n\n");
 printf(
        " --plugin-load FILE    - Load plugin FILE\n"
       );

 printf("\nAudio Options:\n\n");
 printf(
        " -R  --rate   RATE     - Set server rate\n"
        " -B  --bits   BITS     - Set server bits\n"
        " -C  --chans  CHANNELS - Set server channels\n"
        " --aiprofile  PROFILE  - Use the given audio profile\n"
       );

 printf("\nStream Options:\n\n");
 printf(
        " --stream-flags D=F    - Set default flags for stream directions\n"
        "                         D is the stream direction and F is a comma separated\n"
        "                         list of flags in form +flag or -flag to set or unset\n"
        "                         a flag as default or remove it from the default\n"
       );

 printf("\nOutput Options:\n\n");
 printf(" -o  --odriver DRV     - Set the driver, use '--list-driver' to get a list\n");
 printf(" -O  --odevice DEV     - Set the device\n");
 printf(" -oO OPTS              - Set output options\n");
 printf(" -oN                   - Adds another output\n");
 printf(" -oP                   - Mark output as primary\n");
#ifdef ROAR_DRIVER_DEFAULT
 printf(" --list-driver         - List all drivers (default driver: %s)\n", ROAR_DRIVER_DEFAULT);
#else
 printf(" --list-driver         - List all drivers (default driver: (autodetect))\n");
#endif

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 printf("\nSource Options:\n\n");
 printf(" -s  --source DRV      - Use DRV as input driver\n"
        " -S           DEV      - Use DEV as input device\n"
        " -sO          OPTS     - Use OPTS as input options\n"
        " -sN                   - Adds another source\n"
        " -sP                   - Make souce as primary\n"
       );
 printf(" --list-sources        - List all sources\n");
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIXER
 printf("\nHardware Mixer Options:\n\n");
 printf(" -m  --mixer  DRV      - Use DRV as mixer driver\n"
        " -M           DEV      - Use DEV as mixer device\n"
        " -mO          OPTS     - Use OPTS as mixer options\n"
        " -mN                   - Adds another mixer\n"
        " -mP                   - Make mixer as primary\n"
       );
 printf(" --list-mixers         - List all mixers\n");
#endif

 printf("\nCodec Filter Options:\n\n");
 printf(" --list-cf             - List all codec filter\n"
       );

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 printf("\nMIDI Options:\n\n");
 printf(" --midi-no-console     - Disable console based MIDI synth\n"
        " --midi-console-enable - Enables the console based MIDI synth\n"
        " --midi-console DEV    - Set device for MIDI console\n"
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
        " --ssynth-enable       - Enable simple software synth\n"
        " --ssynth-disable      - Disable simple software synth\n"
#endif
       );
#endif

#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 printf("\nLight Control Options:\n\n");
 printf(" --light-channels NUM  - Sets the number of channels for Light control (default: %i)\n",
                                  LIGHT_CHANNELS_DEFAULT
       );
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 printf("\nRadio Data and Transmitter Control System Options:\n\n");
 printf(" --rds-pi   PI         - Sets the RDS Programme Identification (PI)\n"
        " --rds-ps   PS         - Sets the RDS Programme Service Name (PS)\n"
        " --rds-pty  PTY        - Sets the RDS Programme Type (PTY)\n"
        " --rds-tp              - Sets the RDS Traffic Programme (TP) flag\n"
        " --rds-ct              - Enables sending of RDS Clock Time (CT)\n"
       );
#endif

#ifdef ROAR_HAVE_LIBX11
 printf("\nX11 Options:\n\n");
 printf(
        " --x11-display DISPLAY - Set display for X11\n"
        " --display DISPLAY     - Set display for X11\n"
       );
#endif

 printf("\nServer Options:\n\n");
 printf(" -t  --tcp             - Use TCP listen socket\n"
#ifdef ROAR_HAVE_UNIX
        " -u  --unix            - Use UNIX Domain listen socket (default)\n"
#endif
#ifdef ROAR_HAVE_LIBDNET
        " -n  --decnet          - use DECnet listen socket\n"
#endif
        " -4                    - Use IPv4 connections (implies -t)\n"
#ifdef AF_INET6
        " -6                    - Use IPv6 connections (implies -t)\n"
#endif
#ifdef IPV6_ADDRFORM
        " -64                   - Try to downgrade sockets from IPv6 into IPv4,\n"
        "                         this is normaly not useful.\n"
#endif
        " -p  --port            - TCP Port to bind to\n"
        " -b  --bind            - IP/Hostname to bind to\n"
        "     --sock            - Filename for UNIX Domain Socket\n"
        "     --proto PROTO     - Use PROTO as protocol on Socket\n"
        "     --proto-dir DIR   - Set direction parameter for protocol\n"
        "     --proto-rate RATE - Set sample rate parameter for protocol\n"
        "     --proto-bits BITS - Set bits per sample parameter for protocol\n"
        "     --proto-codec E   - Set codec parameter for protocol\n"
        "     --proto-chans C   - Set number of channels parameter for protocol\n"
        "     --proto-aiprofile PROFILE\n"
        "                       - Sets the audio profile for socket\n" 
        "     --proto-profile P - Set profile for listen socket\n"
        "     --list-proto      - List supported protocols\n"
        "     --list-profiles   - List supported profiles for --proto-profile\n"
        "     --new-sock        - Parameters for new socket follows\n"
#ifdef ROAR_HAVE_LIBSLP
        "     --slp             - Enable OpenSLP support\n"
#endif
#ifdef ROAR_HAVE_LIBX11
        "     --x11             - Enable X11 support\n"
#endif
        " --jumbo-mtu MTU       - Sets the MTU for Jumbo Packets\n"
        " -G  GROUP             - Sets the group for the UNIX Domain Socket, (default: %s)\n"
        "                         You need the permissions to change the GID\n"
        " -U  USER              - Sets the user for the UNIX Domain Socket, (default: do not set)\n"
        "                         You need the permissions to change the UID (normaly only root has)\n"
        " --no-listen           - Do not listen for new clients\n"
        "                         (only useful for relaing, impleys --terminate)\n"
        " --client-fh           - Comunicate with a client over this handle\n"
        "                         (only useful for relaing)\n"
        " --close-fh            - Closes the given fh\n"
        " --standby             - Start in standby state\n"
        " --auto-standby        - Automatical goes into standby if there are no streams\n",
#ifdef ROAR_DEFAULT_SOCKGRP
        ROAR_DEFAULT_SOCKGRP
#else
        "(none)"
#endif
       );
// printf("\n Options:\n\n");
 printf("\n");
}

static void list_proto (void) {
 printf("  Protocol Flag Subsys - Description\n");
 printf("------------------------------------------------------\n");
 printf("  roar          WM LRX - RoarAudio native protocol\n");
#if !defined(ROAR_WITHOUT_DCOMP_EMUL_ESD) && defined(ROAR_HAVE_H_ESD)
 printf("  esd           W      - EsounD emulation\n");
#endif
#ifndef ROAR_WITHOUT_DCOMP_EMUL_SIMPLE
 printf("  simple        WM LRX - PulseAudio simple protocol\n");
#endif
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND
 printf("  rsound        W      - RSound emulation\n");
#endif
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RPLAY
 printf("  rplay         W      - RPlay emulation\n");
#endif
}

#endif

#define _pmsg(format, args...) roar_debug_msg(type, __LINE__, __FILE__, ROAR_DBG_PREFIX, format, ## args)
#define _pmsgc(group, counter, name) _pmsg("  Counter %-10s: %llu", (name), (long long unsigned int)counters_get(group, counter))
void counters_print(int type, int force) {
 if ( type != ROAR_DEBUG_TYPE_INFO || force || (ROAR_DBG_INFOVAR) >= ROAR_DBG_INFO_INFO ) {
  _pmsg("--- Counter Listing ---");
  _pmsg(" Current:");
  _pmsgc(cur, clients, "Clients");
  _pmsgc(cur, streams, "Streams");
  _pmsg(" Total:");
  _pmsgc(sum, clients, "Clients");
  _pmsgc(sum, streams, "Streams");
  _pmsg("--- End of Counter Listing ---");
 }
}
#undef _pmsgc
#undef _pmsg

int restart_server (char * server, int terminate) {
 struct roar_connection con;
#ifdef ROAR_HAVE_KILL
 char buf[80];
 ssize_t l;
 struct roar_vio_calls fh;
 pid_t pid;
 int ok;

 ROAR_INFO("restart_server(server='%s', terminate=%i): trying to restart server", ROAR_DBG_INFO_INFO, server, terminate);

 if ( pidfile != NULL ) {
  if ( roar_vio_open_file(&fh, pidfile, O_RDONLY, 0644) == -1 ) {
   ROAR_WARN("restart_server(*): Can not read pidfile: %s", pidfile);
  } else {
   l = roar_vio_read(&fh, buf, 80);
   roar_vio_close(&fh);
   if ( l > 0 ) {
    buf[l-1] = 0;
    buf[79]  = 0;
    pid = atoi(buf);
    if ( terminate ) {
     ok = kill(pid, SIGUSR1);
    } else {
     ok = kill(pid, SIGINT);
    }
    if ( ok == 0 ) {
     return 0;
    } else {
     ROAR_WARN("restart_server(*): Can not kill roard by pidfile");
    }
   } else {
    ROAR_WARN("restart_server(*): Can not find a PID in the pidfile");
   }
  }
 }
#endif

 if ( roar_connect(&con, server) == -1 ) {
  return -1;
 }

 if ( roar_terminate(&con, terminate) == -1 ) {
  return -1;
 }

 return roar_disconnect(&con);
}

#define R_SETUID 1
#define R_SETGID 2

int init_config (void) {
 int i;

 memset(g_config, 0, sizeof(struct roard_config));

 for (i = 0; i < ROAR_DIR_DIRIDS; i++) {
  g_config->streams[i].mixer_channels = 1;
  g_config->streams[i].mixer.rpg_mul  = 1;
  g_config->streams[i].mixer.rpg_div  = 1;
  g_config->streams[i].mixer.scale    = 65535;
  g_config->streams[i].mixer.mixer[0] = g_config->streams[i].mixer.scale;
 }

 g_config->streams[ROAR_DIR_PLAY    ].flags = ROAR_FLAG_META;
 g_config->streams[ROAR_DIR_OUTPUT  ].flags = ROAR_FLAG_PASSMIXER;
 g_config->streams[ROAR_DIR_FILTER  ].flags = ROAR_FLAG_SYNC;
 g_config->streams[ROAR_DIR_MIDI_OUT].flags = ROAR_FLAG_SYNC;
 g_config->streams[ROAR_DIR_BIDIR   ].flags = ROAR_FLAG_ANTIECHO;

 g_config->location    = CONF_DEF_STRING;
 g_config->description = CONF_DEF_STRING;

 g_config->memlock_level = -1;

 return 0;
}

#ifdef ROAR_SUPPORT_LISTEN
int init_listening (void) {
 int i;

 memset(g_listen, 0, sizeof(g_listen));

 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  g_listen[i].proto  = ROAR_PROTO_ROARAUDIO;
  server[i]          = NULL;
 }

 return 0;
}

int get_listen(struct roard_listen ** sock, char *** sockname) {
 int i;

 if ( sock == NULL )
  return -1;

 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  if ( ! g_listen[i].used ) {
   server[i] = NULL;
   *sock = &(g_listen[i]);

   if ( sockname != NULL )
    *sockname = &(server[i]);

   return 0;
  }
 }

 return -1;
}

#ifdef ROAR_SUPPORT_LISTEN
static struct _listen_profile {
 const char * name;
 int          type;
 int          port;
 const char * sockaddr;
 int          proto;
 int          dir;
 const char * aiprofile;
 const char * desc;
} _g_listen_profiles[] = {
 // RoarAudio:
#ifdef ROAR_HAVE_UNIX
 {"roar-usock",     ROAR_SOCKET_TYPE_UNIX,   0,                 "~/" ROAR_DEFAULT_SOCK_USER,
                    ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio default user profile"},
 {"roar-gsock",     ROAR_SOCKET_TYPE_UNIX,   0,                 ROAR_DEFAULT_SOCK_GLOBAL,
                    ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio default global profile"},
#endif
#ifdef ROAR_HAVE_IPV4
 {"roar-tcp",       ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_PORT, ROAR_DEFAULT_INET4_HOST,
                    ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio local TCP profile"},
 {"roar-tcp-pub",   ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio network TCP profile"},
#endif
#ifdef ROAR_HAVE_LIBDNET
 {"roar-dnet",      ROAR_SOCKET_TYPE_DECNET, 0,                 ROAR_DEFAULT_LISTEN_OBJECT,
                    ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio default DECnet"},
#endif
#ifdef ROAR_HAVE_UNIX
 {"roar-abstract",  ROAR_SOCKET_TYPE_UNIX,   0,                 "+abstract",        ROAR_PROTO_ROARAUDIO, -1, NULL,
                    "RoarAudio abstract namespace profile"},
#endif

 // EsounD:
#if !defined(ROAR_WITHOUT_DCOMP_EMUL_ESD) && defined(ROAR_HAVE_H_ESD)
#ifdef ROAR_HAVE_UNIX
 {"esd-unix",       ROAR_SOCKET_TYPE_UNIX,   0,                 ROAR_DEFAULT_ESD_GSOCK,
                    ROAR_PROTO_ESOUND,    -1, NULL,
                    "EsounD default local profile"},
#endif
#ifdef ROAR_HAVE_IPV4
 {"esd-tcp",        ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_ESD_PORT, ROAR_NET_INET4_LOCALHOST,
                    ROAR_PROTO_ESOUND,    -1, NULL,
                    "EsounD local TCP profile"},
 {"esd-tcp-pub",    ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_ESD_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_ESOUND,    -1, NULL,
                    "EsounD network TCP profile"},
#endif
#endif

 // RSound:
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND
#ifdef ROAR_HAVE_UNIX
 {"rsound-unix",    ROAR_SOCKET_TYPE_UNIX,   0,                 ROAR_DEFAULT_RSOUND_GSOCK,
                    ROAR_PROTO_RSOUND,    -1, NULL,
                    "RSound default local profile"},
#endif
#ifdef ROAR_HAVE_IPV4
 {"rsound-tcp",     ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_RSOUND_PORT, ROAR_NET_INET4_LOCALHOST,
                    ROAR_PROTO_RSOUND,    -1, NULL,
                    "RSound local TCP profile"},
 {"rsound-tcp-pub", ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_RSOUND_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_RSOUND,    -1, NULL,
                    "RSound network TCP profile"},
#endif
#ifdef ROAR_HAVE_LIBDNET
 {"rsound-dnet",    ROAR_SOCKET_TYPE_DECNET, 0,                 ROAR_DEFAULT_RSOUND_OBJECT,
                    ROAR_PROTO_RSOUND,    -1, NULL,
                    "RSound DECnet profile"},
#endif
#endif

 // PulseAudio Simple:
#ifndef ROAR_WITHOUT_DCOMP_EMUL_SIMPLE
#ifdef ROAR_HAVE_IPV4
 {"pas-play-tcp",   ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_PA_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_SIMPLE, ROAR_DIR_PLAY, "default",
                    "PulseAudio Simple TCP play profile"},
 {"pas-mon-tcp",    ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_PA_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_SIMPLE, ROAR_DIR_MONITOR, "default",
                    "PulseAudio Simple TCP monitor profile"},
#endif
#endif

 // RPlay:
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RPLAY
#ifdef ROAR_HAVE_IPV4
 {"rplay-tcp",      ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_RPLAY_PORT, ROAR_NET_INET4_LOCALHOST,
                    ROAR_PROTO_RPLAY,     -1, NULL,
                    "RPlay local TCP profile"},
 {"rplay-tcp-pub",  ROAR_SOCKET_TYPE_TCP,    ROAR_DEFAULT_RPLAY_PORT, ROAR_NET_INET4_ANYHOST,
                    ROAR_PROTO_RPLAY,     -1, NULL,
                    "RPlay network TCP profile"},
#endif
#endif

 // End of List:
 {NULL, -1, -1, NULL, -1, -1, NULL, NULL}
};

void listen_listen_profiles (void) {
 struct _listen_profile * p;
 char * type;
 int i;
 char port[8];

 printf("Name           Type    Address          Port    Protocol  Dir        Audio Profile - Description\n");
 printf("------------------------------------------------------------------------------------------------\n");
      //roar-tcp-pub 0.0.0.0       16002   RoarAudio unknown    (null) - (null)

 for (i = 0; (p = &(_g_listen_profiles[i]))->name != NULL; i++) {
  switch (p->type) {
   case ROAR_SOCKET_TYPE_UNIX:   type = "UNIX";   break;
   case ROAR_SOCKET_TYPE_TCP:    type = "TCP";    break;
   case ROAR_SOCKET_TYPE_DECNET: type = "DECnet"; break;
   default:
     type = "unknown";
    break;
  }

  if ( p->port ) {
   snprintf(port, sizeof(port)-1, "%i", p->port);
   port[sizeof(port)-1] = 0;
  } else {
   strcpy(port, "(none)");
  }

  printf("%-14s %-7s %-16s %-7s %-9s %-10s %-13s - %s\n",
           p->name,
           type,
           p->sockaddr, port,
           roar_proto2str(p->proto),
           p->dir == -1 ? "(none)" : roar_dir2str(p->dir), p->aiprofile == NULL ? "(none)" : p->aiprofile,
           p->desc == NULL ? "" : p->desc);
 }
}

int get_listen_profile (const char * name,
                        int * port, char ** sockaddr, int * type,
                        int * proto,
                        int * dir, struct roar_audio_info * info) {
 static char buf[1024];
 struct _listen_profile * p;
 int i;

 for (i = 0; (p = &(_g_listen_profiles[i]))->name != NULL; i++) {
  if ( !strcasecmp(p->name, name) ) {
   *port     = p->port;

   if ( p->type == ROAR_SOCKET_TYPE_UNIX && p->sockaddr[0] != '+' ) {
    roar_env_render_path_r(buf, sizeof(buf), p->sockaddr);
   } else {
    strncpy(buf, p->sockaddr, sizeof(buf));
   }
   *sockaddr = buf;

   *proto    = p->proto;
   *dir      = p->dir;

   if ( p->aiprofile != NULL ) {
    if ( roar_profile2info(info, p->aiprofile) == -1 ) {
     ROAR_ERR("Unknown audio profile: %s", p->aiprofile);
     return -1;
    }
   }
   return 0;
  }
 }

 return -1;
}
#endif

int add_listen (char * addr, int port, int sock_type, char * user, char * group, int proto, int dir, struct roar_audio_info * info) {
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 struct group   * grp  = NULL;
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 struct passwd  * pwd  = NULL;
#endif
#ifdef ROAR_HAVE_UNIX
 char * env_roar_proxy_backup;
#endif
 int    sockid = -1;
#ifdef ROAR_HAVE_UNIX
 int    sock;
#endif
 int    i;

 ROAR_INFO("add_listen(addr='%s', port=%i, sock_type=%i, user='%s', group='%s', proto=%s(%i), dir=%s(%i), info={.rate=%u, .bits=%u, .channels=%u, .codec=%s(%u)}): trying to add listen socket",
            ROAR_DBG_INFO_INFO,
            addr, port, sock_type, user, group, roar_proto2str(proto), proto, roar_dir2str(dir), dir,
            info->rate, info->bits, info->channels, roar_codec2str(info->codec), info->codec);

 if ( *addr != 0 ) {
  for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
   if ( ! g_listen[i].used ) {
    sockid = i;
    break;
   }
  }

  if ( sockid == -1 )
   return -1;

  g_listen[sockid].proto = proto;

  ROAR_DBG("add_listen(*): proto=0x%.4x", proto);

  if ( roar_vio_open_socket_listen(&(g_listen[sockid].sock), sock_type, addr, port) == -1 ) {
#ifdef ROAR_HAVE_UNIX
   if ( *addr == '/' ) {
    if ( (env_roar_proxy_backup = getenv("ROAR_PROXY")) != NULL ) {
     env_roar_proxy_backup = strdup(env_roar_proxy_backup);
     unsetenv("ROAR_PROXY");
    }
    if ( (sock = roar_socket_connect(addr, port)) != -1 ) {
     close(sock);
     ROAR_ERR("Can not open listen socket: Socket allready in use");
     return 1;
    } else {
     unlink(addr);
     if ( roar_vio_open_socket_listen(&(g_listen[sockid].sock), sock_type, addr, port) == -1 ) {
      ROAR_ERR("Can not open listen socket: %s", strerror(errno));
      return 1;
     }
    }
    if ( env_roar_proxy_backup != NULL ) {
     setenv("ROAR_PROXY", env_roar_proxy_backup, 0);
     free(env_roar_proxy_backup);
    }
#else
   if (0) { // noop
#endif
   } else {
    ROAR_ERR("Can not open listen socket: %s", strerror(errno));
    return 1;
   }
  }

#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
  if ( group != NULL ) {
   if ( (grp = getgrnam(group)) == NULL ) {
    ROAR_ERR("Can not get GID for group %s: %s", group, strerror(errno));
   }
  }
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
  if ( user ) {
   if ( (pwd = getpwnam(user)) == NULL ) {
    ROAR_ERR("Can not get UID for user %s: %s", user, strerror(errno));
   }
  }
#endif

#if defined(ROAR_HAVE_IO_POSIX) && defined(ROAR_HAVE_UNIX)
  if ( *addr == '/' ) {
   if ( grp || pwd ) {
     if ( chown(addr, pwd ? pwd->pw_uid : -1, grp ? grp->gr_gid : -1) == -1 )
      return 1;
   }
#ifdef ROAR_HAVE_GETUID
   if ( grp ) {
    if ( getuid() == 0 )
     if ( chmod(addr, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1 )
      return 1;
   }
#endif
  }
#endif
 }

 // in case we opened the listening socket correctly.
 if ( dir == -1 )
  dir = ROAR_DIR_PLAY;

 g_listen[sockid].inst.stpl.dir = dir;
 memcpy(&(g_listen[sockid].inst.stpl.info), info, sizeof(struct roar_audio_info));

 switch (dir) {
  case ROAR_DIR_PLAY:
  case ROAR_DIR_RECORD:
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_FILTER:
  case ROAR_DIR_BIDIR:
    if ( !g_listen[sockid].inst.stpl.info.rate )
     g_listen[sockid].inst.stpl.info.rate = g_sa->rate;

    if ( !g_listen[sockid].inst.stpl.info.bits )
     g_listen[sockid].inst.stpl.info.bits = g_sa->bits;

    if ( !g_listen[sockid].inst.stpl.info.channels )
     g_listen[sockid].inst.stpl.info.channels = g_sa->channels;

    if ( !g_listen[sockid].inst.stpl.info.codec )
     g_listen[sockid].inst.stpl.info.codec = g_sa->codec;
   break;
 }

 g_listen[sockid].used = 1;
 server[sockid]        = addr;

 return 0;
}
#endif

int update_stream_flags (char * str) {
 int    dir;
 char * flags;
 char * k;
 int    op;
 int    flag;

 if ( (flags = strstr(str, "=")) == NULL )
  return -1;

 *flags = 0;
  flags++;

 if ( (dir = roar_str2dir(str)) == -1 )
  return -1;

 while (flags != NULL) {
  k = flags;
  flags = strstr(flags, ",");

  if ( flags != NULL )
   *(flags++) = 0;

  switch (*k) {
   case '+': k++; op = ROAR_SET_FLAG;   break;
   case '-': k++; op = ROAR_RESET_FLAG; break;
   default:
     op = ROAR_SET_FLAG;
  }

  flag = 0;

  if ( !strcmp(k, "sync") ) {
   flag = ROAR_FLAG_SYNC;
  } else if ( !strcmp(k, "meta") ) {
   flag = ROAR_FLAG_META;
  } else if ( !strcmp(k, "cleanmeta") ) {
   flag = ROAR_FLAG_CLEANMETA;
  } else if ( !strcmp(k, "pause") ) {
   flag = ROAR_FLAG_PAUSE;
  } else if ( !strcmp(k, "mute") ) {
   flag = ROAR_FLAG_MUTE;
  } else if ( !strcmp(k, "antiecho") ) {
   flag = ROAR_FLAG_ANTIECHO;
  } else if ( !strcmp(k, "passmixer") ) {
   flag = ROAR_FLAG_PASSMIXER;
  } else {
   return -1;
  }

  g_config->streams[dir].flags |= flag;

  if ( op == ROAR_RESET_FLAG )
   g_config->streams[dir].flags -= flag;
 }

 return 0;
}

#ifdef ROAR_DRIVER_DEFAULT
#define add_default_output add_output
#else
int add_default_output (char * drv, char * dev, char * opts, int prim, int count) {
 char * drvs[] = {
  // operating system depended things:
#ifdef __OpenBSD__
  /* OpenBSD use sndio natively, this check is discusses with upstream (See ML archive August 2010) */
  "sndio",
#endif
  // native and pseudo-native interfaces:
  "oss", "alsa", "sndio", "wmm",
  // sound libs:
  "ao", "portaudio",
  // other sound systems:
  "esd", "rsound", "pulsesimple", "roar",
  // specal buildins:
  "sysclock", "null",
  // terminator:
  NULL
 };
 int i;
 int ret;
 int _alive;

 if ( drv != NULL )
  return add_output(drv, dev, opts, prim, count);

 if ( dev != NULL ) {
  ROAR_WARN("add_default_output(drv=(none), dev='%s', opts='%s', prim=%i, count=%i): It's not recommended to use device name without driver name.", dev, opts, prim, count);
 }

 for (i = 0; drvs[i] != NULL; i++) {
  ROAR_INFO("add_default_output(*): trying driver %s", ROAR_DBG_INFO_INFO, drvs[i]);
  _alive = alive; // save global alive setting

  ret = add_output(drvs[i], dev, opts, prim, count);
  if ( ret != -1 )
   return ret;

  alive = _alive; // restore global alive setting
  ROAR_INFO("add_default_output(*): Driver %s faild to load", ROAR_DBG_INFO_VERBOSE, drvs[i]);
 }

 return -1;
}
#endif

int add_output (char * drv, char * dev, char * opts, int prim, int count) {
 int stream;
 struct roar_stream * s;
 struct roar_stream_server * ss;
 char * k, * v;
#ifdef ROAR_DRIVER_CODEC
 char * to_free = NULL;
#endif
 int sync = 0, f_mmap = 0;
 int32_t blocks = -1, blocksize = -1;
 int dir = ROAR_DIR_OUTPUT;
 int error = 0;
 // DMX:
 int32_t channel  = -1;
 int32_t universe = -1;
 uint16_t tu16;
 float q = -32e6;

 ROAR_INFO("add_output(drv='%s', dev='%s', opts='%s', prim=%i, count=%i): trying to add output driver", ROAR_DBG_INFO_INFO, drv, dev, opts, prim, count);

 if ( drv == NULL && count == 0 ) {
#ifdef ROAR_DRIVER_DEFAULT
  drv  = ROAR_DRIVER_DEFAULT;
  prim = 1;
  sync = 1;

#ifdef ROAR_DRIVER_CODEC
  if ( opts == NULL ) {
   opts = to_free = strdup("codec=" ROAR_DRIVER_CODEC);
  }
#endif
#else
  ROAR_ERR("add_output(*): Can not find default driver");
  return -1;
#endif
 }

 if ( opts == NULL && count == 0 ) {
  sync = 1;
  prim = 1; // if ( prim == 0 ) prim = 1; -> prim allways = 1
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 if ( (stream = streams_new()) == -1 ) {
  ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = -1", drv, dev, opts);
  if ( prim ) alive = 0;
  return -1;
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 streams_get(stream, &ss);
 s = ROAR_STREAM(ss);

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 memset(&(s->info), 0xFF, sizeof(struct roar_audio_info)); // set everything to -1

 s->pos_rel_id = -1;
// s->info.codec = codec;

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 if ( opts == NULL ) {
  k = NULL;
 } else {
  k = strtok(opts, ",");
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s'): initial k='%s'(%p)", drv, dev, opts, k, k);

 while (k != NULL) {
//  ROAR_WARN("add_output(*): opts: %s", k);

  if ( (v = strstr(k, "=")) != NULL ) {
   *v++ = 0;
  }

  ROAR_DBG("add_output(*): opts: k='%s', v='%s'", k, v);
  if ( strcmp(k, "rate") == 0 ) {
   s->info.rate = atoi(v);
  } else if ( strcmp(k, "channels") == 0 ) {
   s->info.channels = atoi(v);
  } else if ( strcmp(k, "bits") == 0 ) {
   s->info.bits = atoi(v);
  } else if ( strcmp(k, "codec") == 0 ) {
   if ( (s->info.codec = roar_str2codec(v)) == -1 ) {
    ROAR_ERR("add_output(*): unknown codec '%s'", v);
    error++;
   }
  } else if ( strcmp(k, "q") == 0 ) {
   q = atof(v);
  } else if ( strcmp(k, "blocks") == 0 ) {
   blocks = atoi(v);
  } else if ( strcmp(k, "blocksize") == 0 ) {
   blocksize = atoi(v);
  } else if ( strcmp(k, "mmap") == 0 ) {
   f_mmap = 1;
  } else if ( strcmp(k, "subsystem") == 0 ) {
   if ( !strcasecmp(v, "wave") || !strcasecmp(v, "waveform") ) {
    dir = ROAR_DIR_OUTPUT;
#ifndef ROAR_WITHOUT_DCOMP_MIDI
   } else if ( !strcasecmp(v, "midi") ) {
    dir = ROAR_DIR_MIDI_OUT;
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
   } else if ( !strcasecmp(v, "light") ) {
    dir = ROAR_DIR_LIGHT_OUT;
#endif
#ifndef ROAR_WITHOUT_DCOMP_RAW
   } else if ( !strcasecmp(v, "raw") ) {
    dir = ROAR_DIR_RAW_OUT;
#endif
   } else if ( !strcasecmp(v, "complex") ) {
    dir = ROAR_DIR_COMPLEX_OUT;
   } else {
    ROAR_ERR("add_output(*): unknown/unsupported subsystem '%s'", k);
    error++;
   }
  // DMX:
  } else if ( strcmp(k, "channel") == 0 ) {
   channel  = atoi(v);
   if ( channel < 0 || channel > 65535 ) {
    ROAR_ERR("add_output(*): Invalide channel (not within 0..65535): %i", channel);
    channel = -1;
    error++;
   }
  } else if ( strcmp(k, "universe") == 0 ) {
   universe = atoi(v);
   if ( universe < 0 || universe > 255 ) {
    ROAR_ERR("add_output(*): Invalide universe (not within 0..255): %i", universe);
    universe = -1;
    error++;
   }

  } else if ( strcmp(k, "name") == 0 ) {
   if ( streams_set_name(stream, v) == -1 ) {
    ROAR_ERR("add_output(*): Can not set Stream name");
    error++;
   }

  } else if ( strcmp(k, "meta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_META);
  } else if ( strcmp(k, "sync") == 0 ) {
   sync = 1;
  } else if ( strcmp(k, "primary") == 0 ) {
   prim = 1;

  } else if ( strcmp(k, "cleanmeta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_CLEANMETA);
  } else if ( strcmp(k, "autoconf") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_AUTOCONF);
  } else if ( strcmp(k, "recsource") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_RECSOURCE);
  } else if ( strcmp(k, "passmixer") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_PASSMIXER);
  } else {
   ROAR_ERR("add_output(*): unknown option '%s'", k);
   error++;
  }

  if ( error ) {
   streams_delete(stream);
   if ( prim ) alive = 0;
#ifdef ROAR_DRIVER_CODEC
   if ( to_free != NULL )
    free(to_free);
#endif
   return -1;
  }

  k = strtok(NULL, ",");
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 // set audio info...
 switch (dir) {
  case ROAR_DIR_LIGHT_OUT:
    switch (s->info.codec) {
     case ROAR_CODEC_DMX512:
     case -1:
       if ( s->info.rate == -1 )
        s->info.rate = ROAR_OUTPUT_CFREQ;

       s->info.channels =   0;
       s->info.bits     =   8;
       s->info.codec    = ROAR_CODEC_DMX512; // in case codec == -1
      break;
    }
   break;
  case ROAR_DIR_MIDI_OUT:
    switch (s->info.codec) {
     case ROAR_CODEC_MIDI:
     case -1:
       if ( s->info.rate == -1 )
        s->info.rate    = ROAR_MIDI_TICKS_PER_BEAT;

       s->info.channels = ROAR_MIDI_CHANNELS_DEFAULT;
       s->info.bits     = ROAR_MIDI_BITS;
       s->info.codec    = ROAR_CODEC_MIDI; // in case codec == -1
      break;
    }
   break;
  case ROAR_DIR_RAW_OUT:
    if ( s->info.rate == -1 )
     s->info.rate = 0;
    if ( s->info.bits == -1 )
     s->info.bits = 0;
    if ( s->info.channels == -1 )
     s->info.channels = 0;
    if ( s->info.codec == -1 )
     s->info.codec = 0;
   break;
 }

 if ( s->info.rate == -1 )
  s->info.rate = g_sa->rate;
 if ( s->info.bits == -1 )
  s->info.bits = g_sa->bits;
 if ( s->info.channels == -1 )
  s->info.channels = g_sa->channels;
 if ( s->info.codec == -1 )
  s->info.codec = g_sa->codec;

 ROAR_DBG("add_output(*): s->info = {.rate=%i, .bits=%i, .channels=%i, .codec=%i}", s->info.rate, s->info.bits, s->info.channels, s->info.codec);

 if ( streams_set_dir(stream, dir, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

#ifdef ROAR_DRIVER_CODEC
 if ( to_free != NULL )
  free(to_free);
#endif

 if ( s->info.codec == ROAR_CODEC_ALAW || s->info.codec == ROAR_CODEC_MULAW )
  s->info.bits = 8; // needed to open OSS driver, will be overriden by codecfilter

 ROAR_STREAM_SERVER(s)->codec_orgi = s->info.codec;

 if ( driver_openvio(&(ss->vio), &(ss->driver_id), drv, dev, &(s->info), -1, ss) == -1 ) {
  ss->driver_id = -1; // don't close a driver not opened...
  memset(&(ss->vio), 0, sizeof(struct roar_vio_calls));
  streams_delete(stream);
  ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = -1", drv, dev, opts);
  if ( prim ) alive = 0;
  return -1;
 }

 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAMID, &stream); // ignore errors here
 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAM,   s); // ignore errors here

 if ( blocks != -1 )
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DBLOCKS, &blocks);

 if ( blocksize != -1 )
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DBLKSIZE, &blocksize);

 // TODO: we shoudld *really* check for errors here...
 if ( channel != -1 ) {
  tu16 = channel;
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DMXSCHAN, &tu16);
 }
 if ( universe != -1 ) {
  tu16 = universe;
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DMXUNIV, &tu16);
 }

 ROAR_DBG("add_output(*): ss->driver_id=%i", ss->driver_id);

 streams_set_fh(stream, -1); // update some internal structures

 if ( q > -1e6 ) {
  ROAR_DBG("add_output(*): setting q=%f", q);
  streams_ctl(stream, ROAR_CODECFILTER_CTL_SET_Q|ROAR_STREAM_CTL_TYPE_FLOAT, &q);
 }

 client_stream_add(g_self_client, stream);

 if ( prim ) {
  streams_mark_primary(stream);
  s->pos_rel_id = stream;
 }

 if ( sync ) {
  streams_set_flag(stream, ROAR_FLAG_SYNC);
 } else {
  streams_reset_flag(stream, ROAR_FLAG_SYNC);
 }

 if ( f_mmap )
  streams_set_flag(stream, ROAR_FLAG_MMAP);

 ROAR_DBG("add_output(*): s->info = {.rate=%i, .bits=%i, .channels=%i, .codec=%i}", s->info.rate, s->info.bits, s->info.channels, s->info.codec);
 return 0;
}

#ifndef ROAR_WITHOUT_DCOMP_MIXER
int add_hwmixer (char * drv, char * dev, char * opts, int prim, int count) {
 char * basename = NULL;
 char * subnames = NULL;
 char * k, * v;
 int basestream = streams_new();
 int ret;
 int error = 0;
//int hwmixer_open(int basestream, char * drv, char * dev, int fh, char * basename, char * subnames) {

 if ( basestream == -1 )
  return -1;

 client_stream_add(g_self_client, basestream);

 if ( opts == NULL ) {
  k = NULL;
 } else {
  k = strtok(opts, ",");
 }

 while (k != NULL) {
//  ROAR_WARN("add_output(*): opts: %s", k);

  if ( (v = strstr(k, "=")) != NULL ) {
   *v++ = 0;
  }

  if ( strcmp(k, "primary") == 0 ) {
   prim = 1;

  } else if ( strcmp(k, "name") == 0 ) {
   basename = v;
  } else if ( strcmp(k, "subs") == 0 ) {
   subnames = v;

  } else if ( strcmp(k, "autoconf") == 0 ) {
   streams_set_flag(basestream, ROAR_FLAG_AUTOCONF);
  } else if ( strcmp(k, "passmixer") == 0 ) {
   streams_set_flag(basestream, ROAR_FLAG_PASSMIXER);
  } else {
   ROAR_ERR("add_hwmixer(*): unknown option '%s'", k);
   error++;
  }

  if ( error ) {
   streams_delete(basestream);
   if ( prim ) alive = 0;
   return -1;
  }

  k = strtok(NULL, ",");
 }

 if ( prim ) {
  streams_mark_primary(basestream);
 }

 ret = hwmixer_open(basestream, drv, dev, -1, basename, subnames);

 if ( ret == -1 ) {
  streams_delete(basestream);
 }

 return ret == -1 ? -1 : 0;
}
#endif

// X11:
#ifdef ROAR_HAVE_LIBX11
int register_x11 (int unreg, char * sockname) {
 struct roar_x11_connection * x11con = NULL;
 int ret = 0;

 if ( (x11con = roar_x11_connect(x11display)) == NULL ) {
  ROAR_ERR("Can not connect to X11 server for %sregistering", unreg ? "un" : "");
  return -1;
 }

 if ( unreg ) {
  if ( roar_x11_delete_prop(x11con, "ROAR_SERVER") == -1 ) {
   ret = -1;
   ROAR_ERR("Error while unregistereing from X11", ROAR_DBG_INFO_INFO);
  } else {
   ROAR_INFO("Successfully unregistered from X11", ROAR_DBG_INFO_INFO);
  }
 } else {
  if ( roar_x11_set_prop(x11con, "ROAR_SERVER", sockname) == -1 ) {
   ret = -1;
   ROAR_ERR("Error while registereing to X11", ROAR_DBG_INFO_INFO);
  } else {
   ROAR_INFO("Successfully registered to X11", ROAR_DBG_INFO_INFO);
  }
 }

 roar_x11_disconnect(x11con);

 return ret;
}
#endif

// SLP:
void register_slp_callback(SLPHandle hslp, SLPError errcode, void * cookie) {
 /* return the error code in the cookie */
 *(SLPError*)cookie = errcode;
}

int register_slp (int unreg, char * sockname) {
#ifdef ROAR_HAVE_LIBSLP
 static int regged = 0;
 static char * sn = NULL;
 SLPError err;
 SLPError callbackerr;
 SLPHandle hslp;
 char addr[1024];
 char attr[1024] = "";
 char * location;
 char * description;

 if ( sockname != NULL )
  sn = sockname;

 snprintf(addr, sizeof(addr), ROAR_SLP_URL_TYPE_ROAR "://%s", sn);

 err = SLPOpen("en", SLP_FALSE, &hslp);

 if (err != SLP_OK) {
  ROAR_ERR("Error opening slp handle: Error #%i", err);
  return -1;
 }

 if (!unreg) {

  if ( SLPEscape(g_config->location, &location, SLP_FALSE) != SLP_OK ) {
   ROAR_ERR("Error using SLPEscape() on server location, really bad!");
   SLPClose(hslp);
   return -1;
  }

  if ( SLPEscape(g_config->description, &description, SLP_FALSE) != SLP_OK ) {
   ROAR_ERR("Error using SLPEscape() on server location, really bad!");
   SLPClose(hslp);
   return -1;
  }

  snprintf(attr, sizeof(attr), "(wave-rate=%i),(wave-channels=%i),(wave-bits=%i),"
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
                               "(light-channels=%i),"
#endif
                               "(location=%s),(description=%s)",
           g_sa->rate, g_sa->channels, g_sa->bits,
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
           g_light_state.channels,
#endif
           location, description
          );

  /* Register a service with SLP */
  err = SLPReg(hslp,
               addr,
               SLP_LIFETIME_MAXIMUM,
               0,
               attr,
               SLP_TRUE,
               register_slp_callback,
               &callbackerr);
  regged = 1;
 } else if ( unreg && regged ) {
  err = SLPDereg(hslp, addr, register_slp_callback, &callbackerr);
  regged = 0;
 } else {
  SLPClose(hslp);
  return -1;
 }

 /* err may contain an error code that occurred as the slp library    */
 /* _prepared_ to make the call.                                     */
 if ( err != SLP_OK ) {
  ROAR_ERR("Error (de)registering service with slp: Error #%i", err);
  return -1;
 }

 /* callbackerr may contain an error code (that was assigned through */
 /* the callback cookie) that occurred as slp packets were sent on    */
 /* the wire */
 if (callbackerr != SLP_OK) {
  ROAR_ERR("Error (de)registering service with slp: Error #%i", callbackerr);
  return -1;
 }

 SLPClose(hslp);
 return 0;
#else
 return -1;
#endif
}


// MAIN:

#define _CKHAVEARGS(x) if ( (i + (x)) >= argc ) { \
                        ROAR_ERR("Option %s requires more arguments. See --help for more details.", k); \
                        return 70; \
                       }

#ifdef ROAR_HAVE_MAIN_ARGS
int main (int argc, char * argv[]) {
#else
int main (void) {
#endif
#ifdef ROAR_HAVE_MAIN_ARGS
 int i;
 char * k;
#endif
#if defined(ROAR_SUPPORT_LISTEN) && defined(ROAR_HAVE_GETUID)
 char user_sock[80]  = {0};
#endif
 struct roar_audio_info sa, max_sa;
 struct roard_config config;
#ifdef ROAR_HAVE_FORK
 int    daemon       = 0;
#endif
 int    realtime     = 0;
 int    sysclocksync = 0;
 char * driver    = NULL;
 char * device    = NULL;
#ifdef ROAR_HAVE_MAIN_ARGS
 char * opts      = NULL;
#endif
// char * server = ROAR_DEFAULT_SOCK_GLOBAL;
#ifdef ROAR_SUPPORT_LISTEN
 int    port       = ROAR_DEFAULT_PORT;
 char * sock_addr  = NULL;
 int    sock_proto = ROAR_PROTO_ROARAUDIO;
 int    sock_dir   = -1;
 struct roar_audio_info sock_info = {0, 0, 0, 0};
#endif
 int               drvid;
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 char * s_drv     = "cf";
 char * s_dev     = NULL;
 char * s_con     = NULL;
 char * s_opt     = NULL;
 int    s_prim    = 0;
#endif
 char * o_drv     = getenv("ROAR_DRIVER");
 char * o_dev     = getenv("ROAR_DEVICE");
 char * o_opts    = NULL;
 int    o_prim    = 0;
 int    o_count   = 0;
#ifndef ROAR_WITHOUT_DCOMP_MIXER
 char * m_drv     = NULL;
 char * m_dev     = NULL;
 char * m_opts    = NULL;
 int    m_prim    = 0;
 int    m_count   = 0;
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 int    light_channels = LIGHT_CHANNELS_DEFAULT;
#endif
#ifdef ROAR_DEFAULT_SOCKGRP
 char * sock_grp  = ROAR_DEFAULT_SOCKGRP;
#else
 char * sock_grp  = NULL;
#endif
 char * sock_user = NULL;
#ifdef ROAR_SUPPORT_LISTEN
 int    sock_type = ROAR_SOCKET_TYPE_UNKNOWN;
#endif
#ifdef ROAR_HAVE_LIBSLP
 int    reg_slp   = 0;
#endif
#ifdef ROAR_HAVE_LIBX11
 int    reg_x11   = 0;
#endif
#ifdef ROAR_HAVE_CHROOT
 char * chrootdir = NULL;
#endif
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 struct group   * grp  = NULL;
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 struct passwd  * pwd  = NULL;
#endif
#ifdef ROAR_HAVE_GETSERVBYNAME
 struct servent * serv = NULL;
#endif
 DRIVER_USERDATA_T drvinst;
 struct roar_client * self = NULL;
#ifdef ROAR_HAVE_LIBDNET
 char decnethost[80];
#endif
#ifdef SUPPORT_PIDFILE
 struct roar_vio_calls pidfile_vio;
#endif

 ROAR_DBG("main(*): starting roard...");

 g_standby       =  0;
 g_autostandby   =  0;
 alive           =  1;
#ifdef ROAR_SUPPORT_LISTEN
 g_no_listen     =  0;
#else
 g_terminate     =  1;
#endif

 g_verbose       = ROAR_DBG_INFO_NONE;

 sa.bits     = ROAR_BITS_DEFAULT;
 sa.channels = ROAR_CHANNELS_DEFAULT;
 sa.rate     = ROAR_RATE_DEFAULT;
 sa.codec    = ROAR_CODEC_DEFAULT;

 g_sa        = &sa;
 g_max_sa    = &max_sa;

 memcpy(g_max_sa, g_sa, sizeof(max_sa));

 counters_init();

 g_config = &config;

 if ( init_config() == -1 ) {
  ROAR_ERR("Can not init default config!");
  return 1;
 }

 // load config
 roar_libroar_get_config();

 // init notify core:
 // TODO: reconsider number of lists.
 if ( roar_notify_core_new_global(-1) == -1 ) {
  ROAR_ERR("Can not init notify core!");
  return 1;
 }

 if ( roar_notify_core_register_proxy(NULL, roar_notify_proxy_std, NULL) == -1 ) {
  ROAR_ERR("Can not init notify core!");
  return 1;
 }

#ifdef DEBUG
 // enable early in case we have DEBUG set.
 dbg_notify_cb_register();
#endif

 if ( auth_init() == -1 ) {
  ROAR_ERR("Can not init auth subsystem!");
  return 1;
 }

#ifdef ROAR_SUPPORT_LISTEN
 if ( init_listening() == -1 ) {
  ROAR_ERR("Can not init listening sockets!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 if ( midi_init_config() == -1 ) {
  ROAR_ERR("Can not init MIDI config!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 if ( ssynth_init_config() == -1 ) {
  ROAR_ERR("Can not init ssynth config!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 if ( rdtcs_init_config() == -1 ) {
  ROAR_ERR("Can not init RDTCS config!");
  return 1;
 }
#endif

 if ( plugins_preinit() == -1 ) {
  ROAR_ERR("Can not pre-init plugins!");
  return 1;
 }

#ifdef ROAR_SUPPORT_LISTEN
#ifndef ROAR_TARGET_WIN32
 sock_addr = ROAR_DEFAULT_SOCK_GLOBAL;
#else
 sock_addr = ROAR_DEFAULT_HOST;
#endif

#ifdef ROAR_HAVE_GETUID
 if ( getuid() != 0 && getenv("HOME") != NULL ) {
/*
  snprintf(user_sock, 79, "%s/%s", (char*)getenv("HOME"), ROAR_DEFAULT_SOCK_USER);
*/
  roar_env_render_path_r(user_sock, sizeof(user_sock), "~/" ROAR_DEFAULT_SOCK_USER);
  sock_addr = user_sock;
  ROAR_DBG("main(*): setting sock_addr='%s'", sock_addr);
 }
#endif

 if ( getenv("ROAR_SERVER") != NULL )
  sock_addr = getenv("ROAR_SERVER");
#endif

 if ( clients_init() == -1 ) {
  ROAR_ERR("Can not init clients!");
  return 1;
 }

 if ( streams_init() == -1 ) {
  ROAR_ERR("Can not init streams!");
  return 1;
 }

 if ( (g_self_client = clients_new()) == -1 ) {
  ROAR_ERR("Can not create self client!");
  return 1;
 }

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 if ( sources_init() == -1 ) {
  ROAR_ERR("Can not init sources!");
  return 1;
 }

 if ( (sources_set_client(g_self_client)) == -1 ) {
  ROAR_ERR("Can not init set source client!");
  return 1;
 }
#endif

#ifdef ROAR_HAVE_MAIN_ARGS
 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "-h") == 0 || strcmp(k, "--help") == 0 ) {
   usage();
   return 0;

  } else if ( strcmp(k, "--start") == 0 ) {
   // this is a no op
  } else if ( strcmp(k, "--restart") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 1) == -1 ) {
    ROAR_WARN("Can not terminate old server (not running at %s?), tring to continue anyway", sock_addr);
   }
#else
   ROAR_ERR("--restart not supported");
#endif
  } else if ( strcmp(k, "--shutdown") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 1) == -1 ) {
    ROAR_WARN("Can not terminate old server (not running at %s?)", sock_addr);
    return 1;
   }
   return 0;
#else
   ROAR_ERR("--shutdown not supported");
   return 1;
#endif
  } else if ( strcmp(k, "--stop") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 0) == -1 ) {
    ROAR_WARN("Can not stop old server (not running at %s?)", sock_addr);
    return 1;
   }
   return 0;
#else
   ROAR_ERR("--stop not supported");
   return 1;
#endif


  } else if ( strcmp(k, "--demon") == 0 || strcmp(k, "--daemon") == 0 ) {
#ifdef ROAR_HAVE_FORK
   daemon = 1;
#else
   ROAR_ERR("--daemon not supported");
#endif
  } else if ( strcmp(k, "--verbose") == 0 ) {
   g_verbose++;
  } else if ( strcmp(k, "--terminate") == 0 ) {
   g_terminate = 1;
  } else if ( strcmp(k, "--sysclocksync") == 0 ) {
   sysclocksync = 1000;
  } else if ( strcmp(k, "--realtime") == 0 ) {
   realtime++;
  } else if ( strcmp(k, "--memlock") == 0 ) {
   _CKHAVEARGS(1);
   g_config->memlock_level = memlock_str2level(argv[++i]);
  } else if ( strcmp(k, "--chroot") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_HAVE_CHROOT
   chrootdir = argv[++i];
#else
   ROAR_ERR("--chroot not supported");
   i++;
#endif
  } else if ( strcmp(k, "--setgid") == 0 ) {
#ifdef ROAR_HAVE_SETGID
   setids |= R_SETGID;
#else
   ROAR_ERR("--setgid not supported");
#endif
  } else if ( strcmp(k, "--setuid") == 0 ) {
#ifdef ROAR_HAVE_SETUID
   setids |= R_SETUID;
#else
   ROAR_ERR("--setuid not supported");
#endif
  } else if ( strcmp(k, "--location") == 0 ) {
   _CKHAVEARGS(1);
   g_config->location = argv[++i];
  } else if ( strcmp(k, "--description") == 0 ) {
   _CKHAVEARGS(1);
   g_config->description = argv[++i];
  } else if ( strcmp(k, "--pidfile") == 0 ) {
   _CKHAVEARGS(1);
#ifdef SUPPORT_PIDFILE
   pidfile = argv[++i];
#else
   ROAR_ERR("--pidfile not supported");
   i++;
#endif
  } else if ( strcmp(k, "--log-syslog") == 0 ) {
#ifdef ROAR_HAVE_SYSLOG
   roar_debug_set_stderr_mode(ROAR_DEBUG_MODE_SYSLOG);
#else
   ROAR_ERR("--log-syslog not supported");
#endif


  } else if ( strcmp(k, "--plugin-load") == 0 ) {
   _CKHAVEARGS(1);
   if ( plugins_load(argv[++i]) == -1 ) {
    ROAR_ERR("Can not load plugin");
   }

  } else if ( strcmp(k, "--list-cf") == 0 ) {
   print_codecfilterlist();
   return 0;

  } else if ( strcmp(k, "-R") == 0 || strcmp(k, "--rate") == 0 ) {
   _CKHAVEARGS(1);
   sa.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "-B") == 0 || strcmp(k, "--bits") == 0 ) {
   _CKHAVEARGS(1);
   sa.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "-C") == 0 || strcmp(k, "--chans") == 0 ) {
   _CKHAVEARGS(1);
   sa.channels = atoi(argv[++i]);

  } else if ( strcmp(k, "--aiprofile") == 0 ) {
   _CKHAVEARGS(1);
   if ( roar_profile2info(&sa, argv[++i]) == -1 ) {
    ROAR_ERR("Unknown audio profile: %s", argv[i]);
    return 1;
   }
   sa.codec    = ROAR_CODEC_DEFAULT;

  } else if ( strcmp(k, "--stream-flags") == 0 ) {
   _CKHAVEARGS(1);
   if ( update_stream_flags(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set stream flags");
    return 1;
   }

  } else if ( strcmp(k, "-d") == 0 || strcmp(k, "--driver") == 0 ) {
   _CKHAVEARGS(1);
   driver = argv[++i];
   if ( strcmp(driver, "list") == 0 ) {
    ROAR_WARN("The option is obsolete, use --list-driver!");
    print_driverlist();
    return 0;
   }
  } else if ( strcmp(k, "-D") == 0 || strcmp(k, "--device") == 0 ) {
   _CKHAVEARGS(1);
   device = argv[++i];
  } else if ( strcmp(k, "-dO") == 0 ) {
   _CKHAVEARGS(1);
   opts = argv[++i];
  } else if ( strcmp(k, "--list-driver") == 0 ) {
   print_driverlist();
   return 0;

  } else if ( strcmp(k, "-o") == 0 || strcmp(k, "--odriver") == 0 ) {
   _CKHAVEARGS(1);
   o_drv  = argv[++i];
  } else if ( strcmp(k, "-O") == 0 || strcmp(k, "--odevice") == 0 ) {
   _CKHAVEARGS(1);
   o_dev  = argv[++i];
  } else if ( strcmp(k, "-oO") == 0 ) {
   _CKHAVEARGS(1);
   o_opts = argv[++i];
  } else if ( strcmp(k, "-oP") == 0 ) {
   o_prim = 1;
  } else if ( strcmp(k, "-oN") == 0 ) {
   if ( add_output(o_drv, o_dev, o_opts, o_prim, o_count) != -1 )
    o_count++;

   o_drv  = o_dev = o_opts = NULL;
   o_prim = 0;

  } else if ( strcmp(k, "-s") == 0 || strcmp(k, "--source") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_drv = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   i++;
#endif
  } else if ( strcmp(k, "-S") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_dev = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   i++;
#endif
  } else if ( strcmp(k, "-sO") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_opt = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   i++;
#endif
  } else if ( strcmp(k, "-sC") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_con = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   i++;
#endif
  } else if ( strcmp(k, "-sP") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_prim = 1;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-sN") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
    ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
   }
   s_opt = s_dev = s_con = NULL;
   s_drv = "cf";
   s_prim = 0;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "--list-sources") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   print_sourcelist();
   return 0;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   return 1;
#endif

  } else if ( strcmp(k, "-m") == 0 || strcmp(k, "--mixer") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   m_drv  = argv[++i];
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif
  } else if ( strcmp(k, "-M") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   m_dev  = argv[++i];
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif
  } else if ( strcmp(k, "-mO") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   m_opts = argv[++i];
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif
  } else if ( strcmp(k, "-mP") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   m_prim = 1;
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif
  } else if ( strcmp(k, "-mN") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   if ( add_hwmixer(m_drv, m_dev, m_opts, m_prim, m_count) != -1 )
    m_count++;

   m_drv  = o_dev = o_opts = NULL;
   m_prim = 0;
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif
  } else if ( strcmp(k, "--list-mixers") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_MIXER
   print_hwmixerlist();
   return 0;
#else
   ROAR_ERR("main(*): No support for mixer compiled in");
   return 1;
#endif

  } else if ( strcmp(k, "--light-channels") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
   light_channels = atoi(argv[++i]);
#else
   ROAR_WARN("main(*): no light subsystem compiled in");
   i++;
#endif

  } else if ( strcmp(k, "--rds-pi") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   g_rdtcs.rds.pi = atoi(argv[++i]);
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
   i++;
#endif
  } else if ( strcmp(k, "--rds-ps") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_ps(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set RDS PS to '%s' (longer than 8 chars?)", argv[i]);
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
   i++;
#endif
  } else if ( strcmp(k, "--rds-pty") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_pty(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set RDS PTY to '%s'", argv[i]);
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
   i++;
#endif
  } else if ( strcmp(k, "--rds-tp") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_flag(RDTCS_RDS_FLAG_TP, 0) == -1 ) {
    ROAR_ERR("Can not set RDS TP flag");
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif
  } else if ( strcmp(k, "--rds-ct") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_flag(RDTCS_RDS_FLAG_CT, 0) == -1 ) {
    ROAR_ERR("Can not set RDS CT flag");
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif


  } else if ( strcmp(k, "--midi-no-console") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.init_cb = 0;
#else
   // no warning here as this is the disable option
#endif
  } else if ( strcmp(k, "--midi-console-enable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.init_cb = 1;
#else
   ROAR_ERR("main(*): No support for MIDI subsystem part CB compiled in");
#endif
  } else if ( strcmp(k, "--midi-console") == 0 ) {
   _CKHAVEARGS(1);
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.console_dev = argv[++i];
   midi_config.init_cb = 1;
#else
   ROAR_ERR("main(*): No support for MIDI subsystem part CB compiled in");
   i++;
#endif

  } else if ( strcmp(k, "--ssynth-enable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
   ssynth_conf.enable = 1;
#else
   ROAR_ERR("main(*): No support for ssynth compiled in");
#endif
  } else if ( strcmp(k, "--ssynth-disable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
   ssynth_conf.enable = 0;
#else
   // we can safely ignore the disable
#endif

  } else if ( strcmp(k, "--x11-display") == 0 || strcmp(k, "--display") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_HAVE_LIBX11
   x11display = argv[++i];
#else
   ROAR_ERR("No X11 support compiled in!");
   return 1;
#endif


  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--port") == 0 ) {
   _CKHAVEARGS(1);
   // This is only useful in INET not UNIX mode.
#ifdef ROAR_SUPPORT_LISTEN
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;

   errno = 0;
   if ( (port = atoi(argv[++i])) < 1 ) {
#ifdef ROAR_HAVE_GETSERVBYNAME
    if ( (serv = getservbyname(argv[i], "tcp")) == NULL ) {
     ROAR_ERR("Unknown service: %s: %s", argv[i], strerror(errno));
     return 1;
    }
    // NOTE: we need to use ROAR_NET2HOST16() here even if s_port is of type int!
    ROAR_DBG("main(*): serv = {s_name='%s', s_aliases={...}, s_port=%i, s_proto='%s'}",
            serv->s_name, ROAR_NET2HOST16(serv->s_port), serv->s_proto);
    port = ROAR_NET2HOST16(serv->s_port);
#else
    ROAR_ERR("invalite port number: %s", argv[i]);
    return 1;
#endif
   }
#endif
  } else if ( strcmp(k, "-b") == 0 || strcmp(k, "--bind") == 0 || strcmp(k, "--sock") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   sock_addr = argv[++i];
#else
   i++;
#endif

  } else if ( strcmp(k, "--proto") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( (sock_proto = roar_str2proto(argv[++i])) == -1 ) {
    ROAR_ERR("Unknown protocol: %s", argv[i]);
    return 1;
   }
#endif
  } else if ( strcmp(k, "--proto-dir") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   if ( (sock_dir = roar_str2dir(argv[++i])) == -1 ) {
    ROAR_ERR("Unknown stream direction: %s", argv[i]);
    return 1;
   }
#else
   i++;
#endif
  } else if ( strcmp(k, "--proto-rate") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   sock_info.rate = atoi(argv[++i]);
#else
   i++;
#endif
  } else if ( strcmp(k, "--proto-bits") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   sock_info.bits = atoi(argv[++i]);
#else
   i++;
#endif
  } else if ( strcmp(k, "--proto-chans") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   sock_info.channels = atoi(argv[++i]);
#else
   i++;
#endif
  } else if ( strcmp(k, "--proto-codec") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   if ( (sock_info.codec = roar_str2codec(argv[++i])) == -1 ) {
    ROAR_ERR("Unknown codec: %s", argv[i]);
    return 1;
   }
#else
   i++;
#endif
  } else if ( strcmp(k, "--proto-aiprofile") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   if ( roar_profile2info(&sock_info, argv[++i]) == -1 ) {
    ROAR_ERR("Unknown audio profile: %s", argv[i]);
    return 1;
   }
#else
   i++;
#endif
  } else if ( strcmp(k, "--list-profiles") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   listen_listen_profiles();
   return 0;
#endif
  } else if ( strcmp(k, "--proto-profile") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_SUPPORT_LISTEN
   if ( get_listen_profile(argv[++i], &port, &sock_addr, &sock_type, &sock_proto, &sock_dir, &sock_info) == -1 ) {
    ROAR_ERR("Unknown listen profile: %s", argv[i]);
    return 1;
   }
#else
   i++;
#endif


  } else if ( strcmp(k, "--list-proto") == 0 ) {
   list_proto();
   return 0;

  } else if ( strcmp(k, "-t") == 0 || strcmp(k, "--tcp") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( sock_type != ROAR_SOCKET_TYPE_TCP && sock_type != ROAR_SOCKET_TYPE_TCP6 )
    sock_type = ROAR_SOCKET_TYPE_TCP;

   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#endif

  } else if ( strcmp(k, "-4") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_type = ROAR_SOCKET_TYPE_TCP;
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#endif
  } else if ( strcmp(k, "-6") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
#ifdef AF_INET6
   sock_type = ROAR_SOCKET_TYPE_TCP6;
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#else
    ROAR_ERR("No IPv6 support compiled in!");
    return 1;
#endif
#endif

  } else if ( strcmp(k, "-u") == 0 || strcmp(k, "--unix") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   // ignore this case as it is the default behavor.
   sock_type = ROAR_SOCKET_TYPE_UNIX;
#endif

  } else if ( strcmp(k, "-n") == 0 || strcmp(k, "--decnet") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
#ifdef ROAR_HAVE_LIBDNET
    port   = ROAR_DEFAULT_NUM;
    strcpy(decnethost, ROAR_DEFAULT_LISTEN_OBJECT);
    sock_addr = decnethost;
    sock_type = ROAR_SOCKET_TYPE_DECNET;
#else
    ROAR_ERR("No DECnet support compiled in!");
    return 1;
#endif
#endif
  } else if ( strcmp(k, "--new-sock") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( add_listen(sock_addr, port, sock_type, sock_user, sock_grp, sock_proto, sock_dir, &sock_info) != 0 ) {
    ROAR_ERR("Can not open listen socket!");
    return 1;
   }
#endif

  } else if ( strcmp(k, "--slp") == 0 ) {
#ifdef ROAR_HAVE_LIBSLP
   reg_slp = 1;
#else
   ROAR_ERR("No OpenSLP support compiled in!");
   return 1;
#endif

  } else if ( strcmp(k, "--x11") == 0 ) {
#ifdef ROAR_HAVE_LIBX11
   reg_x11 = 1;
#else
   ROAR_ERR("No X11 support compiled in!");
   return 1;
#endif


  } else if ( strcmp(k, "--jumbo-mtu") == 0 ) {
   _CKHAVEARGS(1);
   g_config->jumbo_mtu = atoi(argv[++i]);

  } else if ( strcmp(k, "-G") == 0 ) {
   _CKHAVEARGS(1);
   sock_grp  = argv[++i];
  } else if ( strcmp(k, "-U") == 0 ) {
   _CKHAVEARGS(1);
   sock_user = argv[++i];

  } else if ( strcmp(k, "--no-listen") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_addr   = "";
   g_terminate = 1;
   g_no_listen = 1;
#endif
  } else if ( strcmp(k, "--client-fh") == 0 ) {
   _CKHAVEARGS(1);
   if ( clients_new_from_fh(atoi(argv[++i]), ROAR_PROTO_ROARAUDIO, ROAR_BYTEORDER_NETWORK, 1) == -1 ) {
    ROAR_ERR("main(*): Can not set client's fh");
    return 1;
   }
  } else if ( strcmp(k, "--close-fh") == 0 ) {
   _CKHAVEARGS(1);
#ifdef ROAR_HAVE_IO_POSIX
   close(atoi(argv[++i]));
#else
   i++;
   ROAR_WARN("can not close file handle %s (closing not supported)", argv[i]);
#endif

  } else if ( strcmp(k, "--standby") == 0 ) {
   g_standby = 1;
  } else if ( strcmp(k, "--auto-standby") == 0 ) {
   g_autostandby = 1;
  } else {
   usage();
   return 1;
  }

 }
#endif

#ifndef DEBUG
 // notify dbg if requested, if in DEBUG mode
 // do not able because it got enabled early.
 if ( g_verbose >= 4 )
  dbg_notify_cb_register();
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIXER
 if ( m_drv != NULL ) {
  if ( add_hwmixer(m_drv, m_dev, m_opts, m_prim, m_count) == -1 ) {
   ROAR_ERR("main(*): adding mixer '%s' via '%s' failed!", m_dev, m_drv);
  }
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 if ( s_dev != NULL ) {
  if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
   ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
  }
 }
#endif

 add_default_output(o_drv, o_dev, o_opts, o_prim, o_count);

 ROAR_INFO("Server config: rate=%i, bits=%i, chans=%i", ROAR_DBG_INFO_NOTICE, sa.rate, sa.bits, sa.channels);

 if ( waveform_init() == -1 ) {
  ROAR_ERR("Can not initialize Waveform subsystem");
  return 1;
 }

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 if ( midi_init() == -1 ) {
  ROAR_ERR("Can not initialize MIDI subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 if ( ssynth_init() == -1 ) {
  ROAR_ERR("Can not initialize ssynth subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 if ( light_init(light_channels) == -1 ) {
  ROAR_ERR("Can not initialize light control subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 if ( rdtcs_init() == -1 ) {
  ROAR_ERR("Can not initialize RDTCS subsystem");
 }
#endif

 if ( plugins_init() == -1 ) {
  ROAR_ERR("Can not initialize plugins");
 }

#ifdef ROAR_SUPPORT_LISTEN
 if ( !g_no_listen ) {
  if ( add_listen(sock_addr, port, sock_type, sock_user, sock_grp, sock_proto, sock_dir, &sock_info) != 0 ) {
   ROAR_ERR("Can not open listen socket!");
   return 1;
  }
 }
#endif

 if ( output_buffer_init(&sa) == -1 ) {
  ROAR_ERR("Can not init output buffer!");
  return 1;
 }

 if ( driver == NULL ) {
  driver = "null";
 } else {
  ROAR_ERR("Usage of old driver interface. use -o not -d!");
  ROAR_WARN("-d will be removed within the next releases");
 }

 if ( driver_open(&drvinst, &drvid, driver, device, &sa) == -1 ) {
  ROAR_ERR("Can not open output driver!");
  return 1;
 }

 if ( samples_init() == -1 ) {
  ROAR_ERR("Can not init samples!");
  return 1;
 }


 // we should handle this on microcontrollers, too.
#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_TARGET_WIN32)
 signal(SIGINT,  on_sig_int);
 signal(SIGTERM, on_sig_term);
 signal(SIGCHLD, on_sig_chld);
 signal(SIGUSR1, on_sig_usr1);
 signal(SIGPIPE, SIG_IGN);  // ignore broken pipes
#endif

 if ( g_config->memlock_level == -1 ) {
  g_config->memlock_level = MEMLOCK_DEFAULT;
 }

 if ( memlock_set_level(g_config->memlock_level) == -1 ) {
  ROAR_WARN("Can not set memory locking level to target level.");
 }

 if ( realtime ) {
#ifdef DEBUG
  ROAR_WARN("compiled with -DDEBUG but realtime is enabled: for real realtime support compile without -DDEBUG");
#endif

#ifdef ROAR_HAVE_NICE
  errno = 0;
  nice(-5*realtime); // -5 for each --realtime
  if ( errno ) {
   ROAR_WARN("Can not decrease nice value by %i: %s", 5*realtime, strerror(errno));
  }
#else
  ROAR_WARN("Can not decrease nice value by %i: %s", 5*realtime, strerror(errno));
#endif
/*
#ifdef __linux__
  if ( ioprio_set(IOPRIO_WHO_PROCESS, getpid(), IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, 0)) == -1 )
   ROAR_WARN("Can not set io priority: %s", strerror(errno));
#endif
*/
 }

#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 if ( setids & R_SETGID ) {
  if ( sock_grp == NULL ) {
   ROAR_ERR("Can not set GID if no groupname is supplied");
   return 1;
  }
  if ( (grp = getgrnam(sock_grp)) == NULL ) {
   ROAR_ERR("Can not get GID for group %s: %s", sock_grp, strerror(errno));
   return 1;
  }
  if ( setgroups(0, (const gid_t *) NULL) == -1 ) {
   ROAR_ERR("Can not clear supplementary group IDs: %s", strerror(errno));
  }
  if ( !grp || setgid(grp->gr_gid) == -1 ) {
   ROAR_ERR("Can not set GroupID: %s", strerror(errno));
  }
 }
#endif


 clients_set_pid(g_self_client, getpid());
#ifdef ROAR_HAVE_GETUID
 clients_set_uid(g_self_client, getuid());
#endif
#ifdef ROAR_HAVE_GETGID
 clients_set_gid(g_self_client, getgid());
#endif
 clients_get(g_self_client, &self);

 if ( self == NULL ) {
  ROAR_ERR("Can not get self client!");
  return 1;
 }

 strcpy(self->name, "RoarAudio daemon internal");

 if ( roar_nnode_free(&(self->nnode)) == -1 )
  return 1;

 // not fully correct but ok as workaorund
 // so tools assume that roard runs on the same machine as
 // they in case they use AF_UNIX:
 if ( roar_nnode_new(&(self->nnode), ROAR_SOCKET_TYPE_UNIX) == -1 )
  return 1;

#ifdef ROAR_HAVE_FORK
 if ( daemon ) {
#ifdef ROAR_HAVE_SYSLOG
  roar_debug_set_stderr_mode(ROAR_DEBUG_MODE_SYSLOG);
#else
  roar_debug_set_stderr_fh(-1);
#endif

  close(ROAR_STDIN );
  close(ROAR_STDOUT);
  close(ROAR_STDERR);

  if ( fork() )
   ROAR_U_EXIT(0);

#ifdef ROAR_HAVE_SETSID
  setsid();
#endif
  clients_set_pid(g_self_client, getpid()); // reset pid as it changed
 }
#endif

#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 // early test for UID as we need this for the pidfile and the setuid()
 if ( sock_user != NULL ) {
  if ( (pwd = getpwnam(sock_user)) == NULL ) {
   ROAR_ERR("Can not get UID for user %s: %s", sock_user, strerror(errno));
   return 1;
  }
 }
#endif

 ROAR_INFO("Process ID: %i", ROAR_DBG_INFO_INFO, (int)getpid());

#ifdef SUPPORT_PIDFILE
 if ( pidfile != NULL ) {
  if ( roar_vio_open_file(&pidfile_vio, pidfile, O_WRONLY|O_CREAT, 0644) == -1 ) {
   ROAR_ERR("Can not write pidfile: %s", pidfile);
  } else {
   roar_vio_printf(&pidfile_vio, "%i\n", getpid());
   roar_vio_close(&pidfile_vio);
  }
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
  if ( pwd || grp ) {
   if ( chown(pidfile, pwd ? pwd->pw_uid : -1, grp ? grp->gr_gid : -1) == -1 ) {
    ROAR_WARN("Can not change ownership of pidfile: %s: %s", pidfile, strerror(errno));
   }
  }
  if ( chmod(pidfile, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1 ) {
   ROAR_WARN("Can not change permissions of pidfile: %s: %s", pidfile, strerror(errno));
  }
#endif
 }
#endif

#ifdef ROAR_HAVE_CHROOT
 if (chrootdir) {
  if ( chroot(chrootdir) == -1 ) {
   ROAR_ERR("Can not chroot to %s: %s", chrootdir, strerror(errno));
   return 2;
  }
  if ( chdir("/") == -1 ) {
   ROAR_ERR("Can not chdir to /: %s", strerror(errno));
   return 2;
  }
 }
#endif

#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 if ( setids & R_SETUID ) {
  if ( sock_user == NULL ) {
   ROAR_ERR("Can not set UID if no username is supplied");
   return 1;
  }
  if ( !pwd || setuid(pwd->pw_uid) == -1 ) {
   ROAR_ERR("Can not set UserID: %s", strerror(errno));
   return 3;
  }
#ifdef ROAR_HAVE_GETUID
  clients_set_uid(g_self_client, getuid());
#endif
 }
#endif

 // Register with OpenSLP:
#ifdef ROAR_HAVE_LIBSLP
 if ( reg_slp ) {
  register_slp(0, sock_addr);
 }
#endif

#ifdef ROAR_HAVE_LIBX11
 if ( reg_x11 ) {
  register_x11(0, sock_addr);
 }
#endif

 ROAR_INFO("Startup complet", ROAR_DBG_INFO_INFO);

 // start main loop...
 ROAR_INFO("Entering main loop", ROAR_DBG_INFO_INFO);
 main_loop(drvid, drvinst, &sa, sysclocksync);
 ROAR_INFO("Left main loop", ROAR_DBG_INFO_INFO);

 // clean up.
 clean_quit_prep();
 driver_close(drvinst, drvid);
 output_buffer_free();

 roar_notify_core_free(NULL);

 ROAR_INFO("Exiting, no error", ROAR_DBG_INFO_INFO);
 return 0;
}

void cleanup_listen_socket (int terminate) {
 int i;

 ROAR_DBG("cleanup_listen_socket(terminate=%i) = (void)?", terminate);

 ROAR_INFO("Cleaning up listen sockets", ROAR_DBG_INFO_INFO);

 // Deregister from SLP:
#ifdef ROAR_HAVE_LIBSLP
 register_slp(1, NULL);
#endif

#ifdef ROAR_HAVE_LIBX11
 register_x11(1, NULL);
#endif

#ifdef ROAR_SUPPORT_LISTEN
 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  if ( g_listen[i].used  ) {
   roar_vio_close(&(g_listen[i].sock));

   g_listen[i].used = 0;

#ifdef ROAR_HAVE_UNIX
   if ( server[i] != NULL )
    if ( *(server[i]) == '/' )
     unlink(server[i]);
#endif
  }
 }

#endif

 if ( terminate )
  g_terminate = 1;
}

void clean_quit_prep (void) {
 cleanup_listen_socket(0);

 plugins_free();

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 sources_free();
#endif
 streams_free();
 clients_free();
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 ssynth_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_CB
 midi_cb_stop(); // stop console beep
#endif
#ifndef ROAR_WITHOUT_DCOMP_MIDI
 midi_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 light_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 rdtcs_free();
#endif

 waveform_free();

#ifdef SUPPORT_PIDFILE
 if ( pidfile != NULL )
  unlink(pidfile);
#endif

 auth_free();
}

void clean_quit (void) {
 ROAR_INFO("Shuting down", ROAR_DBG_INFO_INFO);

 counters_print(ROAR_DEBUG_TYPE_INFO, 0);

 clean_quit_prep();
// driver_close(drvinst, drvid);
// output_buffer_free();

 roar_notify_core_free(NULL);

 ROAR_INFO("Exiting, no error", ROAR_DBG_INFO_INFO);
 exit(0);
}

//ll

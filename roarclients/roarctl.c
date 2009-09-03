//roarctl.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>

#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_SETUID)
#define _POSIX_USERS
#endif

#ifdef _POSIX_USERS
#include <pwd.h>
#include <grp.h>
#endif

#include <sys/time.h>
#include <time.h>

int g_verbose = 0;

int display_mixer (struct roar_connection * con, int stream);
int show_meta_all (struct roar_connection * con, int id);

void usage (void) {
 printf("roarctl [OPTIONS]... COMMAND [OPTS] [COMMAND [OPTS] [COMMAND [OPTS] [...]]]\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER         - Set server hostname\n"
        "  --help                  - Show this help\n"
        "  --verbose   -v          - Show verbose output\n"
       );

 printf("\nCommands:\n\n");
 printf(
        "  help                    - Show this help\n"
        "  sleep TIME              - Sleeps for TIME seconds\n"
#ifdef ROAR_HAVE_GETTIMEOFDAY
        "  ping  NUM               - Do NUM pings using NOOP commands\n"
#endif
        "\n"
        "  standby, off            - Go into standby mode\n"
        "  resume, on              - Go into active mode\n"
        "  standbymode             - Show current standby mode\n"
        "  exit                    - Quits the roard (must be used as last command)\n"
        "  terminate               - Like exit but let the server up to serve still connected clients,\n"
        "                            new clients cann't connect and the server terminates after the last\n"
        "                            client disconnected\n"
        "\n"
        "  volume ID CHAN V0 V1... - Sets volume for stream ID\n"
        "                            CHAN is the number of channels or 'mono' or 'stereo'\n"
        "                            if mono or stereo is chosen roarctl trys to set\n"
        "                            sensfull values for all channels even if the output\n"
        "                            is has more channels.\n"
        "                            all other args are the volumes of the channels\n"
        "                            you may use integer or percent values.\n"
        "                            percent values can flooding points.\n"
        "\n"
        "  flag   ID FLAGS         - Sets flags FLAGS on stream ID. FLAGS may be a comma\n"
        "                            seperated list of flags.\n"
        "  unflag ID FLAGS         - Unsets flags on a stream. See flag.\n"
        "\n"
        "  kick TYPE ID            - Kicks object of TYPE with id ID\n"
        "                            Types: client stream sample source\n"
        "\n"
        "  metasave ID FILE        - Saves meta data of stream ID to file FILE\n"
        "  metaload ID FILE        - Loads meta data from file FILE and set it on stream ID\n"
        "\n"
        "  serveroinfo             - Gets Informations about server output\n"
        "  listclients             - Gets Informations about clients\n"
        "  liststreams             - Gets Informations about streams\n"
        "  allinfo                 - Get all infos\n"
       );
}

#ifdef ROAR_HAVE_GETTIMEOFDAY
int ping (struct roar_connection * con, int num) {
 struct timeval         try, ans;
 struct roar_message    m;
 register int           ret;
 int                    i;
 double                 cur, min = 3600*1000, max = 0, sum = 0;

 if ( num == 0 )
  return 0;

 for (i = 0; i < num; i++) {
  m.cmd = ROAR_CMD_NOOP;
  m.datalen = 0;

  gettimeofday(&try, NULL);
  ret = roar_req(con, &m, NULL);
  gettimeofday(&ans, NULL);

  if ( ret == -1 )
   return -1;

  while (ans.tv_sec > try.tv_sec) {
   ans.tv_sec--;
   ans.tv_usec += 1000000;
  }
  ans.tv_usec -= try.tv_usec;

  printf("Pong from server: seq=%i time=%.3fms\n", i, (cur = ans.tv_usec/1000.0));

  sum += cur;
  if ( min > cur )
   min = cur;
  if ( cur > max )
   max = cur;

  if ( i != (num - 1) )
   sleep(1);
 }

 printf("\n--- ping statistics ---\n");
 printf("%i packets transmitted\n", i);
 printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n", min, sum/(double)i, max);

 return 0;
}
#endif

void server_oinfo (struct roar_connection * con) {
 struct roar_stream s;

 if ( roar_server_oinfo(con, &s) == -1 ) {
  fprintf(stderr, "Error: can not get server output info\n");
  return;
 }

 printf("Stream direction      : %s\n", roar_dir2str(s.dir));
 printf("Server Output rate    : %i\n", s.info.rate);
 printf("Server Output bits    : %i\n", s.info.bits);
 printf("Server Output channels: %i\n", s.info.channels);
 printf("Server Output codec   : %i (%s%s)\n", s.info.codec, roar_codec2str(s.info.codec),
                                     s.info.codec == ROAR_CODEC_DEFAULT ? " native" : "");
// printf("Server Output rate: %i", s.info.rate);
  if ( g_verbose > 1 )
   printf("Server Position       : %lu S (%.3fs)\n", (unsigned long int) s.pos, (float)s.pos/(s.info.rate*s.info.channels));
}

const char * proc_name (pid_t pid) {
 static char ret[80] = "?";
#ifdef __linux__
 char file[80], buf[80], *r;
 int  i;

 snprintf(file, 79, "/proc/%i/exe", pid);
 file[79] = 0;

 ret[0] = '?';
 ret[1] = 0;

 if ( (i = readlink(file, buf, 79)) != -1 ) {
  buf[i] = 0;
  if ( (r = strrchr(buf, '/')) != NULL ) {
   r++;
   if ( *r != 0 )
    strcpy(ret, r);
  }
 }
#endif

 return ret;
}

void list_clients (struct roar_connection * con) {
 int i;
 int num;
 int h;
 int id[ROAR_CLIENTS_MAX];
 struct roar_client c;
#ifdef _POSIX_USERS
 struct group  * grp = NULL;
 struct passwd * pwd = NULL;
#endif

 if ( (num = roar_list_clients(con, id, ROAR_CLIENTS_MAX)) == -1 ) {
  fprintf(stderr, "Error: can not get client list\n");
  return;
 }

 for (i = 0; i < num; i++) {
  printf("client %i:\n", id[i]);
  if ( roar_get_client(con, &c, id[i]) == -1 ) {
   fprintf(stderr, "Error: can not get client info\n");
   continue;
  }
  printf("Player name           : %s\n", c.name);
  printf("Player PID            : %i(%s)\n", c.pid, proc_name(c.pid));
  if ( c.uid != -1 ) {
#ifdef _POSIX_USERS
   pwd = getpwuid(c.uid);
   grp = getgrgid(c.gid);
   printf("Player UID/GID        : %i(%s)/%i(%s)\n", c.uid, pwd ? pwd->pw_name : "?", c.gid, grp ? grp->gr_name : "?");
#else
   printf("Player UID/GID        : %i/%i\n", c.uid, c.gid);
#endif
  }

  if ( g_verbose && c.proto != ROAR_PROTO_NONE ) {
   printf("Player Protocol       : %s\n", roar_proto2str(c.proto));
  }

  if ( c.execed != -1 )
   printf("Execed stream         : %i\n", c.execed);

  for (h = 0; h < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; h++)
   if ( c.streams[h] != -1 )
    printf("stream                : %i\n", c.streams[h]);
 }

}

void list_streams (struct roar_connection * con) {
 int i;
 int num;
 int id[ROAR_STREAMS_MAX];
 struct roar_stream s;
 struct roar_stream_info info;
 char flags[1024];
 char name[1024];
 char * infotext;


 if ( (num = roar_list_streams(con, id, ROAR_STREAMS_MAX)) == -1 ) {
  fprintf(stderr, "Error: can not get stream list\n");
  return;
 }

 for (i = 0; i < num; i++) {
  printf("stream %i:\n", id[i]);
  if ( roar_get_stream(con, &s, id[i]) == -1 ) {
   fprintf(stderr, "Error: can not get stream info\n");
   continue;
  }
  printf("Stream direction      : %s\n", roar_dir2str(s.dir));

  if ( roar_stream_get_name(con, &s, name, 1024) == 0 )
   printf("Stream name           : %s\n", name);

  if ( s.pos_rel_id == -1 ) {
   printf("Relativ position id   : none (stream not synchronized)\n");
  } else if ( s.pos_rel_id == id[i] ) {
   printf("Relativ position id   : %i (self synchronized)\n", s.pos_rel_id);
  } else {
   printf("Relativ position id   : %i (synchronized)\n", s.pos_rel_id);
  }
  if ( g_verbose > 1 ) {
   if ( s.info.rate && s.info.channels ) {
    printf("Position              : %lu S (%.3fs)\n", (unsigned long int) s.pos,
                                    (float)s.pos/(s.info.rate*s.info.channels));
   } else {
    printf("Position              : %lu S\n", (unsigned long int) s.pos);
   }
  }

  switch (s.dir) {
   case ROAR_DIR_MIDI_IN:
   case ROAR_DIR_MIDI_OUT:
     infotext = " ticks/s";
    break;
   case ROAR_DIR_LIGHT_IN:
   case ROAR_DIR_LIGHT_OUT:
     infotext = " updates/s";
    break;
   default:
     infotext = " Hz";
  }
  if ( s.info.rate )
   printf("Stream sample rate    : %i%s\n", s.info.rate, infotext);
  if ( s.info.bits )
   printf("Stream bits           : %i\n", s.info.bits);
  if ( s.info.channels )
  printf("Stream channels       : %i\n", s.info.channels);

  printf("Stream codec          : %2i (%s%s)\n", s.info.codec, roar_codec2str(s.info.codec),
                                       s.info.codec == ROAR_CODEC_DEFAULT ? " native" : "");
  if ( roar_stream_get_info(con, &s, &info) != -1 ) {
   if ( info.codec != s.info.codec ) {
    printf("Streamed codec        : %2i (%s%s)\n", info.codec, roar_codec2str(info.codec),
                                       info.codec == ROAR_CODEC_DEFAULT ? " native" : "");
   }

   if ( g_verbose ) {
    if ( info.block_size )
     printf("Stream block size     : %i Byte\n", info.block_size);

    printf("Underruns pre/post    : %i/%i\n",   info.pre_underruns, info.post_underruns);
    if ( g_verbose > 1 )
     printf("Stream delay          : %ims\n",   (int)info.delay/1000);

    *flags = 0;
    if ( info.flags & ROAR_FLAG_PRIMARY )
     strcat(flags, "primary ");
    if ( info.flags & ROAR_FLAG_SYNC )
     strcat(flags, "sync ");
    if ( info.flags & ROAR_FLAG_OUTPUT )
     strcat(flags, "output ");
    if ( info.flags & ROAR_FLAG_SOURCE )
     strcat(flags, "source ");
    if ( info.flags & ROAR_FLAG_META )
     strcat(flags, "meta ");
    if ( info.flags & ROAR_FLAG_AUTOCONF )
     strcat(flags, "autoconf ");
    if ( info.flags & ROAR_FLAG_CLEANMETA )
     strcat(flags, "cleanmeta ");
    if ( info.flags & ROAR_FLAG_HWMIXER )
     strcat(flags, "hwmixer ");
    if ( info.flags & ROAR_FLAG_PAUSE )
     strcat(flags, "pause ");
    if ( info.flags & ROAR_FLAG_MUTE )
     strcat(flags, "mute ");
    if ( info.flags & ROAR_FLAG_MMAP )
     strcat(flags, "mmap ");
    if ( info.flags & ROAR_FLAG_ANTIECHO )
     strcat(flags, "antiecho ");
    if ( info.flags & ROAR_FLAG_VIRTUAL )
     strcat(flags, "virtual ");
    if ( info.flags & ROAR_FLAG_RECSOURCE )
     strcat(flags, "recsource ");
    if ( info.flags & ROAR_FLAG_PASSMIXER )
     strcat(flags, "passmixer ");

    printf("Flags                 : %s\n", flags);
   }
  }

  if ( s.dir != ROAR_DIR_THRU ) {
   display_mixer(con, id[i]);
   show_meta_all(con, id[i]);
  }
 }

}

int display_mixer (struct roar_connection * con, int stream) {
 int channels;
 struct roar_mixer_settings mixer;
 int i;

 if ( roar_get_vol(con, stream, &mixer, &channels) == -1 ) {
  fprintf(stderr, "Error: can not get stream mixer info\n");
  return -1;
 }

 for (i = 0; i < channels; i++)
  printf("Mixer volume chan %2i  : %i (%.2f%%)\n", i, mixer.mixer[i], (float)mixer.mixer[i]/655.35);

 return 0;
}

int set_mixer (struct roar_connection * con, int * cur, int max, char * arg[]) {
 int chans = 0;
 int id;
 int i;
 int len;
 int old_chans;
 int vol_l, vol_r;
 char * k;
 struct roar_mixer_settings mixer;
 struct roar_mixer_settings old_mixer;

 if (*cur + 2 > max)
  return -1;

 id = atoi(arg[++(*cur)]);

 k = arg[++(*cur)];

 if ( roar_get_vol(con, id, &old_mixer, &old_chans) == -1 ) {
  fprintf(stderr, "Error: can not get stream mixer info\n");
  return -1;
 }


// TODO: clean up code here as the % vs. abs code is very duplicate...

 if ( strcmp(k, "mono") == 0 && old_chans != 1 ) {
  chans = 1;

  if ( *cur + 1 > max )
   return -1;

  k   = arg[++(*cur)];
  len = strlen(k);

  if ( k[len - 1] == '%' ) {
   k[len - 1] = 0;
   vol_l = (atof(k)*65535)/100;
  } else {
   vol_l = atoi(k);
  }

  for (i = 0; i < old_chans; i++)
   mixer.mixer[i] = vol_l;

  chans = old_chans;

 } else if ( strcmp(k, "stereo") == 0 && old_chans == 4 ) {
  chans = 4;

  if ( *cur + 2 > max )
   return -1;

  k   = arg[++(*cur)];
  len = strlen(k);

  if ( k[len - 1] == '%' ) {
   k[len - 1] = 0;
   vol_l = (atof(k)*65535)/100;
  } else {
   vol_l = atoi(k);
  }

  k   = arg[++(*cur)];
  len = strlen(k);

  if ( k[len - 1] == '%' ) {
   k[len - 1] = 0;
   vol_r = (atof(k)*65535)/100;
  } else {
   vol_r = atoi(k);
  }

  mixer.mixer[0] = vol_l;
  mixer.mixer[1] = vol_r;
  mixer.mixer[2] = vol_l;
  mixer.mixer[3] = vol_r;

 } else if ( strcmp(k, "stereo") == 0 && old_chans != 2 ) {
  chans = 2;
//  printf("mode: stereo; chans=%i, old_chans=%i\n", chans, old_chans);
  ROAR_ERR("mode stereo not supported");
  return -1;
 } else {
  if ( strcmp(k, "mono") == 0 ) {
   chans = 1;
  } else if ( strcmp(k, "stereo") == 0 ) {
   chans = 2;
  } else {
   chans = atoi(k);
  }

//  printf("mode: int; chans=%i, old_chans=%i\n", chans, old_chans);

  if ( *cur + chans > max )
   return -1;

  for (i = 0; i < chans; i++) {
   k   = arg[++(*cur)];
   len = strlen(k);

   if ( k[len - 1] == '%' ) {
    k[len - 1] = 0;
    mixer.mixer[i] = (atof(k)*(int)65535)/100;
   } else {
    mixer.mixer[i] = atoi(k);
   }
  }
 }

 mixer.scale = 65535;

 return roar_set_vol(con, id, &mixer, chans);
}

int set_meta (struct roar_connection * con, int id, char * mode, char * type, char * val) {
 struct roar_meta   meta;
 struct roar_stream s;
 int mode_i = ROAR_META_MODE_SET;

 memset(&s, 0, sizeof(s));

 s.id = id;

// printf("set_meta(*): mode='%s', type='%s', val='%s'\n", mode, type, val);

 if ( strcmp(mode, "add") == 0 ) {
  mode_i = ROAR_META_MODE_ADD;
 }

 meta.type   = roar_meta_inttype(type);
 meta.value  = val;
 meta.key[0] = 0;

 if ( meta.type == -1 ) {
  fprintf(stderr, "Error: unknown type: %s\n", type);
  return -1;
 }

// printf("D: type=%i, mode=%i\n", meta.type, mode_i);

 if ( roar_stream_meta_set(con, &s, mode_i, &meta) == -1 )
  return -1;

 meta.type  = ROAR_META_TYPE_NONE;
 meta.value = NULL;

 return roar_stream_meta_set(con, &s, ROAR_META_MODE_FINALIZE, &meta);
}

int load_meta (struct roar_connection * con, int id, char * file) {
 struct roar_meta   meta;
 struct roar_stream s;
 int mode_i = ROAR_META_MODE_SET;
 FILE * in;
 char lion[1024];
 char * v;

 memset(&s, 0, sizeof(s));

 s.id = id;

 if ( (in = fopen(file, "r")) == NULL )
  return -1;

 while (fgets(lion, 1024, in) != NULL) {
  if ( (v = strtok(lion, "\r\n")) != NULL )
   if ( (v = strtok(NULL, "\r\n")) != NULL )
    *(v-1) = 0;

  if ( (v = strstr(lion, "=")) == NULL ) {
   fprintf(stderr, "Error: can not parse meta data lion: %s\n", lion);
   continue;
  }

  *v++ = 0;

  meta.type   = roar_meta_inttype(lion);
  meta.value  = v;
  meta.key[0] = 0;

  if ( meta.type == -1 ) {
   fprintf(stderr, "Error: unknown type: %s\n", lion);
   continue;
  }

  if ( roar_stream_meta_set(con, &s, mode_i, &meta) == -1 )
   return -1;
 }

 fclose(in);

 meta.type  = ROAR_META_TYPE_NONE;
 meta.value = NULL;

 return roar_stream_meta_set(con, &s, ROAR_META_MODE_FINALIZE, &meta);
}

int show_meta_type (struct roar_connection * con, int id, char * type) {
 struct roar_meta   meta;
 struct roar_stream s;

 memset(&s, 0, sizeof(s));

 s.id = id;

 meta.type  = roar_meta_inttype(type);

 if ( meta.type == -1 ) {
  fprintf(stderr, "Error: unknown type: %s\n", type);
  return -1;
 }

 if ( roar_stream_meta_get(con, &s, &meta) == -1 )
  return -1;

 printf("Meta %-17s: %s\n", roar_meta_strtype(meta.type), meta.value);

 roar_meta_free(&meta);

 return 0;
}

int show_meta_all (struct roar_connection * con, int id) {
 struct roar_stream s;
 int types[ROAR_META_MAX_PER_STREAM];
 int i;
 int len;

 memset(&s, 0, sizeof(s));

 s.id = id;

 if ( (len = roar_stream_meta_list(con, &s, types, ROAR_META_MAX_PER_STREAM)) == -1 )
  return -1;

 for (i = 0; i < len; i++)
  show_meta_type(con, id, roar_meta_strtype(types[i]));

 return 0;
}

int save_meta (struct roar_connection * con, int id, char * file) {
 struct roar_stream s;
 struct roar_meta   meta;
 int types[ROAR_META_MAX_PER_STREAM];
 int i;
 int len;
 FILE * out;

 memset(&s, 0, sizeof(s));

 s.id = id;

 if ( (out = fopen(file, "w")) == NULL )
  return -1;

 if ( (len = roar_stream_meta_list(con, &s, types, ROAR_META_MAX_PER_STREAM)) == -1 )
  return -1;

 for (i = 0; i < len; i++) {
/*
  show_meta_type(con, id, roar_meta_strtype(types[i]));
*/
  meta.type  = types[i];

  if ( roar_stream_meta_get(con, &s, &meta) == -1 )
   continue;

//  printf("Meta %-17s: %s\n", roar_meta_strtype(meta.type), meta.value);

  fprintf(out, "%s=%s\n", roar_meta_strtype(meta.type), meta.value);

  roar_meta_free(&meta);
 }

 fclose(out);

 return 0;
}

int set_flags (struct roar_connection * con, int id, int reset, char * flags) {
 int f = ROAR_FLAG_NONE;
 char * c;
 struct roar_stream s[1];

 memset(s, 0, sizeof(struct roar_stream));

 s->id = id;

 c = strtok(flags, ",");
 while (c != NULL) {
  if ( !strcmp(c, "meta") ) {
   f |= ROAR_FLAG_META;
  } else if ( !strcmp(c, "primary") ) {
   f |= ROAR_FLAG_PRIMARY;
  } else if ( !strcmp(c, "sync") ) {
   f |= ROAR_FLAG_SYNC;
  } else if ( !strcmp(c, "cleanmeta") ) {
   f |= ROAR_FLAG_CLEANMETA;
  } else if ( !strcmp(c, "hwmixer") ) {
   f |= ROAR_FLAG_HWMIXER;
  } else if ( !strcmp(c, "pause") ) {
   f |= ROAR_FLAG_PAUSE;
  } else if ( !strcmp(c, "mute") ) {
   f |= ROAR_FLAG_MUTE;
  } else if ( !strcmp(c, "mmap") ) {
   f |= ROAR_FLAG_MMAP;
  } else if ( !strcmp(c, "antiecho") ) {
   f |= ROAR_FLAG_ANTIECHO;
  } else if ( !strcmp(c, "recsource") ) {
   f |= ROAR_FLAG_RECSOURCE;
  } else if ( !strcmp(c, "passmixer") ) {
   f |= ROAR_FLAG_PASSMIXER;
  } else {
   fprintf(stderr, "Error: unknown flag: %s\n", c);
   return -1;
  }

  c = strtok(NULL, ",");
 }

 return roar_stream_set_flags(con, s, f, reset);
}

int main (int argc, char * argv[]) {
 struct roar_connection con;
 char * server   = NULL;
 char * k = NULL;
 int    i;
 int    t = 0;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "-v") == 0 || strcmp(k, "--verbose") == 0 ) {
   g_verbose++;
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( *k == '-' ) {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  } else {
   break;
  }
 }

 // connect

 if ( roar_connect(&con, server) == -1 ) {
  fprintf(stderr, "Error: Can not connect to server\n");
  return 1;
 }

 if ( roar_identify(&con, "roarctl") == -1 ) {
  fprintf(stderr, "Error: Can not identify to server\n");
  return 1;
 }

 if ( i == argc ) {
  fprintf(stderr, "Error: No Commands given\n");
  return 0; // this is not a fatal error...
 }

 for (; i < argc; i++) {
  k = argv[i];
  // cmd is in k

  printf("--- [ %s ] ---\n", k);

  if ( !strcmp(k, "help") ) {
   usage();

  } else if ( !strcmp(k, "sleep") ) {
   sleep(atoi(argv[++i]));

  } else if ( !strcmp(k, "ping") ) {
#ifdef ROAR_HAVE_GETTIMEOFDAY
   if ( ping(&con, atoi(argv[++i])) == -1 ) {
    fprintf(stderr, "Error: can not ping\n");
   }
#else
    fprintf(stderr, "Error: ping not supported.\n");
    i++;
#endif

  } else if ( !strcmp(k, "standby") || !strcmp(k, "off") ) {
   if ( roar_set_standby(&con, ROAR_STANDBY_ACTIVE) == -1 ) {
    fprintf(stderr, "Error: can not set mode to standby\n");
   } else {
    printf("going into standby\n");
   }
  } else if ( !strcmp(k, "resume") || !strcmp(k, "on") ) {
   if ( roar_set_standby(&con, ROAR_STANDBY_INACTIVE) == -1 ) {
    fprintf(stderr, "Error: can not set mode to active\n");
   } else {
    printf("going into active mode\n");
   }

  } else if ( !strcmp(k, "exit") ) {
   if ( roar_exit(&con) == -1 ) {
    fprintf(stderr, "Error: can not quit server\n");
   } else {
    printf("Server quited\n");
    break;
   }
  } else if ( !strcmp(k, "terminate") ) {
   if ( roar_terminate(&con, 1) == -1 ) {
    fprintf(stderr, "Error: can not terminate server\n");
   } else {
    printf("Server got asked to quited\n");
    break;
   }

  } else if ( !strcmp(k, "standbymode") ) {
   t = roar_get_standby(&con);
   if ( t == -1 ) {
    fprintf(stderr, "Error: can not get stanby mode\n");
   } else if ( t == ROAR_STANDBY_ACTIVE ) {
    printf("Server is in standby\n");
   } else if ( t == ROAR_STANDBY_INACTIVE ) {
    printf("Server is active\n");
   } else {
    fprintf(stderr, "Error: unknown standby mode: %i\n", t);
   }

  } else if ( !strcmp(k, "whoami") ) {
   printf("My client ID is: %i\n", roar_get_clientid(&con));
  } else if ( !strcmp(k, "serveroinfo") ) {
   server_oinfo(&con);
  } else if ( !strcmp(k, "listclients") ) {
   list_clients(&con);
  } else if ( !strcmp(k, "liststreams") ) {
   list_streams(&con);
  } else if ( !strcmp(k, "allinfo") ) {
   server_oinfo(&con);
   printf("\n");
   list_clients(&con);
   printf("\n");
   list_streams(&con);

  } else if ( !strcmp(k, "kick") ) {
   k = argv[++i];
   if ( !strcmp(k, "client") ) {
    t = ROAR_OT_CLIENT;
   } else if ( !strcmp(k, "stream") ) {
    t = ROAR_OT_STREAM;
   } else if ( !strcmp(k, "sample") ) {
    t = ROAR_OT_SAMPLE;
   } else if ( !strcmp(k, "source") ) {
    t = ROAR_OT_SOURCE;
   } else {
    fprintf(stderr, "Error: unknown type: %s\n", k);
    continue;
   }
   //t = atoi(argv[i++]);
   if ( roar_kick(&con, t, atoi(argv[++i])) == -1 ) {
    fprintf(stderr, "Error: can not kick %s\n", k);
   } else {
    printf("%s kicked\n", k);
   }

  } else if ( !strcmp(k, "volume") ) {
   if ( set_mixer(&con, &i, argc, argv) == -1 ) {
    fprintf(stderr, "Error: can not set volume\n");
   } else {
    printf("volume changed\n");
   }

  } else if ( !strcmp(k, "flag") ) {
   i++;
   if ( set_flags(&con, atoi(argv[i]), ROAR_SET_FLAG, argv[i+1]) == -1 ) {
    fprintf(stderr, "Error: can not set flags\n");
   } else {
    printf("flags changed\n");
   }
   i++;
  } else if ( !strcmp(k, "unflag") ) {
   i++;
   if ( set_flags(&con, atoi(argv[i]), ROAR_RESET_FLAG, argv[i+1]) == -1 ) {
    fprintf(stderr, "Error: can not reset flags\n");
   } else {
    printf("flags changed\n");
   }
   i++;
  } else if ( !strcmp(k, "metaset") ) {
   i++;
   if ( set_meta(&con, atoi(argv[i]), argv[i+1], argv[i+2], argv[i+3]) == -1 ) {
    fprintf(stderr, "Error: can not set meta data\n");
   } else {
    printf("meta data changed\n");
   }
   i += 3;
  } else if ( !strcmp(k, "metaget") ) {
   i++;
   if ( show_meta_type(&con, atoi(argv[i]), argv[i+1]) == -1 ) {
    fprintf(stderr, "Error: can not get meta data\n");
   }
   i++;
  } else if ( !strcmp(k, "metasave") ) {
   i++;
   if ( save_meta(&con, atoi(argv[i]), argv[i+1]) == -1 ) {
    fprintf(stderr, "Error: can not get meta data\n");
   } else {
    printf("meta data saved\n");
   }
   i++;
  } else if ( !strcmp(k, "metaload") ) {
   i++;
   if ( load_meta(&con, atoi(argv[i]), argv[i+1]) == -1 ) {
    fprintf(stderr, "Error: can not set meta data\n");
   } else {
    printf("meta data saved\n");
   }
   i++;

  } else {
   fprintf(stderr, "Error: invalid command: %s\n", k);
  }

 }

 roar_disconnect(&con);

 return 0;
}

//ll

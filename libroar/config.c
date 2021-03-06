//config.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

static struct roar_libroar_config_codec *
           roar_libroar_config_codec_get_conf(int codec, int create, struct roar_libroar_config * config);


struct roar_libroar_config * roar_libroar_get_config_ptr(void) {
 static struct roar_libroar_config config;
 static int    inited = 0;
 static char   authfile[1024];
 char        * home = roar_env_get_home(0);

 if ( !inited ) {
  memset(&config, 0, sizeof(config));

  config.server   = NULL;
  config.authfile = NULL;

  if ( home != NULL ) {
   snprintf(authfile, 1023, "%s/.roarauth", home);
   authfile[1023]     = 0;
   config.authfile    = authfile;
   config.serversfile = NULL;
  }

  inited++;
 }

 return &config;
}

struct roar_libroar_config * roar_libroar_get_config(void) {
 struct roar_libroar_config * config = roar_libroar_get_config_ptr();
 static int inited = 0;
 char * next;

 if ( !inited ) {
  next = getenv("ROAR_OPTIONS");

  if ( next != NULL ) {
   roar_libroar_config_parse(next, " ");
  }

  inited++;
 }

 return config;
}

#define _P_FP(v)   ((int)(atof((v))*256.0))
#define _P_INT(v)  (atoi((v)))
#define _P_BOOL(v) (*(v) == 'y' || *(v) == 'j' || *(v) == 't' || *(v) == '1' ? 1 : 0)

static int roar_libroar_config_parse_codec(struct roar_libroar_config * config, char * txt) {
 struct roar_libroar_config_codec * codec_cfg;
 int codec;
 char * codec_str, * option_str, * value_str;

 ROAR_DBG("roar_libroar_config_parse_codec(config=%p, txt='%s') = ?", config, txt);

 if ( config == NULL || txt == NULL )
  return -1;

 ROAR_DBG("roar_libroar_config_parse_codec(config=%p, txt='%s') = ?", config, txt);

 codec_str = strtok(txt, ":");

 if ( codec_str == NULL )
  return -1;

 option_str = strtok(NULL, ":");

 if ( option_str == NULL )
  return -1;

 value_str = strtok(NULL, ":");

 if ( value_str == NULL )
  return -1;

 ROAR_DBG("roar_libroar_config_parse_codec(config=%p, txt='%s') = ?", config, txt);

 if ( (codec = roar_str2codec(codec_str)) == -1 ) {
  ROAR_WARN("roar_libroar_config_parse_codec(*): Unknown codec: %s", codec_str);
  return -1;
 }

 ROAR_DBG("roar_libroar_config_parse_codec(config=%p, txt='%s'): codec=%i", config, txt, codec);

 if ( (codec_cfg = roar_libroar_config_codec_get_conf(codec, 1, config)) == NULL )
  return -1;

 ROAR_DBG("roar_libroar_config_parse_codec(config=%p, txt='%s'): codec=%i, codec_cfg=%p", config, txt, codec, codec_cfg);

 if ( !strcmp(option_str, "q") || !strcmp(option_str, "quality") ) {
  codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_Q;
  codec_cfg->q = _P_FP(value_str);
 } else if ( !strcmp(option_str, "complexity") ) {
  codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_COMPLEXITY;
  codec_cfg->complexity = _P_FP(value_str);
 } else if ( !strcmp(option_str, "dtx") ) {
  codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_DTX;
  codec_cfg->dtx = _P_BOOL(value_str);
 } else if ( !strcmp(option_str, "cc-max") ) {
  codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_MAX_CC;
  codec_cfg->max_cc = _P_INT(value_str);
 } else if ( !strcmp(option_str, "vbr") ) {
  codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_VBR;
  codec_cfg->vbr = _P_BOOL(value_str);
 } else if ( !strcmp(option_str, "mode") ) {
  if ( !strcmp(value_str, "nb") ) {
   codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_MODE;
   codec_cfg->mode      = ROAR_LIBROAR_CONFIG_MODE_NB;
  } else if ( !strcmp(value_str, "wb") ) {
   codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_MODE;
   codec_cfg->mode      = ROAR_LIBROAR_CONFIG_MODE_WB;
  } else if ( !strcmp(value_str, "uwb") ) {
   codec_cfg->para_set |= ROAR_LIBROAR_CONFIG_PSET_MODE;
   codec_cfg->mode      = ROAR_LIBROAR_CONFIG_MODE_UWB;
  } else {
   ROAR_WARN("roar_libroar_config_parse_codec(*): Unknown codec mode: %s", value_str);
   return -1;
  }
 } else {
  ROAR_WARN("roar_libroar_config_parse_codec(*): Unknown codec option: %s", option_str);
  return -1;
 }

 return 0;
}

int    roar_libroar_config_parse(char * txt, char * delm) {
 struct roar_libroar_config * config = roar_libroar_get_config_ptr();
 char * k, * v, * next = txt;

 while (next != NULL) {
  k = next;

  if ( delm == NULL ) {
   // no delm -> we have only one option
   next = NULL;
  } else {
   next = strstr(next, delm);
  }

  if ( next != NULL ) {
   *next = 0;
    next++;
  }

  ROAR_DBG("roar_libroar_config_parse(*): k='%s'", k);

  // strip leading spaces:
  while ( *k == ' ' ) k++;

  ROAR_DBG("roar_libroar_config_parse(*): k='%s'", k);

  // strip tailing new lions:
  v = strtok(k, "\r\n");
  if ( v != NULL ) {
   if ( *v == '\r' || *v == '\n' )
    *v = 0;
  }

  ROAR_DBG("roar_libroar_config_parse(*): k='%s'", k);

  // comments
  if ( *k == '#' )
   continue;

  ROAR_DBG("roar_libroar_config_parse(*): k='%s'", k);

  // empty options:
  if ( *k == 0 )
   continue;

  if ( (v = strstr(k, ":")) != NULL ) {
   *v = 0;
    v++;
  }

  ROAR_DBG("roar_libroar_config_parse(*): k='%s', v='%s'", k, v);

  if ( !strcmp(k, "workaround") ) {
   if ( !strcmp(v, "use-execed") ) {
    config->workaround.workarounds |= ROAR_LIBROAR_CONFIG_WAS_USE_EXECED;
   } else {
    ROAR_WARN("roar_libroar_config_parse(*): Unknown workaround option: %s", v);
   }
  } else if ( !strcmp(k, "warning") || !strcmp(k, "warn") ) {
   if ( !strcmp(v, "sysio") ) {
    config->warnings.sysio = ROAR_WARNING_ALWAYS;
   } else if ( !strcmp(v, "obsolete") ) {
    config->warnings.obsolete = ROAR_WARNING_ALWAYS;
   } else if ( !strcmp(v, "all") ) {
    config->warnings.sysio    = ROAR_WARNING_ALWAYS;
    config->warnings.obsolete = ROAR_WARNING_ALWAYS;
   } else {
    ROAR_WARN("roar_libroar_config_parse(*): Unknown warning option: %s", v);
   }
  } else if ( !strcmp(k, "force-rate") ) {
   config->info.rate = atoi(v);
  } else if ( !strcmp(k, "force-bits") ) {
   config->info.bits = atoi(v);
  } else if ( !strcmp(k, "force-channels") ) {
   config->info.channels = atoi(v);
  } else if ( !strcmp(k, "force-codec") ) {
   config->info.codec = roar_str2codec(v);
  } else if ( !strcmp(k, "codec") ) {
   if ( roar_libroar_config_parse_codec(config, v) == -1 ) {
    ROAR_WARN("roar_libroar_config_parse(*): Error parsing codec config option");
   }
  } else if ( !strcmp(k, "set-server") ) {
   if ( roar_libroar_get_server() == NULL )
    roar_libroar_set_server(v);
  } else if ( !strcmp(k, "set-authfile") ) {
   strncpy(config->authfile, v, 1023);
   config->authfile[1023] = 0;
  } else if ( !strcmp(k, "x11-display") ) {
   config->x11.display = v;
  } else {
   ROAR_WARN("roar_libroar_config_parse(*): Unknown option: %s", k);
  }
 }

 return 0;
}

struct roar_libroar_config_codec * roar_libroar_config_codec_get(int codec, int create) {
 struct roar_libroar_config * config = roar_libroar_get_config();
 return roar_libroar_config_codec_get_conf(codec, create, config);
}

static struct roar_libroar_config_codec *
           roar_libroar_config_codec_get_conf(int codec, int create, struct roar_libroar_config * config) {
 int i;
 int need_new = 1;

 ROAR_DBG("roar_libroar_config_codec_get_conf(codec=%i, create=%i, config=%p) = ?", codec, create, config);

 if ( codec < 0 || create < 0 )
  return NULL;

 ROAR_DBG("roar_libroar_config_codec_get_conf(codec=%i, create=%i, config=%p) = ?", codec, create, config);

 if ( config->codecs.num == 0 ) {
  // no match case:
  if ( !create )
   return NULL;
 } else {
  for (i = 0; i < config->codecs.num; i++) {
   if ( config->codecs.codec[i].codec == codec )
    return &(config->codecs.codec[i]);
   if ( config->codecs.codec[i].codec == -1 )
    need_new = 0;
  }
 }

 ROAR_DBG("roar_libroar_config_codec_get_conf(codec=%i, create=%i, config=%p) = ?", codec, create, config);

 if ( !create )
  return NULL;

 if ( !need_new ) {
  for (i = 0; i < config->codecs.num; i++) {
   if ( config->codecs.codec[i].codec == -1 ) {
    memset(&(config->codecs.codec[i]), 0, sizeof(struct roar_libroar_config_codec));
    config->codecs.codec[i].codec = codec;
    return &(config->codecs.codec[i]);
   }
  }
 }

 if ( config->codecs.num == 0 ) {
  config->codecs.codec = malloc(16*sizeof(struct roar_libroar_config_codec));
 } else {
  config->codecs.codec = realloc(config->codecs.codec, (config->codecs.num+16)*sizeof(struct roar_libroar_config_codec));
 }

 if ( config->codecs.codec == NULL )
  return NULL;

 memset(&(config->codecs.codec[config->codecs.num]), 0, 16);
 for (i = config->codecs.num; i < (config->codecs.num+16); i++) {
  config->codecs.codec[i].codec = -1;
 }

 i = config->codecs.num;
 config->codecs.num += 16;

 memset(&(config->codecs.codec[i]), 0, sizeof(struct roar_libroar_config_codec));
 config->codecs.codec[i].codec = codec;

 return &(config->codecs.codec[i]);
}

int    roar_libroar_set_server(char * server) {
 roar_libroar_get_config_ptr()->server = server;
 return 0;
}

char * roar_libroar_get_server(void) {
 return roar_libroar_get_config_ptr()->server;
}

void   roar_libroar_nowarn(void) {
 roar_libroar_get_config_ptr()->nowarncounter++;
}

void   roar_libroar_warn(void) {
 struct roar_libroar_config * cfg = roar_libroar_get_config_ptr();

 if ( cfg->nowarncounter == 0 ) {
  ROAR_WARN("roar_libroar_warn(): Re-Enabling already enabled warnings! (Application error?)");
  return;
 }

 cfg->nowarncounter--;
}

//ll

//roarphone.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include <roaraudio.h>
#include <libroardsp/libroardsp.h>
#include <libroareio/libroareio.h>

#if defined(ROAR_HAVE_LIBSPEEX) && !defined(ROAR_HAVE_LIBSPEEXDSP)
#define _SPEEX_API_OLD
#elif defined(ROAR_HAVE_LIBSPEEX) && defined(ROAR_HAVE_LIBSPEEXDSP)
#define _SPEEX_API_NEW
#endif

#ifdef _SPEEX_API_OLD
#include <speex/speex_echo.h>
#endif

#define TIMEDIV  100

#define DRIVER  "oss"

// anti echo:
#define AE_NONE      0
#define AE_SIMPLE    1
#define AE_SPEEX     2
#define AE_ROARD     3

#define DTX_F        25

#define CON_NONE     0x00
#define CON_CON      0x01
#define CON_STREAM   0x02

struct {
 int antiecho;
 int samples;
 int transcode;
 int64_t dtx_threshold;
 size_t jumbo_mtu;
 int    ioflush_interval;
 struct {
  struct {
   int downmix;
   float lowp_freq;
   int speex_prep;
   int speex_prep_denoise;
   int speex_prep_agc;
   int speex_prep_vad;
  } in;
 } filter;
} g_conf;

int dtx_counter = 0;

struct {
 int state;
 struct roar_connection con;
 struct roar_stream     stream;
 struct roar_vio_calls * svio;
} g_cons;

struct roar_bixcoder transcoder[1];

struct {
 struct roardsp_filterchain input;
 struct roardsp_filterchain output;
} g_filterchains;

struct {
 struct {
  char key[ROAR_META_MAX_NAMELEN];
  char value[LIBROAR_BUFFER_MSGDATA];
 } tmp;
 char * rn;
 char * nick;
 char * org;
 char * email;
 char * hp;
 char * loc;
 char * thumbnail;
} g_meta;

void usage (void) {
 printf("roarphone [OPTIONS]...\n");

 printf("\nServer Options:\n\n");

 printf("  --server    SERVER   - Set server hostname\n"
        "  --jumbo-mtu MTU      - Sets the MTU for Jumbo Packets\n"
        "  --io-flush  INTERVAL - Flushs output every INTERVAL packets\n"
       );

 printf("\nAudio Options:\n\n");
 printf("  --rate     RATE      - Set sample rate\n"
        "  --bits     BITS      - Set bits per sample\n"
        "  --chans    CHANNELS  - Set number of channels\n"
       );

 printf("\nAudio Filter Options:\n\n");
 printf("  --afi-downmix        - Enable input downmixing\n"
        "  --afi-lowpass FREQ   - Enable input lowpass at FREQ (in Hz)\n"
        "  --afi-speex-prep     - Enable speex preprocessor\n"
        "  --afi-speex-denoise  - Enable speex denoiser\n"
        "  --afi-speex-agc      - Enable speex AGC\n"
        "  --afi-speex-vad      - Enable speex VAD\n"
       );

 printf("\nCodec Options:\n\n");
 printf("  --codec    CODEC     - Set the codec\n"
        "  --transcode          - Use local transcodeing\n"
       );

 printf("\nDriver Options:\n\n");
 printf("  --driver   DRIVER    - Set the driver\n"
        "  --device   DEVICE    - Set the device\n"
       );

 printf("\nGeneral Options:\n\n");
 printf("  --antiecho AEMODE    - Set the anti echo mode\n"
        "  --threshold DTXTHRES - Set the DTX threshold, disabled by default\n"
        "  --help               - Show this help\n"
       );

 printf("\nMeta Data Options:\n\n");
 printf("  --m-rn    REALNAME   - Sets the real name\n"
        "  --m-nick  NICK       - Sets the nick name\n"
        "  --m-email EMAIL      - Sets the email address\n"
        "  --m-hp    HOMEPAGE   - Sets the homepage URL\n"
        "  --m-thumbn THUMBNAIL - Sets a URL to a thumbnail\n"
        "  --m-loc   LOCATION   - Sets the location (room number)\n"
        "  --m-org ORGANIZATION - Sets the organization/company name\n"
       );
}

int open_stream (struct roar_vio_calls * vio, char * server, struct roar_audio_info * info) {
 int fh;

 g_cons.svio = vio;

 if ( !(g_cons.state & CON_CON) )
  if ( roar_simple_connect(&(g_cons.con), server, "roarphone") == -1 )
   return -1;

 g_cons.state |= CON_CON;

 if ( (fh = roar_vio_simple_new_stream_obj(vio, &(g_cons.con), &(g_cons.stream),
                                           info->rate, info->channels, info->bits, info->codec,
                                           ROAR_DIR_BIDIR
                                          )) == -1 )
  return -1;

 g_cons.state |= CON_STREAM;

 return 0;
}

#define _SET_META(ivar,itype) if ( (ivar) != NULL ) {  \
                              meta.value = (ivar);     \
                              meta.type  = (itype);    \
                              roar_stream_meta_set(&(g_cons.con), &(g_cons.stream), ROAR_META_MODE_SET, &meta); \
                              }
int set_meta (void) {
 struct roar_meta   meta;

 meta.value  = g_meta.tmp.value;
 meta.key[0] = 0;
 meta.type   = ROAR_META_TYPE_NONE;

 roar_stream_meta_set(&(g_cons.con), &(g_cons.stream), ROAR_META_MODE_CLEAR, &meta);

 _SET_META(g_meta.thumbnail, ROAR_META_TYPE_THUMBNAIL);
 _SET_META(g_meta.loc,       ROAR_META_TYPE_LOCATION);
 _SET_META(g_meta.hp,        ROAR_META_TYPE_HOMEPAGE);
 _SET_META(g_meta.org,       ROAR_META_TYPE_ORGANIZATION);

 if ( g_meta.nick != NULL ) {
  if ( g_meta.rn  != NULL ) {
   snprintf(g_meta.tmp.value, LIBROAR_BUFFER_MSGDATA-1, "%s (%s)", g_meta.rn, g_meta.nick);
   g_meta.tmp.value[LIBROAR_BUFFER_MSGDATA-1] = 0;
   _SET_META(g_meta.tmp.value, ROAR_META_TYPE_AUTHOR);
  } else {
   _SET_META(g_meta.nick, ROAR_META_TYPE_AUTHOR);
  }
 } else {
  if ( g_meta.rn  != NULL ) {
   _SET_META(g_meta.rn, ROAR_META_TYPE_AUTHOR);
  }
 }

 // TODO: make this more nice...
 if ( g_meta.email != NULL ) {
  if ( g_meta.nick != NULL ) {
   if ( g_meta.rn != NULL ) {
    snprintf(g_meta.tmp.value, LIBROAR_BUFFER_MSGDATA-1, "%s (%s) <%s>", g_meta.rn, g_meta.nick, g_meta.email);
   } else {
    snprintf(g_meta.tmp.value, LIBROAR_BUFFER_MSGDATA-1, "%s <%s>", g_meta.nick, g_meta.email);
   }
  } else {
   if ( g_meta.rn != NULL ) {
    snprintf(g_meta.tmp.value, LIBROAR_BUFFER_MSGDATA-1, "%s <%s>", g_meta.rn, g_meta.email);
   } else {
    snprintf(g_meta.tmp.value, LIBROAR_BUFFER_MSGDATA-1, "<%s>", g_meta.email);
   }
  }
  g_meta.tmp.value[LIBROAR_BUFFER_MSGDATA-1] = 0;
  _SET_META(g_meta.tmp.value, ROAR_META_TYPE_CONTACT);
 }

 return 0;
}

#ifdef _SPEEX_API_OLD
int anti_echo_speex16(int16_t * buf, int16_t * aebuf, size_t len, struct roar_audio_info * info) {
 static SpeexEchoState * state = NULL;
 size_t samples = info->rate / TIMEDIV;
 static int16_t * obuf = NULL;

 if ( info->channels != 1 )
  return -1;

 if (len != samples)
  return -1;

 ROAR_DBG("anti_echo_speex16(*) = ?");

 if ( state == NULL ) {
  if ( (state = speex_echo_state_init(samples, 100*samples)) == NULL )
   return -1;

  // todo: set sample rate.
 }

 ROAR_DBG("anti_echo_speex16(*) = ?");

 if ( obuf == NULL ) {
  if ( (obuf = malloc(2*samples)) == NULL )
   return -1;
 }

 ROAR_DBG("anti_echo_speex16(*) = ?");

/*
 speex_echo_cancellation(state, buf, aebuf, obuf);
*/

 speex_echo_cancel(state, buf, aebuf, obuf, NULL);

 memcpy(buf, obuf, 2*samples);

 ROAR_DBG("anti_echo_speex16(*) = 0");

 return 0;
}
#endif

int anti_echo16(int16_t * buf, int16_t * aebuf, size_t len, struct roar_audio_info * info) {
 size_t i;

 (void)info;

 switch (g_conf.antiecho) {
  case AE_NONE:
    return 0;
   break;
  case AE_SIMPLE:
    for (i = 0; i < len; i++)
     buf[i] -= aebuf[i];
    return 0;
   break;
#ifdef _SPEEX_API_OLD
  case AE_SPEEX:
    return anti_echo_speex16(buf, aebuf, len, info);
   break;
#endif
  case AE_ROARD:
    return 0;
   break;
  default:
    return -1;
   break;
 }

 return -1;
}

int zero_if_noise16 (int16_t * data, size_t samples) {
 int64_t rms = roar_rms2_1_16(data, samples);

 if ( rms < g_conf.dtx_threshold ) {
  if ( dtx_counter ) {
   dtx_counter--;
  } else {
   memset(data, 0, samples*2);
  }
 } else {
  dtx_counter = DTX_F;
 }

 return 0;
}

int run_stream (struct roar_vio_calls * s0, struct roar_vio_calls * s1, struct roar_audio_info * info) {
 size_t len;
 void * outbuf, * micbuf;
 ssize_t outlen, miclen;
 unsigned long int pkg_count = 0;

 ROAR_DBG("run_stream(*): g_conf.samples = %i, info->bits = %i", g_conf.samples, info->bits);
 len = g_conf.samples * info->bits / 8;
 ROAR_DBG("run_stream(*): len=%lu", (unsigned long) len);

 if ( (outbuf = malloc(2*len)) == NULL )
  return -1;

 micbuf = outbuf + len;

 while (1) {
  if ( (miclen = roar_vio_read(s0, micbuf, len)) <= 0 )
   break;

  if ( roardsp_fchain_num(&(g_filterchains.input)) ) {
   if ( roardsp_fchain_calc(&(g_filterchains.input), micbuf, len) == -1 )
    break;
  }

  if ( g_conf.dtx_threshold > 0 )
   if ( info->bits == 16 )
    zero_if_noise16(micbuf, miclen/2);

  if ( g_conf.transcode ) {
   if ( roar_bixcoder_write_packet(transcoder, micbuf, miclen) == -1 )
    break;
  } else {
   if ( roar_vio_write(s1, micbuf, miclen) != miclen )
    break;
  }

  if ( g_conf.ioflush_interval != -1 ) {
   if ( !(pkg_count % g_conf.ioflush_interval) )
    roar_vio_sync(s1);
  }

  if ( g_conf.transcode ) {
   ROAR_DBG("run_stream(*): outbuf=%p, len=%lu", outbuf, (unsigned long) len);
   if ( roar_bixcoder_read_packet(transcoder, outbuf, len) == -1 )
    break;

   outlen = len;
  } else {
   if ( (outlen = roar_vio_read(s1, outbuf, len)) <= 0 )
    break;
  }

  if ( g_conf.antiecho != AE_NONE && info->bits == 16 )
   anti_echo16(outbuf, micbuf, ROAR_MIN(miclen, outlen)/2, info);

  if ( roardsp_fchain_num(&(g_filterchains.output)) ) {
   if ( roardsp_fchain_calc(&(g_filterchains.output), outbuf, outlen) == -1 )
    break;
  }

  if ( roar_vio_write(s0, outbuf, outlen) != outlen )
   break;

  pkg_count++;
 }

 free(outbuf);

 return 0;
}

int main (int argc, char * argv[]) {
 struct roar_audio_info info = {.rate     = ROAR_RATE_DEFAULT,
                                .bits     = ROAR_BITS_DEFAULT,
                                .channels = ROAR_CHANNELS_DEFAULT,
                                .codec    = ROAR_CODEC_DEFAULT
                               };
 struct roar_audio_info dinfo;
 struct roar_vio_calls dvio, svio, svio_jumbo, svio_real;
 struct roar_vio_calls * svio_p;
 struct roardsp_filter * filter;
 char * driver   = DRIVER;
 char * device   = NULL;
 char * server   = NULL;
 char * k;
 int    i;
 union {
  int32_t i32;
  size_t  size;
 } tmp;

 memset(&g_conf, 0, sizeof(g_conf));

 g_conf.antiecho         = AE_ROARD;
 g_conf.dtx_threshold    = -1;
 g_conf.ioflush_interval = -1;

 memset(&g_cons, 0, sizeof(g_cons));
 g_cons.state = CON_NONE;

 memset(&g_meta, 0, sizeof(g_meta));

 roardsp_fchain_init(&(g_filterchains.input));
 roardsp_fchain_init(&(g_filterchains.output));

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--jumbo-mtu") == 0 ) {
   g_conf.jumbo_mtu = atoi(argv[++i]);
  } else if ( strcmp(k, "--io-flush") == 0 ) {
   g_conf.ioflush_interval = atoi(argv[++i]);
  } else if ( strcmp(k, "--rate") == 0 ) {
   info.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   info.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 || strcmp(k, "--chans") == 0 ) {
   info.channels = atoi(argv[++i]);

  } else if ( strcmp(k, "--afi-downmix") == 0 ) {
   g_conf.filter.in.downmix = 1;
  } else if ( strcmp(k, "--afi-lowpass") == 0 ) {
   g_conf.filter.in.lowp_freq = atof(argv[++i]);
  } else if ( strcmp(k, "--afi-speex-prep") == 0 ) {
   g_conf.filter.in.speex_prep = 1;
  } else if ( strcmp(k, "--afi-speex-denoise") == 0 ) {
   g_conf.filter.in.speex_prep = 1;
   g_conf.filter.in.speex_prep_denoise = 1;
  } else if ( strcmp(k, "--afi-speex-agc") == 0 ) {
   g_conf.filter.in.speex_prep = 1;
   g_conf.filter.in.speex_prep_agc = 1;
  } else if ( strcmp(k, "--afi-speex-vad") == 0 ) {
   g_conf.filter.in.speex_prep = 1;
   g_conf.filter.in.speex_prep_vad = 1;

  } else if ( strcmp(k, "--codec") == 0 ) {
   info.codec = roar_str2codec(argv[++i]);

  } else if ( strcmp(k, "--driver") == 0 ) {
   driver = argv[++i];
  } else if ( strcmp(k, "--device") == 0 ) {
   device = argv[++i];
  } else if ( strcmp(k, "--antiecho") == 0 ) {
   k = argv[++i];
   if ( !strcmp(k, "none") ) {
    g_conf.antiecho = AE_NONE;
   } else if ( !strcmp(k, "simple") ) {
    g_conf.antiecho = AE_SIMPLE;
   } else if ( !strcmp(k, "speex") ) {
    g_conf.antiecho = AE_SPEEX;
   } else if ( !strcmp(k, "roard") ) {
    g_conf.antiecho = AE_ROARD;
   } else {
    fprintf(stderr, "Error: unknown mode: %s\n", k);
    return 1;
   }
  } else if ( strcmp(k, "--threshold") == 0 ) {
   g_conf.dtx_threshold = atol(argv[++i]);

   // use threshold^2 or threshold < 0 for not using DTX
   if ( g_conf.dtx_threshold > 0 )
    g_conf.dtx_threshold *= g_conf.dtx_threshold;
  } else if ( strcmp(k, "--transcode") == 0 ) {
   g_conf.transcode = 1;

  // META DATA:
  } else if ( strcmp(k, "--m-rn") == 0 ) {
   g_meta.rn = argv[++i];
  } else if ( strcmp(k, "--m-nick") == 0 ) {
   g_meta.nick = argv[++i];
  } else if ( strcmp(k, "--m-email") == 0 ) {
   g_meta.email = argv[++i];
  } else if ( strcmp(k, "--m-hp") == 0 ) {
   g_meta.hp = argv[++i];
  } else if ( strcmp(k, "--m-thumbn") == 0 ) {
   g_meta.thumbnail = argv[++i];
  } else if ( strcmp(k, "--m-loc") == 0 ) {
   g_meta.loc = argv[++i];
  } else if ( strcmp(k, "--m-org") == 0 ) {
   g_meta.org = argv[++i];


  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 // ignore errors, maybe it will work even if this fails
 // (btw. it will never fail without crashing the rest of the app ;)
 roar_libroar_set_server(server);

 if ( g_conf.antiecho == AE_SPEEX ) {
  ROAR_WARN("Speex Antiecho is obsolete and may be removed in future versions. Use --antiecho roard");
 }

 g_conf.samples = info.channels * info.rate / TIMEDIV;

 memcpy(&dinfo, &info, sizeof(dinfo));

 if ( g_conf.transcode ) {
  dinfo.bits  = 16;
  dinfo.codec = ROAR_CODEC_DEFAULT;

  switch (info.codec) {
   case ROAR_CODEC_ALAW:
   case ROAR_CODEC_MULAW:
     info.bits = 8;
    break;
   case ROAR_CODEC_ROAR_CELT:
     info.bits = 16;
    break;
   case ROAR_CODEC_ROAR_SPEEX:
     info.bits = 16;
    break;
  }
 }

 if ( roar_cdriver_open(&dvio, driver, device, &dinfo, ROAR_DIR_BIDIR) == -1 ) {
  ROAR_ERR("Can not open sound card.");
  return 1;
 }

 ROAR_DBG("main(*): CALL open_stream(&svio, server, &info)");
 if ( open_stream(&svio_real, server, &info) == -1 ) {
  ROAR_ERR("Can not open connection to server.");
  roar_vio_close(&dvio);
  return 2;
 }
 ROAR_DBG("main(*): RET");

 if ( roar_vio_open_re(&svio, &svio_real) == -1 ) {
  ROAR_ERR("Can not open connection to server (RE VIO).");
  roar_vio_close(&dvio);
  return 2;
 }

 if ( g_conf.jumbo_mtu ) {
  if ( roar_vio_open_jumbo(&svio_jumbo, &svio, g_conf.jumbo_mtu) == -1 ) {
   roar_vio_close(&dvio);
   roar_vio_close(&svio);
   return 2;
  }
  svio_p = &svio_jumbo;
 } else {
  svio_p = &svio;
 }

 set_meta();

 if ( g_conf.transcode ) {
  dinfo.codec = info.codec;

  if ( roar_bixcoder_init(transcoder, &dinfo, svio_p) == -1 ) {
   roar_vio_close(&svio);
   roar_vio_close(&dvio);
   return 10;
  }

  // ignore errors as it may also work if this fails
  roar_bixcoder_write_header(transcoder);
  roar_bixcoder_read_header(transcoder);

  g_conf.samples = 8 * roar_bixcoder_packet_size(transcoder, -1) / dinfo.bits;
 }

#define _err(x) roar_vio_close(&dvio); roar_vio_close(&svio); return (x)

 if ( g_conf.filter.in.downmix ) {
  if ( roardsp_filter_new(&filter, &(g_cons.stream), ROARDSP_FILTER_DOWNMIX) == -1 ) {
   _err(2);
  }

  if ( roardsp_fchain_add(&(g_filterchains.input), filter) == -1 ) {
   _err(2);
  }
 }

 if ( g_conf.filter.in.lowp_freq > 1 ) {
  if ( roardsp_filter_new(&filter, &(g_cons.stream), ROARDSP_FILTER_LOWP) == -1 ) {
   _err(2);
  }

  if ( roardsp_filter_ctl(filter, ROARDSP_FCTL_FREQ, &(g_conf.filter.in.lowp_freq)) == -1 ) {
   _err(2);
  }

  if ( roardsp_fchain_add(&(g_filterchains.input), filter) == -1 ) {
   _err(2);
  }
 }

 if ( g_conf.filter.in.speex_prep ) {
  if ( roardsp_filter_new(&filter, &(g_cons.stream), ROARDSP_FILTER_SPEEX_PREP) == -1 ) {
   _err(2);
  }

  tmp.size = g_conf.samples;
  if ( roardsp_filter_ctl(filter, ROARDSP_FCTL_PACKET_SIZE, &tmp) == -1 ) {
   _err(2);
  }

  tmp.i32 = 0;

  if ( g_conf.filter.in.speex_prep_denoise )
   tmp.i32 |= ROARDSP_SPEEX_PREP_DENOISE_ON;

  if ( g_conf.filter.in.speex_prep_agc )
   tmp.i32 |= ROARDSP_SPEEX_PREP_AGC_ON;

  if ( g_conf.filter.in.speex_prep_vad )
   tmp.i32 |= ROARDSP_SPEEX_PREP_VAD_ON;

  if ( roardsp_filter_ctl(filter, ROARDSP_FCTL_MODE, &tmp) == -1 ) {
   _err(2);
  }

  if ( roardsp_fchain_add(&(g_filterchains.input), filter) == -1 ) {
   _err(2);
  }
 }

#undef _err

 ROAR_DBG("main(*): CALL run_stream(&dvio, &svio, &info);");
 run_stream(&dvio, svio_p, &info);
 ROAR_DBG("main(*): RET");

 roar_bixcoder_close(transcoder);

 roar_vio_close(svio_p);
 roar_vio_close(&dvio);

 roardsp_fchain_uninit(&(g_filterchains.input));
 roardsp_fchain_uninit(&(g_filterchains.output));

 roar_disconnect(&(g_cons.con));

 return 0;
}

//ll

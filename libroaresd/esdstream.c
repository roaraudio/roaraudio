//esdstream.c:

#include "libroaresd.h"

/* open a socket for playing, monitoring, or recording as a stream */
/* the *_fallback functions try to open /dev/dsp if there's no EsounD */
int esd_play_stream( esd_format_t format, int rate,
                     const char *host, const char *name ) {
 int channels;
 int bits;

 if ( (format & ESD_BITS8) ) {
  bits = 8;
 } else {
  bits = 16;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_play(rate, channels, bits, ROAR_CODEC_DEFAULT, (char*)host, (char*) name);
}

int esd_play_stream_fallback( esd_format_t format, int rate,
                              const char *host, const char *name ) {
 int r;

 if ( (r = esd_play_stream(format, rate, host, name)) != -1 ) {
  return r;
 }

 return esd_play_stream(format, rate, "+fork", name);
}



int esd_monitor_stream( esd_format_t format, int rate,
                        const char *host, const char *name ) {
 int channels;
 int bits;

 if ( (format & ESD_BITS8) ) {
  bits = 8;
 } else {
  bits = 16;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_monitor(rate, channels, bits, ROAR_CODEC_DEFAULT, (char*)host, (char*)name);
}
/* int esd_monitor_stream_fallback( esd_format_t format, int rate ); */
int esd_record_stream( esd_format_t format, int rate,
                       const char *host, const char *name );
int esd_record_stream_fallback( esd_format_t format, int rate,
                                const char *host, const char *name );
int esd_filter_stream( esd_format_t format, int rate,
                       const char *host, const char *name ) {
 int channels;
 int bits;

 if ( (format & ESD_BITS8) ) {
  bits = 8;
 } else {
  bits = 16;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_filter(rate, channels, bits, ROAR_CODEC_DEFAULT, (char*)host, (char*)name);
}


//ll

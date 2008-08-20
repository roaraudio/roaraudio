//convert.h:

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "libroar.h"

int roar_conv_bits       (void * out, void * in, int samples, int from, int to);

int roar_conv_bits_8to16 (void * out, void * in, int samples);
int roar_conv_bits_16to8 (void * out, void * in, int samples);

int roar_conv_chans (void * out, void * in, int samples, int from, int to, int bits);

int roar_conv_chans_1ton8  (void * out, void * in, int samples, int to);
int roar_conv_chans_1ton16 (void * out, void * in, int samples, int to);
int roar_conv_chans_nto18  (void * out, void * in, int samples, int from);
int roar_conv_chans_nto116 (void * out, void * in, int samples, int from);

int roar_conv_rate    (void * out, void * in, int samples, int from, int to, int bits, int channels);
int roar_conv_rate_8  (void * out, void * in, int samples, int from, int to, int channels);
int roar_conv_rate_16 (void * out, void * in, int samples, int from, int to, int channels);

int raor_conv_codec (void * out, void * in, int samples, int from, int to, int bits);

int roar_conv_codec_s2u8 (void * out, void * in, int samples);

int roar_conv       (void * out, void * in, int samples, struct roar_audio_info * from, struct roar_audio_info * to);


int roar_conv_poly4_16  (int16_t * out, int16_t * in, size_t olen, size_t ilen);
int roar_conv_poly4_16s (int16_t * out, int16_t * in, size_t olen, size_t ilen, float step);

#endif

//ll

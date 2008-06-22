//convert.c:

#include "libroar.h"

int roar_conv_bits (void * out, void * in, int samples, int from, int to) {
 if ( from == to ) {
  if ( in == out )
   return 0;

  memcpy(out, in, samples * from / 8);
  return 0;
 }

 if ( from ==  8 && to == 16 )
  return roar_conv_bits_8to16(out, in, samples);

 if ( from == 16 && to ==  8 )
  return roar_conv_bits_16to8(out, in, samples);

 return -1;
}

int roar_conv_bits_8to16 (void * out, void * in, int samples) {
 char    * ip = (char   *)in;
 int16_t * op = (int16_t*)out;
 int i;

 for (i = samples - 1; i >= 0; i--)
  op[i] = ip[i] << 8;

 return 0;
}

int roar_conv_bits_16to8 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*)in;
 char    * op = (char   *)out;
 int i;

 for (i = 0; i < samples; i++)
  op[i] = ip[i] >> 8;

 return 0;
}


int roar_conv_chans (void * out, void * in, int samples, int from, int to, int bits) {
 if ( from == 1 ) {
  if ( bits == 8 ) {
   return roar_conv_chans_1ton8(out, in, samples, to);
  } else if ( bits == 16 ) {
   return roar_conv_chans_1ton16(out, in, samples, to);
  } else {
   return -1;
  }
 }

 return -1;
}

int roar_conv_chans_1ton8  (void * out, void * in, int samples, int to) {
 char * ip = (char*) in, * op = (char*) out;
 int i;
 int c;

 for (i = samples - 1; i >= 0; i--)
  for (c = to - 1; c >= 0; c--)
   op[i*to + c] = ip[i];

 return 0;
}

int roar_conv_chans_1ton16 (void * out, void * in, int samples, int to) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i;
 int c;

 for (i = samples - 1; i >= 0; i--)
  for (c = to - 1; c >= 0; c--)
   op[i*to + c] = ip[i];

 return 0;
}

int roar_conv_rate (void * out, void * in, int samples, int from, int to, int bits, int channels) {
 if ( bits == 8  )
  return roar_conv_rate_8(out, in, samples, from, to, channels);

 if ( bits == 16 )
  return roar_conv_rate_16(out, in, samples, from, to, channels);

 return -1;
}

int roar_conv_rate_8  (void * out, void * in, int samples, int from, int to, int channels) {
 return -1;
}

int roar_conv_rate_16 (void * out, void * in, int samples, int from, int to, int channels) {
 int16_t * ip = in, * op = out;
 int16_t avg;
 int i, j, c;
 float scale = to/from;
 int step;


 if ( scale > 1 ) {
  if ( scale - (int)scale != (float)0 )
   return -1;

  step = scale;
/*
  for (i = 0; i < samples/step; i++)
   for (j = 0; j < step; j++)
    for (c = 0; c < channels; c++)
     op[i + j*channels + c] = ip[i + c];
*/
  if ( step == 2 ) {
   for (i = 0; i < samples/step; i += channels) {
    for (j = 0; j < step; j++) {
     for (c = 0; c < channels; c++) {
      op[i*step + j*channels + c] = ip[i+c];
     }
    }
   }
   return 0;
  }
  return -1;
 } else {
  scale = 1/scale;

  if ( scale - (int)scale != (float)0 )
   return -1;

  step = scale;

  for (i = 0; i < samples/step; i++) {
   avg = 0;
   for (j = 0; j < step; j++)
    avg += ip[i*step + j];

   op[i] = avg/step;
  }
  return 0;
 } 

 return -1;
}

int raor_conv_codec (void * out, void * in, int samples, int from, int to, int bits) {
 int inbo = ROAR_CODEC_BYTE_ORDER(from), outbo = ROAR_CODEC_BYTE_ORDER(to);
 int ins  = ROAR_CODEC_IS_SIGNED(from),  outs  = ROAR_CODEC_IS_SIGNED(to);

 if ( inbo != outbo )
  return -1;

 if ( ins != outs ) {
  if ( ins && !outs ) {
   if ( bits == 8 ) {
    roar_conv_codec_s2u8(out, in, samples);
   } else {
    return -1;
   }
  } else {
   return -1;
  }
 }

 return 0;
}

int roar_conv_codec_s2u8 (void * out, void * in, int samples) {
 char * ip = in;
 unsigned char * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] + 128;

 return 0;
}


int roar_conv       (void * out, void * in, int samples, struct roar_audio_info * from, struct roar_audio_info * to) {
 void * ip = in;

 // TODO: decide how to work around both in and out beeing to small to hold all
 //       data between the steps.
 //       for the moment: guess out >= in

 if ( from->bits != to->bits ) {
  if ( roar_conv_bits(out, ip, samples, from->bits, to->bits) == -1 )
   return -1;
  else
   ip = out;
 }

 if ( from->channels != to->channels ) {
  if ( roar_conv_chans(out, ip, samples, from->channels, to->channels, to->bits) == -1 )
   return -1;
  else
   ip = out;
 }

 if ( from->rate != to->rate ) {
  if ( roar_conv_rate(out, ip, samples, from->rate, to->rate, to->bits, to->channels) == -1 )
   return -1;
  else
   ip = out;
 }

 if ( from->codec != to->codec )
  return -1;

 return 0;
}

//ll

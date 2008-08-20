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

 ROAR_DBG("roar_conv_bits_16to8(out=%p, in=%p, samples=%i) = ?", out, in, samples);

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
 } else if ( to == 1 ) {
  if ( bits == 8 ) {
   return -1;
  } else if ( bits == 16 ) {
   return roar_conv_chans_nto116(out, in, samples, from);
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

int roar_conv_chans_nto116 (void * out, void * in, int samples, int from) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i;
 int c;
 register int s;

 samples /= from;

 for (i = samples - 1; i >= 0; i--) {
  s  = 0;

  for (c = 0; c < from; c++)
   s += ip[i*from + c];

  s /= from;
  op[i] = s;
 }

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
 if ( channels == 1 ) {
  printf("roar_conv_rate_16(): samples=%i -> %i, rate=%i -> %i\n", samples*from/to, samples, from, to);
  return roar_conv_poly4_16s((int16_t*) out, (int16_t*) in, samples, samples*from/to, (float)from/to);
//  return roar_conv_poly4_16((int16_t*) out, (int16_t*) in, samples*to/from, samples);
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

 if ( from->rate != to->rate ) {
  if ( roar_conv_rate(out, ip, samples, from->rate, to->rate, to->bits, from->channels) == -1 )
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

 if ( from->codec != to->codec )
  return -1;

 return 0;
}



int roar_conv_poly4_16 (int16_t * out, int16_t * in, size_t olen, size_t ilen) {
 return roar_conv_poly4_16s(out, in, olen, ilen, (float)ilen/olen);
}

int roar_conv_poly4_16s (int16_t * out, int16_t * in, size_t olen, size_t ilen, float step) {
 float poly[4];
 float data[4];
 float t    = 0;
 int16_t * ci = in;
 int io, ii = 0;
 int i;

 printf("step=%f\n", step);

 // we can not make a poly4 with less than 4 points ;)
 if ( ilen < 4 )
  return -1;

 for (i = 0; i < 4; i++)
  data[i] = ci[i];
 roar_math_mkpoly_4x4(poly, data);
/*
 printf("new poly: data[4] = {%f, %f, %f, %f}, poly[4] = {%f, %f, %f, %f}\n",
         data[0], data[1], data[2], data[3],
         poly[0], poly[1], poly[2], poly[3]
       );
*/

 //0 1 2 3

 for (io = 0; io < olen; io++) {
//  printf("t=%f\n", t);
  out[io] = roar_math_cvpoly_4x4(poly, t);
  t += step;
  if ( t > 2 ) { // we need a new ploynome
 //  printf("t > 2, need new data\n");
   if ( (ii + 4) < ilen ) { // else: end of block.
    t -= 1;
//    printf("new data: ii=%i\n", ii);
    ii++;
    ci++;
    for (i = 0; i < 4; i++)
     data[i] = ci[i];
    roar_math_mkpoly_4x4(poly, data);
/* 
   printf("new poly: data[4] = {%f, %f, %f, %f}, poly[4] = {%f, %f, %f, %f}\n",
           data[0], data[1], data[2], data[3],
           poly[0], poly[1], poly[2], poly[3]
          );
*/
   }
  }
 }

 printf("io=%i\n", io);

 return 0;
}

//ll

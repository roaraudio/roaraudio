//mixer.c:

#include "roard.h"

int mix_clients (void * output, int bits, void ** input, int samples) {
 if ( bits == 8 ) {
  return mix_clients_8bit(output, input, samples);
 } else if ( bits == 16 ) {
  return mix_clients_16bit(output, input, samples);
 } else if ( bits == 24 ) {
  return mix_clients_24bit(output, input, samples);
 } else if ( bits == 32 ) {
  return mix_clients_32bit(output, input, samples);
 } else {
  return -1;
 }
}

int mix_clients_8bit (void * output, void ** input, int samples) {
 int i, s;
 int c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i] != NULL; i++)
   c += ((char**)input)[i][s];

  if ( c > 127 )
   c = 127;
  else if ( c < -128 )
   c = -128;
  ((char*)output)[s] = (char)c;
 }

 return 0;
}

int mix_clients_16bit (void * output, void ** input, int samples) {
 int i, s;
 int c;
 int16_t ** in  = (int16_t**) input;
 int16_t *  out = output;

#ifdef DEBUG
 for (i = 0; input[i] != NULL; i++)
  ROAR_DBG("mix_clients_16bit(*): input[%i] = %p", i, input[i]);
#endif

/*
 if ( input[0] != NULL )
  write(1, input[0], samples*2);
*/

/*
 if ( input[0] != NULL ) {
  memcpy(output, input[0], samples*2);
  return -1;
 }
*/

 for (s = 0; s < samples; s++) {
  c = 0;
  for (i = 0; input[i] != NULL; i++)
   c += in[i][s];

  if ( c > 32767 )
   c = 32767;
  else if ( c < -32768 )
   c = -32768;

  out[s] = c;
 }

 return 0;
}

int mix_clients_24bit (void * output, void ** input, int samples) {
 return -1;
}

int mix_clients_32bit (void * output, void ** input, int samples) {
/*
 int i, s;
 int c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i]; i++)
   c += ((int**)input)[i][s];

  if ( c > 127 )
   c = 127;
  else if ( c < -128 )
   c = -128;
  ((int*)output)[s] = (char)c;
 }

 return 0;
*/
 return 1;
}

int change_vol (void * output, int bits, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 if ( bits == 8 ) {
  return  change_vol_8bit(output, input, samples, channels, set);
 } else if ( bits == 16 ) {
  return  change_vol_16bit(output, input, samples, channels, set);
 } else if ( bits == 24 ) {
  return  change_vol_24bit(output, input, samples, channels, set);
 } else if ( bits == 32 ) {
  return  change_vol_32bit(output, input, samples, channels, set);
 } else {
  return -1;
 }
}

int change_vol_8bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 char * in = input, * out = output;
 int    i;
 int    s;

 if ( !(in && out) )
  return -1;

 for (i = 0; i < samples; i++) {
  s  = in[i];
  s *= set->mixer[i % channels];
  s /= set->scale;
  out[i] = s;
 }

 return 0;
}

int change_vol_16bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 int16_t * in = input, * out = output;
 int       i;
 int       s;

 if ( !(in && out) )
  return -1;

 for (i = 0; i < samples; i++) {
  s  = in[i];
  s *= set->mixer[i % channels];
  s /= set->scale;
  out[i] = s;
 }

 return 0;
}

int change_vol_24bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 return -1;
}

int change_vol_32bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 return -1;
}


//ll

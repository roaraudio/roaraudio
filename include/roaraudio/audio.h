//audio.h:

#ifndef _ROARAUDIO_AUDIO_H_
#define _ROARAUDIO_AUDIO_H_

#define ROAR_CODEC_PCM_LE  ROAR_CODEC_PCM_S_LE
#define ROAR_CODEC_PCM_BE  ROAR_CODEC_PCM_S_BE
#define ROAR_CODEC_PCM_PDP ROAR_CODEC_PCM_S_PDP

/*
 Bits:
 76543210
     |||\---\ byte-
     ||\----/ order
     |\-----> unsigned?
     \------> PCM(0) or MIDI(1)?
*/

#define ROAR_CODEC_IS_SIGNED(x)  (((x) & ROAR_CODEC_UNSIGNED) == 0 ? 1 : 0)
#define ROAR_CODEC_BYTE_ORDER(x) ((x) & 0x03)

#define ROAR_CODEC_UNSIGNED    (1 << 2)

#define ROAR_CODEC_PCM_S_LE  0x01
#define ROAR_CODEC_PCM_S_BE  0x02
#define ROAR_CODEC_PCM_S_PDP 0x03

#define ROAR_CODEC_PCM_U_LE  (ROAR_CODEC_PCM_S_LE  | ROAR_CODEC_UNSIGNED)
#define ROAR_CODEC_PCM_U_BE  (ROAR_CODEC_PCM_S_BE  | ROAR_CODEC_UNSIGNED)
#define ROAR_CODEC_PCM_U_PDP (ROAR_CODEC_PCM_S_PDP | ROAR_CODEC_UNSIGNED)

#if BYTE_ORDER == BIG_ENDIAN

#define ROAR_CODEC_DEFAULT ROAR_CODEC_PCM_BE
#define ROAR_BE2DEF(x) (x)

#elif BYTE_ORDER == LITTLE_ENDIAN

#define ROAR_CODEC_DEFAULT ROAR_CODEC_PCM_LE
#define ROAR_LE2DEF(x) (x)

#else

#define ROAR_CODEC_DEFAULT ROAR_CODEC_PCM_PDP
#define ROAR_PDP2DEF(x) (x)

#endif


#define ROAR_CODEC_NATIVE ROAR_CODEC_DEFAULT

#define ROAR_BITS_MAX             32

#define ROAR_BITS_DEFAULT         16
#define ROAR_CHANNELS_DEFAULT      2
#define ROAR_RATE_DEFAULT      44100

#define ROAR_MAX_CHANNELS         64

#define ROAR_SET_VOL_ALL           1
#define ROAR_SET_VOL_ONE           2

struct roar_audio_info {
 unsigned int rate;
 unsigned int bits;
 unsigned int channels;
 unsigned int codec;
};

struct roar_mixer_settings {
 //unsigned      channels;
 roar_intm16   scale;
 roar_intm16   mixer[ROAR_MAX_CHANNELS];
};

#endif

//ll

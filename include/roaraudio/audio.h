//audio.h:

#ifndef _ROARAUDIO_AUDIO_H_
#define _ROARAUDIO_AUDIO_H_

#define ROAR_CODEC_PCM_LE  ROAR_CODEC_PCM_S_LE
#define ROAR_CODEC_PCM_BE  ROAR_CODEC_PCM_S_BE
#define ROAR_CODEC_PCM_PDP ROAR_CODEC_PCM_S_PDP

/*
 Bits:
 76543210
   |||||\---\ byte-
   ||||\----/ order
   |||\-----> unsigned?
   ||\------> PCM(0) or MIDI(1)?
   |\-------> PCM/MIDI(0) or hi-level codecs(1)
   \--------> MISC(0) or RIFF/WAVE like(1)

 MIDI 0x08:
 76543210
      000 0x08 -> MIDI File

 hi-level 0x10:
 76543210
     0000 0x10 -> Ogg Vorbis
     0001 0x11 -> Native FLAC
     0010 0x12 -> Ogg Speex
     0011 0x13 -> Reserved for CELT
     0100 0x14 -> Ogg FLAC
     0101 0x15 -> Ogg General
     0110 0x16 -> Ogg CELT
     [...]
     **** 0x1a -> ROAR CELT
     **** 0x1b -> ROAR SPEEX

 RIFF/WAVE like 0x20:
  76543210
     00000 0x20 -> RIFF/WAVE
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

#define ROAR_CODEC_MIDI_FILE    0x08

#define ROAR_CODEC_OGG_VORBIS   0x10
#define ROAR_CODEC_FLAC         0x11 /* native FLAC without Ogg container */
#define ROAR_CODEC_OGG_SPEEX    0x12
/* #define ROAR_CODEC_CELT/OGG_CELT 0x13 Reserved for CELT */
#define ROAR_CODEC_OGG_FLAC     0x14
#define ROAR_CODEC_OGG_GENERAL  0x15

#define ROAR_CODEC_ROAR_CELT    0x1a
#define ROAR_CODEC_ROAR_SPEEX   0x1b


#define ROAR_CODEC_RIFF_WAVE    0x20

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


#define ROAR_SPEEX_MAX_CC          256
#define ROAR_SPEEX_MAGIC           "RoarSpeex"
#define ROAR_SPEEX_MAGIC_LEN       9
#define ROAR_SPEEX_MODE_NB         1
#define ROAR_SPEEX_MODE_WB         2
#define ROAR_SPEEX_MODE_UWB        3

#define ROAR_CELT_MAGIC            "RoarCELT0"
#define ROAR_CELT_MAGIC_LEN        9

struct roar_audio_info {
 unsigned int rate;
 unsigned int bits;
 unsigned int channels;
 unsigned int codec;
};

struct roar_mixer_settings {
 //unsigned      channels;
 roar_intm16   scale;
 roar_intm16   rpg_mul; // rpg = ReplayGain
 roar_intm16   rpg_div;
 roar_intm16   mixer[ROAR_MAX_CHANNELS];
};

#endif

//ll

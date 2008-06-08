//stream.h:

#ifndef _ROARAUDIO_SAMPLE_H_
#define _ROARAUDIO_SAMPLE_H_

struct roar_sample {
 size_t len;
 void * data;
 struct roar_audio_info info;
};

#endif

//ll

//stream.h:

#ifndef _ROARAUDIO_SAMPLE_H_
#define _ROARAUDIO_SAMPLE_H_

// we do not need any roar_audio_info struct because it IS in servers native format
struct roar_sample {
 char                 name[ROAR_BUFFER_NAME];
 struct roar_buffer * data;
};

#endif

//ll

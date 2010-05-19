//driver_portaudio.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of roard a part of RoarAudio,
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

#ifndef _DRIVER_PORTAUDIO_H_
#define _DRIVER_PORTAUDIO_H_

//#undef ROAR_HAVE_LIBPABLIO
//#define ROAR_HAVE_LIBPORTAUDIO_V0_19

#ifdef ROAR_HAVE_LIBPORTAUDIO
#if defined(ROAR_HAVE_LIBPABLIO) || defined(ROAR_HAVE_LIBPORTAUDIO_V0_19)
#define _DRIVER_PORTAUDIO_CAN_OPERATE
#endif
#endif

#ifdef _DRIVER_PORTAUDIO_CAN_OPERATE

struct driver_portaudio {
#ifdef ROAR_HAVE_LIBPABLIO
 PABLIO_Stream * ostream;
#elif defined(ROAR_HAVE_LIBPORTAUDIO_V0_19)
 PaStream *stream;
 int framesize;
#endif
};

typedef struct {
/*
 params.device                    = Pa_GetDefaultOutputDevice();
 params.channelCount              = info->channels;
 params.sampleFormat              = fmt;
 params.suggestedLatency          = Pa_GetDeviceInfo(params.device)->defaultLowOutputLatency;
 params.hostApiSpecificStreamInfo = NULL;
*/
 void * device;
 int channelCount;
 PaSampleFormat sampleFormat;
 int suggestedLatency;
 void * hostApiSpecificStreamInfo;
} PaStreamParameters;

#define paOutputUnderflowed -1

void *  Pa_GetDefaultOutputDevice(void);
// Pa_WriteStream(device->stream, buf, write_frames);
PaError  Pa_WriteStream(PaStream * stream, void *, size_t);
PaError Pa_StartStream(PaStream * stream);
PaError Pa_StopStream(PaStream * stream);
PaError Pa_CloseStream(PaStream * stream);

int driver_portaudio_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);

int     driver_portaudio_close        (struct roar_vio_calls * vio);
ssize_t driver_portaudio_write        (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_portaudio_sync         (struct roar_vio_calls * vio);
int     driver_portaudio_ctl          (struct roar_vio_calls * vio, int cmd, void * data);

#endif

#endif

//ll

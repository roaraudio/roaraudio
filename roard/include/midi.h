//midi.h:

#ifndef _MIDI_H_
#define _MIDI_H_

#include <roaraudio.h>

#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/kd.h>
#endif

#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1

int g_console;

uint32_t g_midi_cb_stoptime;

int midi_init (void);
int midi_free (void);

// cb = console beep
int midi_cb_play(float t, float freq, int override);
int midi_cb_update (void);
int midi_cb_start(float freq);
int midi_cb_stop (void);

#endif

//ll

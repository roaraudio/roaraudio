//midi.h:

#ifndef _MIDI_H_
#define _MIDI_H_

#include <roaraudio.h>

int g_console;

int midi_init (void);
int midi_free (void);

// cb = console beep
int midi_cb_play(float t, float freq, int override);
int midi_cb_start(float freq);
int midi_cb_stop (void);

#endif

//ll

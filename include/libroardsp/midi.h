//midi.h:

#ifndef _LIBROARMIDI_H_
#define _LIBROARMIDI_H_

#include "libroar.h"
#include <math.h>

#define ROAR_MIDI_NOTE_NONE -1

#define ROAR_MIDI_NOTE_C  26162
#define ROAR_MIDI_NOTE_Cs 27718
#define ROAR_MIDI_NOTE_D  29366
#define ROAR_MIDI_NOTE_Ds 31112
#define ROAR_MIDI_NOTE_E  32962
#define ROAR_MIDI_NOTE_F  34922
#define ROAR_MIDI_NOTE_Fs 36999
#define ROAR_MIDI_NOTE_G  39199
#define ROAR_MIDI_NOTE_Gs 41530
#define ROAR_MIDI_NOTE_A  44000
#define ROAR_MIDI_NOTE_As 46616
#define ROAR_MIDI_NOTE_B  49388

#define ROAR_MIDI_MAX_NOTENAME_LEN (1+8)

#define ROAR_MIDI_TYPE_SINE 1

struct roar_midi_len {
 int mul;
 int div;
};

struct roar_note_octave {
 uint16_t       note;
 char           name[ROAR_MIDI_MAX_NOTENAME_LEN+1];
 int            octave;
 float          freq;
 struct roar_midi_len len;
};


struct roar_midi_basic_state {
 struct roar_midi_len    len;
 struct roar_note_octave note;
};

char         * roar_midi_note2name   (uint16_t note);
uint16_t       roar_midi_name2note   (char * note);
float          roar_midi_note2freq   (uint16_t note);

//int            roar_midi_free_no     (struct roar_note_octave * note);

int            roar_midi_find_octave (char * note);
int            roar_midi_add_octave  (struct roar_note_octave * note);
int            roar_midi_notefill    (struct roar_note_octave * note);

int            roar_midi_gen_tone    (struct roar_note_octave * note, int16_t * samples, float t, int rate, int channels, int type, void * opts);


int roar_midi_play_note  (struct roar_stream * stream, struct roar_note_octave * note, float len);

int roar_midi_basic_init (struct roar_midi_basic_state * state);
int roar_midi_basic_play (struct roar_stream * stream, struct roar_midi_basic_state * state, char * notes);

#endif

//ll

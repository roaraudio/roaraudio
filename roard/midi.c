//midi.c:

#include "roard.h"

int midi_init (void) {
 int i;
 char * files[] = {
                   "/dev/console",
#ifdef __linux__
                   "/dev/tty0",
                   "/dev/vc/0",
#endif
                   NULL
                  };

 g_console = -1;
 g_midi_cb_stoptime = 0;

 for (i = 0; files[i] != NULL; i++) {
  if ( (g_console = open(files[i], O_WRONLY|O_NOCTTY, 0)) != -1 )
   break;
 }

 if ( g_console != -1 ) {
  return 0;
 } else {
  return -1;
 }
}

int midi_free (void) {
 if ( g_console != -1 )
  close(g_console);
 return 0;
}

int midi_cb_play(float t, float freq, int override) {
 float samples_per_sec /* S/s */ = g_sa->rate * g_sa->channels;

/*
#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1
*/
 if ( g_midi_cb_stoptime && override != MIDI_CB_OVERRIDE )
  return -1;

 g_midi_cb_stoptime = ROAR_MATH_OVERFLOW_ADD(g_pos, samples_per_sec*t);
 midi_cb_start(freq);

 return 0;
}

int midi_cb_update (void) {
 if ( g_midi_cb_stoptime <= g_pos )
  midi_cb_stop();

 return 0;
}

int midi_cb_start(float freq) {
// On linux this uses ioctl KIOCSOUND
#ifdef __linux__
 if ( g_console == -1 )
  return -1;

 if ( ioctl(g_console, KIOCSOUND, (int)(1193180.0/freq)) == -1 )
  return -1;

 return 0;
#else
 return -1;
#endif
}

int midi_cb_stop (void) {
#ifdef __linux__
 return midi_cb_start(0);
#else
 return -1;
#endif
}

//ll

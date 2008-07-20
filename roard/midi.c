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
/*
#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1
*/
 return -1;
}

int midi_cb_update (uint32_t pos) {
 if ( g_midi_cb_stoptime <= pos )
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

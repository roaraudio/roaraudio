//libroarpulse.h:

#ifndef _LIBROARPULSE_H_
#define _LIBROARPULSE_H_

#include <roaraudio.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#include <libroarpulse/simple.h>

// POSIX does not clearly specify what happens on NULL
#define ROAR_STRDUP(x) ((x) == NULL ? NULL : strdup((x)))

#endif

//ll

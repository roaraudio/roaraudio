//raorfish.h:

#ifndef _ROARFISH_H_
#define _RAORFISH_H_

#define __USE_XOPEN

#include <roaraudio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <limits.h>
#include <math.h>

#include <oggz/oggz.h>
#include <fishsound/fishsound.h>
#include <sndfile.h>

int play (char * file, char ** opts);

#ifdef __USE_SVID
#define MAXFLOAT HUGE
#endif

#endif

//ll

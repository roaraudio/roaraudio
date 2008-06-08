//libroaresd.h:

#ifndef _LIBROARESD_H_
#define _LIBROARESD_H_

#include <roaraudio.h>
#include <esd.h>
#include <time.h>
#include <sys/time.h>

#define ROAR_BC2ESD(b,c) (((b) == 8 ? ESD_BITS8 : ESD_BITS16) | ((c) == 1 ? ESD_MONO : ESD_STEREO))
#define ROAR_DIR2ESD(d)  ((d)  == ROAR_DIR_PLAY ? ESD_PLAY : (d) == ROAR_DIR_MONITOR ? ESD_MONITOR : \
                          (d)  == ROAR_DIR_RECORD ? ESD_RECORD : \
                          ESD_MONITOR /* ESD implements FILTER a diffrent way, think MONITOR ist most simular */ )

#define ROAR_S2ESD(s)    (ROAR_BC2ESD((s)->info.bits, (s)->info.channels) | ROAR_DIR2ESD((s)->dir))

#endif

//ll

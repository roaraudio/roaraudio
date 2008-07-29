//libroaryiff.h:

#ifndef _LIBROARYIFF_H_
#define _LIBROARYIFF_H_

#include <roaraudio.h>

#include <Y2/Y.h>           /* Basic Y types and constants. */
#include <Y2/Ylib.h>        /* YLib functions and structs. */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// YID 0 is error like -1 on libroar!
#define ROARYIFF_YID2ROAR(x) ((x)-1)
#define ROARYIFF_ROAR2YID(x) ((x)+1)

#endif

//ll

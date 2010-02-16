//roar-config.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>

struct {
 char * name;
 char * cflags;
 char * libs;
} flags[] = {
 // native/own libs:
 {"roar",      ROAR_CFLAGS, ROAR_LIBS        }, // NOTE: libroar *MUST* be the first entry
 {"roardsp",   ROAR_CFLAGS, ROAR_LIBS_DSP    },
 {"roarmidi",  ROAR_CFLAGS, ROAR_LIBS_MIDI   },
 {"roarlight", ROAR_CFLAGS, ROAR_LIBS_LIGHT  },
 {"roareio",   ROAR_CFLAGS, ROAR_LIBS_EIO    },
 // comp libs:
 {"roaresd",   ROAR_CFLAGS, ROAR_LIBS_C_ESD  },
 {"esd",       ROAR_CFLAGS, ROAR_LIBS_C_ESD  },
 {"roarartsc", ROAR_CFLAGS, ROAR_LIBS_C_ARTSC},
 {"artsc",     ROAR_CFLAGS, ROAR_LIBS_C_ARTSC},
 {"roarpulse", ROAR_CFLAGS, ROAR_LIBS_C_PULSE},
 {"pulse",     ROAR_CFLAGS, ROAR_LIBS_C_PULSE},
 {"pulse-simple", ROAR_CFLAGS, ROAR_LIBS_C_PULSE},
 {"roarsndio", ROAR_CFLAGS, ROAR_LIBS_C_SNDIO},
 {"sndio",     ROAR_CFLAGS, ROAR_LIBS_C_SNDIO},
 {"roaryiff",  ROAR_CFLAGS, ROAR_LIBS_C_YIFF },
 {"Y2",        ROAR_CFLAGS, ROAR_LIBS_C_YIFF },
 {NULL, NULL, NULL}
}, * flags_ptr = NULL;

void usage (void) {
 printf("Usage: roar-config [--version] [--libs] [--cflags] [lib]\n");

 printf("\nOptions:\n\n");

 printf(
        "  --version          - Show version of library\n"
        "  --libs             - Show linker flags (-lxxx) needed to link library\n"
        "  --cflags           - Show compiler flags needed to link library\n"
       );

}

int main (int argc, char * argv[]) {
 int i, h;
 int cflags = 0;
 int libs   = 0;
 char buf[1024] = {0};

 if ( argc == 1 ) {
  usage();
  return 0;
 }

 for (i = 1; i < argc; i++) {
  if ( !strcmp(argv[i], "--version") ) {
   printf("unknown\n");
  } else if ( !strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") ) {
   usage();
   return 0;
  } else if ( !strcmp(argv[i], "--libs") ) {
   libs   = 1;
  } else if ( !strcmp(argv[i], "--cflags") ) {
   cflags = 1;
  } else if ( flags_ptr == NULL ) {
   if ( !strncmp(argv[i], "lib", 3) )
    argv[i] += 3;

   for (h = 0; flags[h].name != NULL; h++) {
    if ( !strcasecmp(argv[i], flags[h].name) )
     flags_ptr = &(flags[h]);
   }

   if ( flags_ptr == NULL ) {
    ROAR_ERR("Unknown lib: %s", argv[i]);
    return 2;
   }
  } else {
   fprintf(stderr, "Unknown option: %s\n", argv[i]);
   usage();
   return 1;
  }
 }

 if ( flags_ptr == NULL )
  flags_ptr = &(flags[0]);

 if ( cflags )
  strcat(buf, flags_ptr->cflags);

 if ( libs )
  strcat(buf, flags_ptr->libs);

 puts(buf);

 return 0;
}

//ll

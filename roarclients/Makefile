TARGETS=roarcat roarcatad roarcat2sock roarcatsendfile roarcatplay roarctl roarmon roarsockconnect roartypes roarvorbis roarvumeter roarfilt roarsin roar-config polytest

include ../Makefile.conf

CELT_PREFIX=/home/phi/superhome/xiph/celt-test/
DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) -I$(CELT_PREFIX)/include
LDFLAGS = -g -L../lib/ -lroar -L$(CELT_PREFIX)/lib

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarcat: roarcat.o
roarcatad: roarcatad.o
roarcat2sock: roarcat2sock.o
roarcatsendfile: roarcatsendfile.o
roarcatplay: roarcatplay.o
roarctl: roarctl.o
roarmon: roarmon.o
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o -lm
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o -lm
roarvumeter: roarvumeter.o
	${CC} ${LDFLAGS} -o roarvumeter roarvumeter.o -lm
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roar-config: roar-config.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o -lvorbisfile
polytest: polytest.o

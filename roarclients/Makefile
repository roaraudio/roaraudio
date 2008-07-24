TARGETS=roarcat roarcatad roarcat2sock roarcatsendfile roarctl roarmon roarsockconnect roartypes roarvorbis roarfilt roarsin roar-config

DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -L../lib/ -lroar

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarcat: roarcat.o
roarcatad: roarcatad.o
roarcat2sock: roarcat2sock.o
roarcatsendfile: roarcatsendfile.o
roarctl: roarctl.o
roarmon: roarmon.o
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o -lm
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o -lm
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roar-config: roar-config.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o -lvorbisfile

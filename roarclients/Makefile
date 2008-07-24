TARGETS=roarcat roarcatad roarcat2sock roarctl roarmon roarsockconnect roartypes roarvorbis roarfilt roar-config

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
roarctl: roarctl.o
roarmon: roarmon.o
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o -lm
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roar-config: roar-config.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o -lvorbisfile

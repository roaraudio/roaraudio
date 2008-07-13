TARGETS=roarcat roarcatad roarcat2sock roarctl roarmon roarsockconnect roartypes roarvorbis roarfilt

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
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o -lvorbisfile

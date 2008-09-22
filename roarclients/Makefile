TARGETS=roarcat roarcatad roarcat2sock roarcatsendfile roarcatplay roarcatpassfh roarbidir roarctl roarmon roarsockconnect roartypes roarvorbis roarvumeter roarfilt roarsin roar-config polytest roarsocktypes roarshout roarradio

include ../Makefile.conf

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS = -g -L../lib/ $(LDPATH) -lroar

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
roarcatpassfh: roarcatpassfh.o
roarradio: roarradio.o
roarctl: roarctl.o
roarmon: roarmon.o
roarbidir: roarbidir.o
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o -lm -lroardsp
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o -lm
roarvumeter: roarvumeter.o
	${CC} ${LDFLAGS} -o roarvumeter roarvumeter.o -lm
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roarsocktypes: roarsocktypes.o
roar-config: roar-config.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o $(lib_vorbisfile)
roarshout: roarshout.o
	${CC} ${LDFLAGS} -o roarshout roarshout.o $(lib_shout)
polytest: polytest.o

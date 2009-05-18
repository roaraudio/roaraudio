TARGETS=roarcat roarcatad roarcat2sock roarcatsendfile roarcatplay roarcatpassfh roarbidir roarctl roarmon roarsockconnect roartypes roarvorbis roarvumeter roarfilt roarsin roar-config polytest roarsocktypes roarshout roarradio roarfctest roarmonhttp pinentrytest zcattest pgptest roarcatvio pipetest
TOOLS=roarify

include ../Makefile.conf

#DEFINES        = -DDEBUG
INCLUDE    = -I../include -I./include
CFLAGS    += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS    = -g -L../lib/ $(LDPATH) -lroar
LIBS       = -lroar $(lib_wsock32) $(lib_ws2_32)
DSPLIBS    = -lroardsp -lm $(LIBS)
SHOUTLIBS  = $(lib_shout) $(LIBS)
VORBISLIBS = $(lib_vorbisfile) $(lib_vorbis) $(lib_ogg) $(LIBS)

all: ${TARGETS} $(TOOLS)
	cp $(cp_v) ${TARGETS} $(TOOLS) ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarcat: roarcat.o
roarcatad: roarcatad.o
roarcat2sock: roarcat2sock.o
roarcatsendfile: roarcatsendfile.o
roarcatplay: roarcatplay.o
roarcatpassfh: roarcatpassfh.o
roarcatvio: roarcatvio.o
roarradio: roarradio.o
roarctl: roarctl.o
roarmon: roarmon.o
roarmonhttp: roarmonhttp.o
roarbidir: roarbidir.o
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o $(DSPLIBS)
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o $(DSPLIBS)
roarvumeter: roarvumeter.o
	${CC} ${LDFLAGS} -o roarvumeter roarvumeter.o $(DSPLIBS)
roarsockconnect: roarsockconnect.o
roartypes: roartypes.o
roarsocktypes: roarsocktypes.o
roar-config: roar-config.o
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o $(VORBISLIBS)
roarshout: roarshout.o
	${CC} ${LDFLAGS} -o roarshout roarshout.o $(SHOUTLIBS)
polytest: polytest.o
	${CC} ${LDFLAGS} -o polytest polytest.o $(DSPLIBS)
pinentrytest: pinentrytest.o
zcattest: zcattest.o
pgptest: pgptest.o
pipetest: pipetest.o
roarfctest: roarfctest.o
	${CC} ${LDFLAGS} -o roarfctest roarfctest.o $(DSPLIBS)

roarify.sht:
roarify: roarify.sht
	sed 's#%%%PREFIX_COMP%%%#$(PREFIX_COMP)#g' < roarify.sht > roarify
	chmod +rx roarify

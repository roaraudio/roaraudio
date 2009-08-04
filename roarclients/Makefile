TARGETS=roarcat roarcatad roarcat2sock roarcatsendfile roarcatplay roarcatpassfh roarbidir roarphone roarctl roarlight roarmon roarsockconnect roartypes roarvorbis roarvumeter roarfilt roarsin roar-config polytest roarsocktypes roarshout roarradio roarfctest roarmonhttp pinentrytest zcattest pgptest roarcatvio pipetest roarinterconnect
TOOLS=roarify

DRVS=driver_oss.o
DRVOBJS=driver.o $(DRVS)

include ../Makefile.conf

#DEFINES        = -DDEBUG
INCLUDE    = -I../include -I./include
CFLAGS    += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS    = -g -L../lib/ $(LDPATH) -lroar
NETLIBS    = $(lib_wsock32) $(lib_ws2_32)
LIBS       = -lroar $(NETLIBS)
DSPLIBS    = -lroardsp   -lm $(LIBS)
LIGHTLIBS  = -lroarlight -lm $(LIBS)
EIOLIBS    = -lroareio       $(LIBS)
SHOUTLIBS  = $(lib_shout) $(LIBS)
VORBISLIBS = $(lib_vorbisfile) $(lib_vorbis) $(lib_ogg) $(LIBS)

L          = $(CC) $(LDFLAGS) -o $@ $+ $(LIBS)

all: ${TARGETS} $(TOOLS)
	cp $(cp_v) ${TARGETS} $(TOOLS) ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarcat: roarcat.o
	$L
roarcatad: roarcatad.o
	$L
roarcat2sock: roarcat2sock.o
	$L
roarcatsendfile: roarcatsendfile.o
	$L
roarcatplay: roarcatplay.o
	$L
roarcatpassfh: roarcatpassfh.o
	$L
roarcatvio: roarcatvio.o
	$L
roarradio: roarradio.o
	$L
roarctl: roarctl.o
	$L
roarmon: roarmon.o
	$L
roarmonhttp: roarmonhttp.o
	$L
roarinterconnect: roarinterconnect.o
	$L $(lib_esd)
roarbidir: roarbidir.o
	$L
roarphone: roarphone.o $(DRVOBJS)
	${CC} ${LDFLAGS} -o roarphone roarphone.o $(DRVOBJS) $(DSPLIBS) $(EIOLIBS) $(lib_speex)
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o $(DSPLIBS)
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o $(DSPLIBS)
roarvumeter: roarvumeter.o
	${CC} ${LDFLAGS} -o roarvumeter roarvumeter.o $(DSPLIBS)
roarsockconnect: roarsockconnect.o
	$L
roartypes: roartypes.o
	$L
roarsocktypes: roarsocktypes.o
	$L
roar-config: roar-config.o
	$L
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o $(VORBISLIBS)
roarshout: roarshout.o
	${CC} ${LDFLAGS} -o roarshout roarshout.o $(SHOUTLIBS)
polytest: polytest.o
	${CC} ${LDFLAGS} -o polytest polytest.o $(DSPLIBS)
roarlight: roarlight.o
	${CC} ${LDFLAGS} -o roarlight roarlight.o $(LIGHTLIBS)
pinentrytest: pinentrytest.o
	$L
zcattest: zcattest.o
	$L
pgptest: pgptest.o
	$L
pipetest: pipetest.o
	$L
roarfctest: roarfctest.o
	${CC} ${LDFLAGS} -o roarfctest roarfctest.o $(DSPLIBS)

roarify.sht:
roarify: roarify.sht
	sed 's#%%%PREFIX_COMP%%%#$(PREFIX_COMP)#g' < roarify.sht > roarify
	chmod +rx roarify

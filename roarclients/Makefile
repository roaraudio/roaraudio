TARGETS_TESTS=pinentrytest zcattest pgptest pipetest polytest roarfctest
TARGETS_DEVEL=roartypes roarsocktypes roar-config
TARGETS_EXAMPLE=roarcatad roarcat2sock roarcatsendfile roarcatpassfh roarvorbis roarsin
TARGETS_IO=roarcat roarcatplay roarcatvio roarbidir roarmon roarmonhttp roarradio
TARGETS_CTL=roarctl roarlight roarinterconnect
TARGETS_DSP=roarfilt roarvumeter
TARGETS_MISC=roarsockconnect roarphone roarshout
TARGETS_M=roardtmf

TARGETS=$(TARGETS_IO) $(TARGETS_CTL) $(TARGETS_DSP) $(TARGETS_MISC) $(TARGETS_DEVEL) $(TARGETS_EXAMPLE) $(TARGETS_TESTS) $(TARGETS_M)
TOOLS=roarify

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
DEFINES    = -DCOMMON_VERSION=\"$(COMMON_VERSION)\" -DPREFIX=\"$(PREFIX)\" -DPREFIX_LIB=\"$(PREFIX_LIB)\" -DPREFIX_INC=\"$(PREFIX_INC)\"
INCLUDE    = -I../include -I./include
CFLAGS    += -g -Wall $(Wextra) -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS    = -g -L../lib/ $(LDPATH)
LIBS       = $(LIBROAR)
SHOUTLIBS  = $(lib_shout) $(LIBS)
VORBISLIBS = $(lib_vorbisfile) $(lib_vorbis) $(lib_ogg) $(LIBS)

LNL        = $(CC) $(LDFLAGS) -o $@ $+
L          = $(LNL) $(LIBS)

all: ${TARGETS} $(TOOLS)
	cp $(cp_v) ${TARGETS} $(TOOLS) ../lib/
clean:
	rm -f ${TARGETS} ${TOOLS} *.o
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
	$L $(LIBROARDSP) $(lib_m)
roarmon: roarmon.o
	$L
roarmonhttp: roarmonhttp.o
	$L
roarinterconnect: roarinterconnect.o
	$L $(lib_esd) $(LIBROAREIO)
roarbidir: roarbidir.o
	$L
roarphone: roarphone.o
	${CC} ${LDFLAGS} -o roarphone roarphone.o $(LIBROARDSP) $(LIBROAREIO)
roarfilt: roarfilt.o
	${CC} ${LDFLAGS} -o roarfilt roarfilt.o $(LIBROARDSP)
roarsin: roarsin.o
	${CC} ${LDFLAGS} -o roarsin roarsin.o $(LIBROARDSP)
roarvumeter: roarvumeter.o
	${CC} ${LDFLAGS} -o roarvumeter roarvumeter.o $(LIBROAR) $(LIBROARDSP) $(lib_m)
roardtmf: roardtmf.o
	${CC} ${LDFLAGS} -o roardtmf roardtmf.o $(LIBROAR) $(lib_m)
roarsockconnect: roarsockconnect.o
	$L
roartypes: roartypes.o
	$L
roarsocktypes: roarsocktypes.o
	$L
roar-config: roar-config.o
	$(LNL)
roarvorbis: roarvorbis.o
	${CC} ${LDFLAGS} -o roarvorbis roarvorbis.o $(VORBISLIBS)
roarshout: roarshout.o
	${CC} ${LDFLAGS} -o roarshout roarshout.o $(SHOUTLIBS)
polytest: polytest.o
	${CC} ${LDFLAGS} -o polytest polytest.o $(LIBROARDSP)
roarlight: roarlight.o
	${CC} ${LDFLAGS} -o roarlight roarlight.o $(LIBROARLIGHT)
pinentrytest: pinentrytest.o
	$L
zcattest: zcattest.o
	$L
pgptest: pgptest.o
	$L
pipetest: pipetest.o
	$L
roarfctest: roarfctest.o
	${CC} ${LDFLAGS} -o roarfctest roarfctest.o $(LIBROARDSP)

roarify.sht:
roarify: roarify.sht
	sed 's#%%%PREFIX_COMP_LIBS%%%#$(PREFIX_COMP_LIBS)#g; s#%%%PREFIX_COMP_BINS%%%#$(PREFIX_COMP_BINS)#g; s#%%%PREFIX_LIB%%%#$(PREFIX_LIB)#g; s#%%%VERSION%%%#$(COMMON_VERSION)#g;' < roarify.sht > roarify
	chmod +rx roarify

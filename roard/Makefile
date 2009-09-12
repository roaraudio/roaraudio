TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o meta.o codecfilter.o lib.o ssynth.o container_framework.o
SUBSYS=midi.o light.o raw.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o driver_oss.o driver_shout.o driver_sndio.o driver_dmx.o driver_pwmled.o driver_sysclock.o
CF=codecfilter_cmd.o codecfilter_vorbis.o codecfilter_celt.o codecfilter_speex.o codecfilter_wave.o codecfilter_alaw.o codecfilter_mulaw.o codecfilter_sndfile.o codecfilter_fishsound.o
EMUL=emul_esd.o

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
INCLUDE   = -I../include -I./include
CFLAGS   += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS   = -g -L../lib/ $(LDPATH)
DRVLIBS   = $(lib_esd) $(lib_ossaudio) $(lib_ao) $(lib_shout) $(lib_sndio)
CODECLIBS = $(lib_vorbisfile) $(lib_vorbisenc) $(lib_vorbis) $(lib_celt) $(lib_fishsound) $(lib_speex) $(lib_speexdsp)
CONTLIBS  = $(lib_oggz) $(lib_sndfile) $(lib_ogg)
OWNLIBS   = $(LIBROAREIO) $(LIBROARLIGHT) $(LIBROARMIDI) $(LIBROARDSP) $(LIBROAR)
LIBS      = $(DRVLIBS) $(CODECLIBS) $(CONTLIBS) $(OWNLIBS)

all: ${TARGETS}
	rm -f ../lib/${TARGETS} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${SUBSYS} ${DRV} ${CF} ${EMUL}
	${CC} ${LDFLAGS} -o roard ${OBJ} ${SUBSYS} ${DRV} ${CF} ${EMUL} ${LIBS}

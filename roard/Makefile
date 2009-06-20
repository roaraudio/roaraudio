TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o meta.o codecfilter.o midi.o light.o lib.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o driver_oss.o driver_shout.o driver_sndio.o driver_dmx.o driver_pwmled.o
CF=codecfilter_cmd.o codecfilter_vorbis.o codecfilter_celt.o codecfilter_speex.o codecfilter_wave.o codecfilter_alaw.o codecfilter_mulaw.o codecfilter_sndfile.o codecfilter_fishsound.o

include ../Makefile.conf

#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
INCLUDE   = -I../include -I./include
CFLAGS   += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS   = -g -L../lib/ $(LDPATH)
NETLIBS   = $(lib_socket) $(lib_wsock32) $(lib_ws2_32)
DRVLIBS   = $(lib_esd) $(lib_ossaudio) $(lib_ao) $(lib_shout) $(lib_sndio)
CODECLIBS = $(lib_vorbisfile) $(lib_vorbisenc) $(lib_vorbis) $(lib_celt) $(lib_fishsound) $(lib_speex) $(lib_speexdsp)
CONTLIBS  = $(lib_oggz) $(lib_sndfile) $(lib_ogg)
PROTOLIBS = $(lib_slp)
LIBS      = -lroarlight -lroarmidi -lroardsp -lroar $(DRVLIBS) $(CODECLIBS) $(CONTLIBS) $(PROTOLIBS) $(lib_m) $(NETLIBS)

all: ${TARGETS}
	rm -f ../lib/${TARGETS} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${DRV} ${CF}
	${CC} ${LDFLAGS} -o roard ${OBJ} ${DRV} ${CF} ${LIBS}

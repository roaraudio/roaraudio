TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o meta.o codecfilter.o lib.o ssynth.o container_framework.o plugins.o memlock.o hwmixer.o auth.o
SUBSYS=midi.o light.o raw.o rdtcs.o waveform.o beep.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o driver_oss.o driver_shout.o driver_sndio.o driver_dmx.o driver_pwmled.o driver_sysclock.o driver_wmm.o driver_dstr.o driver_alsa.o driver_rsound.o driver_portaudio.o driver_pulsesimple.o driver_jack.o
CF_CODEC=codecfilter_cmd.o codecfilter_vorbis.o codecfilter_celt.o codecfilter_speex.o codecfilter_alaw.o codecfilter_mulaw.o codecfilter_sndfile.o codecfilter_fishsound.o
CF_CONTAINER=codecfilter_wave.o codecfilter_raum.o codecfilter_au.o
EMUL=emul_esd.o emul_simple.o emul_rsound.o emul_rplay.o
HWMIXER=hwmixer_dstr.o hwmixer_oss.o

CF=${CF_CONTAINER} ${CF_CODEC}

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
PKGDEFINES= -DPACKAGE_VERSION="\"$(VERSION)\"" -DDEVICE_VENDOR_ID=$(DEV_VENDOR) -DDEVICE_VENDOR_NAME="\"$(DEV_VENDOR_NAME)\"" -DDEVICE_VENDOR_STRING="\"$(DEV_VENDOR_STRING)\""
INCLUDE   = -I../include -I./include
CFLAGS   += -g -Wall -O2 $(DEFINES) $(PKGDEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS   = -g -rdynamic -L../lib/ $(LDPATH)
DRVLIBS   = $(lib_esd) $(lib_ossaudio) $(lib_ao) $(lib_shout) $(lib_sndio) $(lib_rsound) $(lib_portaudio) $(lib_pablio) $(lib_asound) $(lib_winmm) $(lib_pulse_simple) $(lib_jack)
CODEC_CELT= $(lib_celt) $(lib_celt0)
CODECLIBS = $(lib_vorbisfile) $(lib_vorbisenc) $(lib_vorbis) $(lib_fishsound) $(lib_speex) $(lib_speexdsp) $(CODEC_CELT)
CONTLIBS  = $(lib_oggz) $(lib_sndfile) $(lib_ogg) $(lib_raum)
OWNLIBS   = $(LIBROAREIO) $(LIBROARLIGHT) $(LIBROARMIDI) $(LIBROARDSP) $(LIBROAR)
LIBS      = $(DRVLIBS) $(CODECLIBS) $(CONTLIBS) $(OWNLIBS)

all: ${TARGETS}
	rm -f ../lib/${TARGETS} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${SUBSYS} ${DRV} ${CF} ${EMUL} ${HWMIXER}
	${CC} ${LDFLAGS} -o roard ${OBJ} ${SUBSYS} ${DRV} ${CF} ${EMUL} ${HWMIXER} ${LIBS}

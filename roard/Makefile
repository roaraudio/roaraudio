TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o meta.o codecfilter.o midi.o lib.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o
CF=codecfilter_cmd.o codecfilter_vorbis.o codecfilter_celt.o codecfilter_speex.o codecfilter_wave.o

include ../Makefile.conf

#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH)
LDFLAGS = -g -L../lib/ $(LDPATH) -lroar -lroardsp $(lib_esd) $(lib_ao) $(lib_vorbisfile) $(lib_vorbisenc) -lm $(lib_celt) $(lib_speex)

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${DRV} ${CF}

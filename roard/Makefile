TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o meta.o codecfilter.o midi.o lib.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o
CF=codecfilter_cmd.o codecfilter_vorbis.o codecfilter_celt.o

CELT_PREFIX=/home/phi/superhome/xiph/celt-test/
#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) -I$(CELT_PREFIX)/include
LDFLAGS = -g -L../lib/ -lroar -lesd -lao -lvorbisfile -lm -L$(CELT_PREFIX)/lib -lcelt

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${DRV} ${CF}

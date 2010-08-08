include ../Makefile.conf
include ../Makefile.inc

SLIB=libroardsp$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroardsp.a
OBJS=libroardsp.o convert.o midside.o poly.o filter.o filterchain.o remove.o transcode.o vio_transcode.o rms.o fader.o mixer.o amp.o interleave.o channels.o resampler_poly3.o
OLDROAR=midi.o
FILTER=filter_lowp.o filter_highp.o filter_amp.o filter_quantify.o filter_add.o filter_clip.o filter_downmix.o filter_dcblock.o filter_swap.o filter_agc.o filter_speex_prep.o
MIDI=synth.o
CODECS=alaw.o mulaw.o
TRANSCODERS=transcode_mualaw.o transcode_celt.o transcode_speex.o

ALLOBJS=$(OBJS) $(FILTER) $(TRANSCODERS) $(OLDROAR) $(MIDI) $(CODECS)

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroardsp
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)

LIBS      = $(LIBROARDSP_NS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): $(ALLOBJS)
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(LIBROARDSP_V) -o $(SLIB) $(ALLOBJS) $(LIBS)
libroardsp.a: $(ALLOBJS)
	${AR} cru libroardsp.a $(ALLOBJS)
	${RANLIB} libroardsp.a

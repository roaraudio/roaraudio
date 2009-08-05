include ../Makefile.conf

SLIB=libroardsp$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroardsp.a
OBJS=libroardsp.o convert.o midside.o poly.o filter.o filterchain.o alaw.o mulaw.o remove.o transcode.o
OLDROAR=midi.o
FILTER=lowp.o highp.o amp.o quantify.o add.o clip.o downmix.o dcblock.o swap.o
TRANSCODERS=transcode_mualaw.o transcode_celt.o transcode_speex.o

ALLOBJS=$(OBJS) $(FILTER) $(TRANSCODERS) $(OLDROAR)

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroardsp
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)

CODECLIBS = $(lib_celt)
NETLIBS   = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
LIBS      = $(lib_dnet) $(lib_m) -lroar $(CODECLIBS) $(NETLIBS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): $(ALLOBJS)
	${CC} ${LDFLAGS} -o $(SLIB) $(ALLOBJS) $(LIBS)
libroardsp.a: $(ALLOBJS)
	${AR} cru libroardsp.a $(ALLOBJS)
	${RANLIB} libroardsp.a

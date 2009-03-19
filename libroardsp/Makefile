include ../Makefile.conf

TARGETS=libroardsp.so
OBJS=libroardsp.o filter.o filterchain.o alaw.o mulaw.o
OLDROAR=midi.o
FILTER=lowp.o highp.o amp.o quantify.o add.o clip.o downmix.o dcblock.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroardsp
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) -fPIC
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH) $(lib_dnet) $(lib_m) -lroar

all: ${TARGETS}
	rm -f ../lib/${TARGETS} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroardsp.so: ${OBJS} $(FILTER) $(OLDROAR)
	${CC} ${LDFLAGS} -o libroardsp.so ${OBJS} $(FILTER) $(OLDROAR)

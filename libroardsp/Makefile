include ../Makefile.conf

TARGETS=libroardsp.so
OBJS=libroardsp.o filter.o filterchain.o alaw.o
OLDROAR=midi.o
FILTER=lowp.o highp.o amp.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroardsp
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) -fPIC
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH) $(lib_dnet) -lm -lroar

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroardsp.so: ${OBJS} $(FILTER) $(OLDROAR)
	${CC} ${LDFLAGS} -o libroardsp.so ${OBJS} $(FILTER) $(OLDROAR)

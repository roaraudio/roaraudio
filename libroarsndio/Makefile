include ../Makefile.conf

SLIB=libroarsndio$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroarsndio.a
OBJS=libroarsndio.o para.o stream.o events.o volume.o midi.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarsndio
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../lib/
LIBS    = -lroar $(lib_wsock32) $(lib_ws2_32)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -o $(SLIB) ${OBJS} ${LIBS}
libroarsndio.a: ${OBJS}
	${AR} cru libroarsndio.a ${OBJS}
	${RANLIB} libroarsndio.a

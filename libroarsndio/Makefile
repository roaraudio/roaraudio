include ../Makefile.conf

SLIB=libroarsndio.so

TARGETS=$(SLIB) libroarsndio.a
OBJS=libroarsndio.o para.o stream.o events.o volume.o

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

libroarsndio.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroarsndio.so ${OBJS} ${LIBS}
libroarsndio.a: ${OBJS}
        ${AR} cru libroarsndio.a ${OBJS}

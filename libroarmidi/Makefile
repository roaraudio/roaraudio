include ../Makefile.conf

SLIB=libroarmidi.so

TARGETS=libroarmidi.so libroarmidi.a
OBJS=libroarmidi.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarmidi
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
NETLIBS = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
LIBS    = -lroardsp -lroar $(lib_m) $(NETLIBS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarmidi.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroarmidi.so ${OBJS} $(LIBS)
libroarmidi.a: ${OBJS}
	${AR} cru libroarmidi.a ${OBJS}
	${RANLIB} libroarmidi.a

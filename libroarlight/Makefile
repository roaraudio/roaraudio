include ../Makefile.conf

SLIB=libroarlight.so

TARGETS=libroarlight.so libroarlight.a
OBJS=libroarlight.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarlight
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
LIBS    = -lroar $(lib_dnet) $(lib_m)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarlight.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroarlight.so ${OBJS} $(LIBS)
libroarlight.a: ${OBJS}
	${AR} cru libroarlight.a ${OBJS}
	${RANLIB} libroarlight.a

include ../Makefile.conf
include ../Makefile.inc

SLIB=libroarlight$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroarlight.a
OBJS=libroarlight.o colors.o roardmx.o pwm.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarlight
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
LIBS    = $(LIBROARLIGHT_NS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -o $(SLIB) ${OBJS} $(LIBS)
libroarlight.a: ${OBJS}
	${AR} cru libroarlight.a ${OBJS}
	${RANLIB} libroarlight.a

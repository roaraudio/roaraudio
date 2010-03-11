include ../Makefile.conf
include ../Makefile.inc

SLIB=libroarmidi$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroarmidi.a
OBJS=libroarmidi.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarmidi
CFLAGS += -g -Wall -Wextra -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
LIBS    = $(LIBROARMIDI_NS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(LIBROARMIDI_V) -o $(SLIB) ${OBJS} ${LIBS}
libroarmidi.a: ${OBJS}
	${AR} cru libroarmidi.a ${OBJS}
	${RANLIB} libroarmidi.a

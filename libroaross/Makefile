include ../Makefile.conf
include ../Makefile.inc

SLIB=libroaross$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroaross.a
OBJS=libroaross.o

DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroar
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../lib
LIBS    = $(LIBROAR) $(lib_dl)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(COMMON_V_MAJOR) -o $(SLIB) ${OBJS} ${LIBS}
libroaross.a: ${OBJS}
	${AR} cru libroaross.a ${OBJS}
	${RANLIB} libroaross.a

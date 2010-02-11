TARGETS=libroarpulse.so libroarpulseinfo
OBJS_ROAR   = libroarpulse.o
OBJS_STD    = util.o context.o xmalloc.o version.o operation.o introspect.o sample.o channelmap.o
OBJS_SIMPLE = simple.o
OBJS=$(OBJS_ROAR) $(OBJS_SIMPLE) $(OBJS_STD)

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(LDPATH) -L../lib/
LIBS    = $(LIBROAR)

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarpulse.so: ${OBJS}
	${CC} ${LDFLAGS} $(SHARED) -Wl,-soname,libroarpulse$(SHARED_SUFFIX).$(COMMON_V_MAJOR) -o libroarpulse$(SHARED_SUFFIX) ${OBJS} ${LIBS}

libroarpulseinfo: libroarpulseinfo.o

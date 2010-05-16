LIB_PREFIX=libroarpulse
LIB_STDAPI=$(LIB_PREFIX)$(SHARED_SUFFIX)
LIB_SIMPLE=$(LIB_PREFIX)-simple$(SHARED_SUFFIX)

TARGETS=$(LIB_STDAPI) $(LIB_SIMPLE) libroarpulseinfo
OBJS_ROAR   = libroarpulse.o
OBJS_STD    = util.o error.o timeval.o context.o xmalloc.o version.o operation.o introspect.o sample.o channelmap.o stream.o volume.o mainloop.o mainloop-signal.o utf8.o x11.o
OBJS_SIMPLE = simple.o
OBJS_STDAPI = $(OBJS_ROAR) $(OBJS_STD)

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(LDPATH) -L../lib/ -L.
LIBS    = $(LIBROAR)

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(LIB_STDAPI): ${OBJS_STDAPI}
	${CC} ${LDFLAGS} $(SHARED) -Wl,-soname,$(LIB_STDAPI).$(COMMON_V_MAJOR) -o $(LIB_STDAPI) ${OBJS_STDAPI} ${LIBS}

$(LIB_SIMPLE): ${OBJS_SIMPLE} $(LIB_STDAPI)
	${CC} ${LDFLAGS} $(SHARED) -Wl,-soname,$(LIB_SIMPLE).$(COMMON_V_MAJOR) -o $(LIB_SIMPLE) ${OBJS_SIMPLE} ${LIBS} -lroarpulse


libroarpulseinfo: libroarpulseinfo.o

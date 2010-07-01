include ../Makefile.conf
include ../Makefile.inc

#TARGETS=libartsc$(SHARED_SUFFIX) libroarartsc$(SHARED_SUFFIX)
TARGETS=libroarartsc$(SHARED_SUFFIX)
OBJS=libartsc.o

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

#libartsc$(SHARED_SUFFIX): libroarartsc$(SHARED_SUFFIX)
#	cp libroarartsc$(SHARED_SUFFIX) libartsc$(SHARED_SUFFIX)
libroarartsc$(SHARED_SUFFIX): ${OBJS}
	${CC} ${LDFLAGS} $(SHARED) -Wl,-soname,libroarartsc$(SHARED_SUFFIX).$(COMMON_V_MAJOR) -o libroarartsc$(SHARED_SUFFIX) ${OBJS} ${LIBS}

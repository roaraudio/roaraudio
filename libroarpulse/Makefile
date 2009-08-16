TARGETS=libroarpulse.so libroarpulseinfo
OBJS=libroarpulse.o simple.o util.o

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
	${CC} ${LDFLAGS} $(SHARED) -o libroarpulse.so ${OBJS} ${LIBS}

libroarpulseinfo: libroarpulseinfo.o

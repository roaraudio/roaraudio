TARGETS=libartsc.so libroarartsc.so
OBJS=libartsc.o

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

libartsc.so: libroarartsc.so
	cp libroarartsc.so libartsc.so
libroarartsc.so: ${OBJS}
	${CC} ${LDFLAGS} $(SHARED) -o libroarartsc.so ${OBJS} ${LIBS}

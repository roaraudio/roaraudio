TARGETS=libartsc.so
OBJS=libartsc.o

include ../Makefile.conf

DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) -fPIC
LDFLAGS = -g -lroar $(LDPATH) -L../lib/

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libartsc.so: ${OBJS}
	${CC} ${LDFLAGS} $(SHARED) -o libartsc.so ${OBJS}

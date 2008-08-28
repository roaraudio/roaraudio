TARGETS=libroaryiff.so
OBJS=libroaryiff.o connection.o playback.o audiocd.o ctl.o

include ../Makefile.conf

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroaryiff
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) -fPIC
LDFLAGS = -g $(SHARED) -lroar $(LDPATH) -L../lib/

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroaryiff.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroaryiff.so ${OBJS}

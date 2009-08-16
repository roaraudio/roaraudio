TARGETS=libroaryiff.so
OBJS=libroaryiff.o connection.o playback.o audiocd.o ctl.o stub.o file.o events.o

include ../Makefile.conf
include ../Makefile.inc

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroaryiff
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../lib/
LIBS    = $(LIBROAR)

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroaryiff.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroaryiff.so ${OBJS} ${LIBS}

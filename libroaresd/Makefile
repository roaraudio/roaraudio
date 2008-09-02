include ../Makefile.conf

TARGETS=libroaresd.so
OBJS=libroaresd.o esdbasic.o esdctl.o esdstream.o esdsample.o esdfile.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroaresd
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) -fPIC
LDFLAGS = -g $(SHARED) -lroar $(LDPATH) -L../lib/

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroaresd.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroaresd.so ${OBJS}

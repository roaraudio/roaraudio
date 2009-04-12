include ../Makefile.conf

TARGETS=libroarsndio.so
OBJS=libroarsndio.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarsndio
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -lroar $(LDPATH) -L../lib/

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarsndio.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroarsndio.so ${OBJS}

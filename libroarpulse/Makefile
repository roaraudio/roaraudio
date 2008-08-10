TARGETS=libroarpulse.so libroarpulseinfo
OBJS=libroarpulse.o simple.o util.o

DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -lroar -L../lib/

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarpulse.so: ${OBJS}
	${CC} ${LDFLAGS} -shared -o libroarpulse.so ${OBJS}

libroarpulseinfo: libroarpulseinfo.o

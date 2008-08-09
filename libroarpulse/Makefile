TARGETS=libroarpulse.so
OBJS=libroarpulse.o simple.o

DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -shared -lroar -L../lib/

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroarpulse.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroarpulse.so ${OBJS}

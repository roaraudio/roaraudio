TARGETS=libartsc.so
OBJS=libartsc.o

DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarpulse
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -lroar -L../lib/

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libartsc.so: ${OBJS}
	${CC} ${LDFLAGS} -shared -o libartsc.so ${OBJS}

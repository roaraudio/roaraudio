TARGETS=libroar.so
OBJS=libroar.o basic.o stream.o simple.o auth.o socket.o ctl.o buffer.o convert.o math.o meta.o file.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroar
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -shared

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroar.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroar.so ${OBJS}

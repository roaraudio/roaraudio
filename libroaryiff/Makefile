TARGETS=libroaryiff.so
OBJS=libroaryiff.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroaryiff
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -shared -lroar -L../lib/

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroaryiff.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroaryiff.so ${OBJS}

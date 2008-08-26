include ../Makefile.conf

TARGETS=libroardsp.so
OBJS=libroardsp.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroardsp
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) -fPIC
LDFLAGS = -g -shared -L../lib/ $(LDPATH) $(lib_dnet) -lm -lroar

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroardsp.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroardsp.so ${OBJS}

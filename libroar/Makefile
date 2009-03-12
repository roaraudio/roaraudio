include ../Makefile.conf

TARGETS=libroar.so
VIO=vio.o vio_cmd.o vio_ops.o vio_magic.o
OBJS=libroar.o basic.o stream.o simple.o auth.o socket.o ctl.o buffer.o convert.o poly.o meta.o file.o acl.o cdrom.o pinentry.o sshaskpass.o $(VIO) stack.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroar
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) -fPIC
LDFLAGS = -g $(SHARED) $(LDPATH) $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)

all: ${TARGETS}
	rm -f ../lib/${TARGETS} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

libroar.so: ${OBJS}
	${CC} ${LDFLAGS} -o libroar.so ${OBJS}

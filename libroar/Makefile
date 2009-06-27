include ../Makefile.conf

SLIB=libroar$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroar.a
VIO=vio.o vio_cmd.o vio_ops.o vio_magic.o vio_pipe.o vio_socket.o vio_bio.o vio_stack.o vio_proto.o vio_dstr.o vio_select.o
OBJS=libroar.o basic.o stream.o simple.o auth.o socket.o ctl.o buffer.o meta.o file.o acl.o cdrom.o pinentry.o sshaskpass.o $(VIO) stack.o slp.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroar
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH)
NETLIBS = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
LIBS    = $(lib_slp) $(lib_ssl) $(lib_sendfile) $(NETLIBS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -o $(SLIB) ${OBJS} ${LIBS}
libroar.a: ${OBJS}
	${AR} cru libroar.a ${OBJS}
	${RANLIB} libroar.a

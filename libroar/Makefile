include ../Makefile.conf
include ../Makefile.inc

SLIB=libroar$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroar.a
VIO_META=vio.o vio_select.o vio_ops.o vio_string.o
VIO_BRIDGE=vio_bio.o vio_stdio.o
VIO=$(VIO_META) $(VIO_BRIDGE) vio_cmd.o vio_magic.o vio_pipe.o vio_socket.o vio_winsock.o vio_stack.o vio_jumbo.o vio_proto.o vio_dstr.o vio_tantalos.o vio_rtp.o vio_buffer.o vio_buffer_store.o
HASHES=hash.o hash_tiger.o
CRYPTO=crypto.o random.o $(HASHES)
OBJS=libroar.o config.o debug.o error.o basic.o stream.o client.o simple.o auth.o socket.o ctl.o buffer.o meta.o file.o acl.o cdrom.o pinentry.o sshaskpass.o $(VIO) stack.o slp.o nnode.o roardl.o roarx11.o beep.o proto.o env.o keyval.o vs.o ltm.o notify.o notify_proxy.o asyncctl.o enumdev.o serverinfo.o $(CRYPTO)

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroar
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH)
LIBS    = $(LIBROAR_NS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(LIBROAR_V) -o $(SLIB) ${OBJS} ${LIBS}
libroar.a: ${OBJS}
	${AR} cru libroar.a ${OBJS}
	${RANLIB} libroar.a

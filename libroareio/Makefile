include ../Makefile.conf

SLIB=libroareio$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroareio.a
OBJS=libroareio.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroareio
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
NETLIBS = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
LIBS    = -lroar $(NETLIBS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -o $(SLIB) ${OBJS} $(LIBS)
libroareio.a: ${OBJS}
	${AR} cru libroareio.a ${OBJS}
	${RANLIB} libroareio.a

include ../Makefile.conf
include ../Makefile.inc

LIB_BASENAME=libroaresd
SLIB=$(LIB_BASENAME)$(SHARED_SUFFIX)

TARGETS=$(SLIB) $(LIB_BASENAME).a
OBJS=libroaresd.o esdbasic.o esdctl.o esdstream.o esdsample.o esdfile.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroaresd
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../lib/
LIBS    = $(LIBROAR)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -o $(SLIB) ${OBJS} ${LIBS}
$(LIB_BASENAME).a: ${OBJS}
	${AR} cru $(LIB_BASENAME).a ${OBJS}
	${RANLIB} $(LIB_BASENAME).a

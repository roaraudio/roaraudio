include ../Makefile.conf
include ../Makefile.inc

SLIB=libroarrsound$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroarrsound.a
OBJS=libroarrsound.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroarrsound
CFLAGS += -g -Wall $(Wextra) -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../lib/
LIBS    = $(LIBROAR)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(COMMON_V_MAJOR) -o $(SLIB) ${OBJS} ${LIBS}
libroarrsound.a: ${OBJS}
	${AR} cru libroarrsound.a ${OBJS}
	${RANLIB} libroarrsound.a

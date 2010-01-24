include ../Makefile.conf
include ../Makefile.inc

SLIB=libroareio$(SHARED_SUFFIX)

TARGETS=$(SLIB) libroareio.a
DRIVER=driver.o driver_oss.o esdfw.a
FF=ff_ssdp.o
OBJS=libroareio.o $(DRIVER) $(FF)

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I../include/libroareio
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC)
LDFLAGS = -g $(SHARED) -L../lib/ $(LDPATH)
LIBS    = $(LIBROAREIO_NS)

all: ${TARGETS}
	rm -f ../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} autodetected.h esdfw.a *.o
new: clean all

esdfw.a:
	if [ "`ls esdfw_*.o`" != '' ]; then ${AR} cru esdfw.a esdfw_*.o; else ${AR} cru esdfw.a; fi
	${RANLIB} esdfw.a

$(SLIB): ${OBJS}
	${CC} ${LDFLAGS} -Wl,-soname,$(SLIB).$(COMMON_V_MAJOR) -o $(SLIB) $(OBJS) $(LIBS)
libroareio.a: ${OBJS}
	${AR} cru libroareio.a ${OBJS}
	${RANLIB} libroareio.a

driver.c: autodetected.h
autodetected.h:
	echo "/* *** */" > autodetected.h
	[ -f driver_esdfw.h -a -f driver_esdfw.c ] && echo '#define ROAR_HAVE_AD_ESDFW' >> autodetected.h || true
	echo "/* *** */" >> autodetected.h

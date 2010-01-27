TARGET=libroar.so
OBJ=ao_roar.o

include ../../Makefile.conf
include ../../Makefile.inc
include Makefile.conf

#DEFINES=-DDEBUG
CFLAGS += -g -Wall -O2 -I../../include $(DEFINES) $(HAVE_MATRIX) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) $(AO_CFLAGS)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../../lib
LIBS    = $(LIBROAR) $(AO_LIBS)

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o tests tests.c
new: clean all

install: libroar.so
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	cp libroar.so ${DESTDIR}${INSTALL_DIR}
semi-install: libroar.so
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	ln -fs `pwd`/libroar.so ${DESTDIR}${INSTALL_DIR}

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -Wl,-soname,libroar.so -o libroar.so $(OBJ) $(LIBS)

ao_roar.c:

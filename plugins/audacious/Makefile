TARGET=libroar.so

OBJ=roar.o gui.o

include ../../Makefile.conf
include ../../Makefile.inc
include Makefile.conf

#DEFINES=-DDEBUG
CFLAGS += -g -Wall -O2 -I../../include $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) $(GTK_CFLAFS) $(XMMS_CFLAGS)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../../lib
LIBS    = $(LIBROAR) $(GTK_LIBS) $(XMMS_LIBS)

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o *~
new: clean all

install: libroar.so
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	cp libroar.so ${DESTDIR}${INSTALL_DIR}/
semi-install: libroar.so
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	ln -fs `pwd`/libroar.so ${DESTDIR}${INSTALL_DIR}/

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -Wl,-soname,libroar.so -o libroar.so $(OBJ) $(LIBS)

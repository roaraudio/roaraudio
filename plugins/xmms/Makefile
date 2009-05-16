TARGET=libroar.so

OBJ=roar.o gui.o

include ../../Makefile.conf
include Makefile.conf

#DEFINES=-DDEBUG
CFLAGS += -g -Wall -O2 -I../../include $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) $(GTK_CFLAFS) $(XMMS_CFLAGS)
LDFLAGS = -g $(SHARED) $(LDPATH) -lroar $(GTK_LIBS) $(XMMS_LIBS)

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o
new: clean all

install: libroar.so
	cp libroar.so ${INSTALL_DIR}/
semi-install: libroar.so
	ln -fs `pwd`/libroar.so ${INSTALL_DIR}/

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -o libroar.so $(OBJ)

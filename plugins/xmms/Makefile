TARGET=libroar.so

OBJ=roar.o


GTK_LIBS=`gtk-config --libs`
GTK_CFLAFS=`gtk-config --cflags`
XMMS_LIBS=`xmms-config --libs`
XMMS_CFLAGS=`xmms-config --cflags`
INSTALL_DIR=`xmms-config --output-plugin-dir`

CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) -fPIC $(GTK_CFLAFS) $(XMMS_CFLAGS)
LDFLAGS = -g -shared -lroar $(GTK_LIBS) $(XMMS_LIBS)

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o
new: clean all

install: libroar.so
	cp libroar.so ${INSTALL_DIR}/

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -o libroar.so $(OBJ)

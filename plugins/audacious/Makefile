TARGET=libroar.so

OBJ=roar.o gui.o

include Makefile.conf

DEFINES=-DDEBUG
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) $(fPIC) $(XMMS_CFLAGS) $(GTK_CFLAFS) 
LDFLAGS = -g -shared -lroar  $(XMMS_LIBS) $(GTK_LIBS) 

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o *~
new: clean all

install: libroar.so
	cp libroar.so ${INSTALL_DIR}/
semi-install: libroar.so
	ln -fs `pwd`/libroar.so ${INSTALL_DIR}/

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -o libroar.so $(OBJ)

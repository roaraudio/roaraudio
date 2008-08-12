TARGET=libroar.so

OBJ=roar.o

CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE) -fPIC
LDFLAGS = -g -shared -lroar

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o
new: clean all

install: libroar.so
	cp libroar.so ${INSTALL_DIR}

libroar.so: $(OBJ)
	$(CC) $(LDFLAGS) -o libroar.so $(OBJ)

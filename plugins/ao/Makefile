TARGET=libroar.so

all: ${TARGET}
clean:
	rm -f ${TARGET} tests tests.c
new: clean all

install: libroar.so
	cp libroar.so ${INSTALL_DIR}
semi-install: libroar.so
	ln -s `pwd`/libroar.so ${INSTALL_DIR}

libroar.so: ao_roar.c
	cc -shared -L../../lib -I../../include -lroar -fPIC -o libroar.so ao_roar.c

ao_roar.c:

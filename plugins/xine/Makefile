TARGET=xineplug_ao_out_roar.so

OBJ=roar.o


XINE_LIBS=`xine-config --libs`
XINE_CFLAGS=`xine-config --cflags`
INSTALL_DIR=`xine-config --plugindir`

#DEFINES=-DDEBUG
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(fPIC) $(XINE_CFLAGS)
LDFLAGS = -g -shared -lroar $(XINE_LIBS)

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o
new: clean all

install: ${TARGET}
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	cp ${TARGET} ${DESTDIR}${INSTALL_DIR}/

xineplug_ao_out_roar.so: $(OBJ)
	$(CC) $(LDFLAGS) -o xineplug_ao_out_roar.so $(OBJ)

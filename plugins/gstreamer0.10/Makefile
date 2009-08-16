include ../../Makefile.conf
include ../../Makefile.inc
include Makefile.conf

SLIB=libgstroar.so
TARGETS=$(SLIB)

OBJS=gstroar.o roarsink.o roarmixer.o

#DEFINES        = -DDEBUG
INCLUDE = -I. -I../include -I../include/libroar
CFLAGS += -g -Wall -O2 $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) $(GST_CFLAGS)
LDFLAGS = -g -L../lib/ $(SHARED) $(LDPATH) $(LIBROAR) $(GST_LIBS) -lgstaudio-0.10

all: $(TARGETS)
	rm -f ../../lib/${SLIB} 2> /dev/null || true
	cp $(cp_v) ${TARGETS} ../../lib/
clean:
	rm -f $(TARGETS) *.o
new: clean all

libgstroar.so: $(OBJS)
	$(CC) $(LDFLAGS) -o libgstroar.so $(OBJS)

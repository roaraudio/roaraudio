BASENAME=libasound_module_roar
SLIB=$(BASENAME)$(SHARED_SUFFIX)
TARGET=$(SLIB)
OBJS=roar.o pcm.o mixer.o

include ../../Makefile.conf
include ../../Makefile.inc
#include Makefile.conf

#DEFINES=-DDEBUG
CFLAGS += -g -Wall -O2 -I../../include $(DEFINES) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) -I/usr/include/alsa -D_GNU_SOURCE
LDFLAGS = -g $(SHARED) $(LDPATH) -L../../lib -module -avoid-version -export-dynamic
LIBS    = $(LIBROAR) -lasound

all: ${TARGET}
clean:
	rm -f ${TARGET} *.o tests tests.c
new: clean all

install: ${TARGET}
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	cp ${TARGET} ${DESTDIR}${INSTALL_DIR}
semi-install: ${TARGET}
	mkdir -p ${DESTDIR}${INSTALL_DIR}
	ln -fs `pwd`/$(SLIB) ${DESTDIR}${INSTALL_DIR}

$(SLIB): $(OBJS)
	$(CC) $(LDFLAGS) -Wl,-soname,$(SLIB) -o $(SLIB) $(OBJS) $(LIBS)

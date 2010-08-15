BASENAME=libasound_module_pcm_roar
SLIB=$(BASENAME)$(SHARED_SUFFIX)
TARGET=$(SLIB)
INSTALL_DIR=$(PREFIX_LIB)/alsa-lib/
OBJS=pcm_roar.o roar.o thread.o

include ../../Makefile.conf
include ../../Makefile.inc
#include Makefile.conf

#DEBUG=-DDEBUG -DXXX
DEFINES=-DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"alsa-plugins\" -DVERSION=\"1.0.13\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DSTDC_HEADERS=1 -DHAVE_LIBASOUND=1 -D_REENTRANT
CFLAGS += -g -Wall $(Wextra) -O2 -I../../include $(DEFINES) $(DEBUG) $(INCLUDE) $(INCPATH) $(SHARED_CF) $(fPIC) -I/usr/include/alsa -D_GNU_SOURCE $(pthread)
LDFLAGS = -g $(SHARED) $(LDPATH) -L../../lib $(pthread)
LIBS    = $(LIBROAR) $(lib_m) $(lib_dl) -lrt $(lib_asound)

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
	$(CC) $(LDFLAGS) -Wl,-soname -Wl,$(SLIB) -o $(SLIB) $(OBJS) $(LIBS)

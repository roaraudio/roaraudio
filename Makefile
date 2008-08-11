include Makefile.conf

SUBDIRS=libroar $(subdir_libroaresd) roard roarclients $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse)

all:
	for i in ${SUBDIRS}; do make -C $$i all; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do make -C $$i clean; done;

new: clean all

include Makefile.conf

SUBDIRS=libroar $(subdir_libroaresd) roard roarclients $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

all:
	for i in ${SUBDIRS}; do $(MAKE) -C $$i all; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do $(MAKE) -C $$i clean; done;

new: clean all

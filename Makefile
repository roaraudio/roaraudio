include Makefile.conf

SUBDIRS=libroar libroardsp $(subdir_libroaresd) roard roarclients $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

all:
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) all; cd ..; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) clean; cd ..; done;

new: clean all

SUBDIRS=libroar libroaresd roard roarclients roarfish libroaryiff

all:
	for i in ${SUBDIRS}; do make -C $$i all; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do make -C $$i clean; done;

new: clean all

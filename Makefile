SUBDIRS=libroar libroaresd roard roarclients

all:
	for i in ${SUBDIRS}; do make -C $$i all; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do make -C $$i clean; done;

new: clean all

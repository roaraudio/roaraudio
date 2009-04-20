include Makefile.conf

SUBDIRS=libroar libroardsp $(subdir_libroaresd) roard roarclients doc $(subdir_libroarsndio) $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts) $(subdir_plugins_ao)

all:
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) all; cd ..; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) clean; cd ..; done;

new: clean all

test: all
	cd tests; make test; cd ..;

prep-install-dirs:
	mkdir -p $(PREFIX_BIN)
	mkdir -p $(PREFIX_LIB)
	mkdir -p $(PREFIX_INC)
	mkdir -p $(PREFIX_MAN)
	cd doc; make prep-install-dirs; cd ..

install: prep-install-dirs
	cp $(cp_v) lib/roar*     $(PREFIX_BIN)
	cp $(cp_v) lib/lib*.so*  $(PREFIX_LIB)
	sh -c 'for file in include/roar* include/lib*; do cp $(cp_v) -r $$file $(PREFIX_INC)/; done'
	cd doc; make install; cd ..
	if [ "$(subdir_plugins_ao)" != '' ]; then cd $(subdir_plugins_ao); make install; cd ../..; fi
semi-install: prep-install-dirs
	sh -c 'for file in lib/roar*;    do ln -fs `pwd`/$$file $(PREFIX_BIN)/; done'
	sh -c 'for file in lib/lib*.so*; do ln -fs `pwd`/$$file $(PREFIX_LIB)/; done'
	sh -c 'for file in include/roar* include/lib*; do ln -fs `pwd`/$$file $(PREFIX_INC)/; done'
	cd doc; make semi-install; cd ..
	if [ "$(subdir_plugins_ao)" != '' ]; then cd $(subdir_plugins_ao); make semi-install; cd ../..; fi

include Makefile.conf

COMP_LIB=$(subdir_libroaresd) $(subdir_libroarsndio) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

SUBDIRS=$(comp_libs) $(subdir_libroaresd) $(comp_roard) $(comp_clients) $(comp_doc) $(subdir_libroarsndio) $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

PLUGINS=$(subdir_plugins_ao) $(subdir_plugins_xmms) $(subdir_plugins_audacious)

all:
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) all; cd ..; done;
	for i in ${PLUGINS}; do cd $$i; $(MAKE) all; cd ../..; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) clean; cd ..; done;
	for i in ${PLUGINS}; do cd $$i; $(MAKE) clean; cd ../..; done;

new: clean all

test: all
	cd tests; make test; cd ..;

prep-install-dirs:
	mkdir -p $(PREFIX_BIN)
	mkdir -p $(PREFIX_LIB)
	mkdir -p $(PREFIX_INC)
	mkdir -p $(PREFIX_MAN)
	mkdir -p $(PREFIX_COMP)
	cd doc; make prep-install-dirs; cd ..

install: prep-install-dirs
	cp $(cp_v) lib/roar*     $(PREFIX_BIN)
	cp $(cp_v) lib/lib*$(SHARED_SUFFIX)*  $(PREFIX_LIB)
	sh -c 'cd lib; for file in $(COMP_LIB); do for i in "" .{0,1}; do ln -fs $(PREFIX_LIB)/$$file$(SHARED_SUFFIX) $(PREFIX_COMP)/`echo $$file | sed s/roar//`$(SHARED_SUFFIX)$$i; done; done'
	sh -c 'for file in include/roar* include/lib*; do cp $(cp_v) -r $$file $(PREFIX_INC)/; done'
	cd doc; make install; cd ..
	for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; make install; cd ../..; fi; done

semi-install: prep-install-dirs
	sh -c 'for file in lib/roar*;    do ln -fs `pwd`/$$file $(PREFIX_BIN)/; done'
	sh -c 'for file in lib/lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file $(PREFIX_LIB)/; done'
	sh -c 'cd lib; for file in $(COMP_LIB); do for i in "" .{0,1}; do ln -fs `pwd`/$$file$(SHARED_SUFFIX) $(PREFIX_COMP)/`echo $$file | sed s/roar//`$(SHARED_SUFFIX)$$i; done; done'
	sh -c 'for file in include/roar* include/lib*; do ln -fs `pwd`/$$file $(PREFIX_INC)/; done'
	cd doc; make semi-install; cd ..
	for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; make semi-install; cd ../..; fi; done

include Makefile.conf
include Makefile.inc

COMP_LIB=$(subdir_libroaresd) $(subdir_libroarsndio) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

SUBDIRS=$(comp_libs) $(subdir_libroaresd) $(comp_roard) $(comp_clients) $(comp_doc) $(comp_comp_bins) $(subdir_libroarsndio) $(subdir_roarfish) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts)

PLUGINS=$(subdir_plugins_ao) $(subdir_plugins_xmms) $(subdir_plugins_audacious)

all:
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) all; cd ..; done;
	for i in ${PLUGINS}; do cd $$i; $(MAKE) all; cd ../..; done;
clean:
	rm -f lib/* || true
	for i in ${SUBDIRS}; do cd $$i; $(MAKE) clean; cd ..; done;
	for i in ${PLUGINS}; do cd $$i; $(MAKE) clean; cd ../..; done;

distclean: clean
	rm -f Makefile.conf config.log include/roaraudio/config.h

new: clean all

test: all
	cd tests; make test; cd ..;

prep-install-dirs:
	mkdir -p '$(DESTDIR)$(PREFIX_BIN)'
	mkdir -p '$(DESTDIR)$(PREFIX_LIB)'
	mkdir -p '$(DESTDIR)$(PREFIX_INC)'
	mkdir -p '$(DESTDIR)$(PREFIX_MAN)'
	mkdir -p '$(DESTDIR)$(PREFIX_COMP_LIBS)'
	mkdir -p '$(DESTDIR)$(PREFIX_COMP_BINS)'
	cd doc; make prep-install-dirs; cd ..

install: prep-install-dirs
	cp $(cp_v) lib/roar*     '$(DESTDIR)$(PREFIX_BIN)'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do cp $$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_VERSION); done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_VERSION) '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MM); done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_V_MM) '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MAJOR); done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_V_MAJOR) '$(DESTDIR)$(PREFIX_LIB)'/$$file; done'
	sh -c 'cd lib; while read d t; do ln -fs '$(DESTDIR)$(PREFIX_LIB)'/$$d '$(DESTDIR)$(PREFIX_COMP_LIBS)'/$$t; done < ../symlinks.comp'
	sh -c 'cd lib; for file in *.r; do b=`basename $$file .r`; cp $$file '$(DESTDIR)$(PREFIX_COMP_BINS)'/$$b; done'
	sh -c 'for file in include/roar* include/lib*; do cp $(cp_v) -r $$file '$(DESTDIR)$(PREFIX_INC)'/; done'
	cd doc; make install; cd ..
	for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; make install; cd ../..; fi; done

semi-install: prep-install-dirs
	sh -c 'for file in lib/roar*;    do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_BIN)'/; done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/; done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_VERSION); done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MM); done'
	sh -c 'cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MAJOR); done'
	sh -c 'cd lib; while read d t; do ln -fs `pwd`/$$d '$(DESTDIR)$(PREFIX_COMP_LIBS)'/$$t; done < ../symlinks.comp'
	sh -c 'cd lib; for file in *.r; do b=`basename $$file .r`; ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_COMP_BINS)'/$$b; done'
	sh -c 'for file in include/roar* include/lib*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_INC)'/; done'
	cd doc; make semi-install; cd ..
	for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; make semi-install; cd ../..; fi; done

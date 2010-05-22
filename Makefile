include Makefile.conf
include Makefile.inc

COMP_LIB=$(subdir_libroaresd) $(subdir_libroarsndio) $(subdir_libroaryiff) $(subdir_libroarpulse) $(subdir_libroararts) $(subdir_libroaross) $(subdir_libroarrsound)

SUBDIRS=$(comp_libs) $(COMP_LIB) $(comp_comp_bins) $(subdir_roarfish) $(comp_clients) $(comp_roard) $(comp_doc)

PLUGINS=$(subdir_plugins_ao) $(subdir_plugins_xmms) $(subdir_plugins_audacious)

all:
	set -e; for i in ${SUBDIRS}; do cd $$i; $(MAKE) all; cd ..; done;
	set -e; for i in ${PLUGINS}; do cd $$i; $(MAKE) all; cd ../..; done;
clean:
	rm -f lib/* || true
	set -e; for i in ${SUBDIRS}; do cd $$i; $(MAKE) clean; cd ..; done;
	set -e; for i in ${PLUGINS}; do cd $$i; $(MAKE) clean; cd ../..; done;
	set -e; cd tests/; $(MAKE) clean; cd ..;

distclean: clean
	rm -f Makefile.conf config.log include/roaraudio/config.h

new: clean all

test: all
	set -e; cd tests; $(MAKE) test; cd ..;

build-pc-files:
	set -e; for lib in $(comp_libs); do lib/roar-config --output-pc $$lib > lib/$$lib.pc; done

prep-install: prep-install-dirs build-pc-files

prep-install-dirs:
	mkdir -p '$(DESTDIR)$(PREFIX_BIN)'
	mkdir -p '$(DESTDIR)$(PREFIX_LIB)'
	mkdir -p '$(DESTDIR)$(PREFIX_INC)'
	mkdir -p '$(DESTDIR)$(PREFIX_MAN)'
	mkdir -p '$(DESTDIR)$(PREFIX_PC)'
	mkdir -p '$(DESTDIR)$(PREFIX_COMP_LIBS)'
	mkdir -p '$(DESTDIR)$(PREFIX_COMP_BINS)'
	set -e; cd doc; $(MAKE) prep-install-dirs; cd ..

install: prep-install
	cp $(cp_v) lib/roar*     '$(DESTDIR)$(PREFIX_BIN)'
	cp $(cp_v) lib/*.pc      '$(DESTDIR)$(PREFIX_PC)'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do cp $$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_VERSION); done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_VERSION) '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MM); done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_VERSION) '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MAJOR); done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs $$file.$(COMMON_VERSION) '$(DESTDIR)$(PREFIX_LIB)'/$$file; done'
	sh -c 'set -e; cd lib; while read d t; do ln -fs '$(DESTDIR)$(PREFIX_LIB)'/$$d '$(DESTDIR)$(PREFIX_COMP_LIBS)'/$$t; done < ../symlinks.comp'
	sh -c 'set -e; cd lib; for file in *.r; do b=`basename $$file .r`; cp $$file '$(DESTDIR)$(PREFIX_COMP_BINS)'/$$b; done'
	sh -c 'set -e; for file in include/roar* include/lib*; do cp $(cp_v) -r $$file '$(DESTDIR)$(PREFIX_INC)'/; done'
	set -e; cd doc; $(MAKE) install; cd ..
	set -e; for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; $(MAKE) install; cd ../..; fi; done

semi-install: prep-install
	sh -c 'set -e; for file in lib/roar*;    do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_BIN)'/; done'
	sh -c 'set -e; for file in lib/*.pc;     do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_PC)'/; done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/; done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_VERSION); done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MM); done'
	sh -c 'set -e; cd lib; for file in lib*$(SHARED_SUFFIX)*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_LIB)'/$$file.$(COMMON_V_MAJOR); done'
	sh -c 'set -e; cd lib; while read d t; do ln -fs `pwd`/$$d '$(DESTDIR)$(PREFIX_COMP_LIBS)'/$$t; done < ../symlinks.comp'
	sh -c 'set -e; cd lib; for file in *.r; do b=`basename $$file .r`; ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_COMP_BINS)'/$$b; done'
	sh -c 'set -e; for file in include/roar* include/lib*; do ln -fs `pwd`/$$file '$(DESTDIR)$(PREFIX_INC)'/; done'
	set -e; cd doc; $(MAKE) semi-install; cd ..
	set -e; for i in $(PLUGINS); do if [ "$$i" != '' ]; then cd $$i; $(MAKE) semi-install; cd ../..; fi; done

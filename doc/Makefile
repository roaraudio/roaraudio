include ../Makefile.conf

all: symlinks
clean:
	while read fn fc tn tc; do rm man$$fc/$$fn.$$fc 2> /dev/null; done < symlinks.src; true
	while read fn fc tn tc; do rm html/man$$fc/$$fn.$$fc.html 2> /dev/null; done < symlinks.src; true
	rm -fr html/man?

symlinks:
	while read fn fc tn tc; do ln -s ../man$$tc/$$tn.$$tc man$$fc/$$fn.$$fc 2> /dev/null; done < symlinks.src; true

symlinks-html: man2html
	while read fn fc tn tc; do ln -s ../../html/man$$tc/$$tn.$$tc.html html/man$$fc/$$fn.$$fc.html 2> /dev/null; done < symlinks.src; true

man2html:
	sh -c 'for dir in man*; do (cd $$dir; mkdir ../html/$$dir 2>/dev/null; for file in *.?; do man2html -r $$file > ../html/$$dir/$$file.html; done); done'

prep-install-dirs:
	sh -c 'for dir in man*; do mkdir -p $(DESTDIR)$(PREFIX_MAN)/$$dir; done'

install: prep-install-dirs
	sh -c 'for dir in man*; do cp $(cp_v) $$dir/*.?* $(DESTDIR)$(PREFIX_MAN)/$$dir/; done'
semi-install: prep-install-dirs
	sh -c 'for dir in man*; do for file in $$dir/*.?*; do ln -fs `pwd`/$$file $(DESTDIR)$(PREFIX_MAN)/$$dir/; done; done'

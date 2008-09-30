include ../Makefile.conf

all: symlinks

symlinks:
	while read fn fc tn tc; do ln -s ../man$$tc/$$tn.$$tc man$$fc/$$fn.$$fc 2> /dev/null; done < symlinks.src; true

symlinks-html: man2html
	while read fn fc tn tc; do ln -s ../../html/man$$tc/$$tn.$$tc.html html/man$$fc/$$fn.$$fc.html 2> /dev/null; done < symlinks.src; true

man2html:
	sh -c 'for dir in man*; do (cd $$dir; mkdir ../html/$$dir 2>/dev/null; for file in *.?; do man2html -r $$file > ../html/$$dir/$$file.html; done); done'

prep-install-dirs:
	sh -c 'for dir in man*; do mkdir -p $(PREFIX_MAN)/$$dir; done'

install: prep-install-dirs
	sh -c 'for dir in man*; do cp $(cp_v) $$dir/*.?* $(PREFIX_MAN)/$$dir/; done'
semi-install: prep-install-dirs
	sh -c 'for dir in man*; do for file in $$dir/*.?*; do ln -s `pwd`/$$file $(PREFIX_MAN)/$$dir/; done; done'

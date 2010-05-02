include ../Makefile.conf

CLIENTS=roar-config roarbidir roarcat roarcat2sock roarcatad roarcatpassfh roarcatplay roarcatsendfile roarcatvio roarctl roarfilt roarmon roarradio roarshout roarsin roarsockconnect roarsocktypes roartypes roarvorbis roarvumeter roarinterconnect roarlight roarphone


all:
	echo Warning: There is nothing to build
clean: sandbox-clean

new: clean all

#test: sandbox-install test-all sandbox-clean
test: sandbox-clean sandbox-install test-all sandbox-clean

test-all: test-runnable test-roard

test-runnable:
	PREFIX_BIN=$(PREFIX_BIN) PREFIX_LIB=$(PREFIX_LIB) ./test-runnable.sh "$(CLIENTS)"
test-roard:
	PREFIX_BIN=$(PREFIX_BIN) PREFIX_LIB=$(PREFIX_LIB) ./test-roard.sh
sandbox-install:
	mkdir sandbox
	sh -c 'cd ..; DESTDIR=`pwd`/tests/sandbox $(MAKE) install'
sandbox-clean:
	rm -rf sandbox/

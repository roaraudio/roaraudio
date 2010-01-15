include ../Makefile.conf

CLIENTS=roar-config roarbidir roarcat roarcat2sock roarcatad roarcatpassfh roarcatplay roarcatsendfile roarcatvio roarctl roarfilt roarmon roarradio roarshout roarsin roarsockconnect roarsocktypes roartypes roarvorbis roarvumeter roarinterconnect roarlight roarphone


all:
	echo Warning: There is nothing to build
clean: sandbox-clean

new: clean all

#test: sandbox-install test-all sandbox-clean
test: test-all

test-all: test-runnable test-roard

test-runnable:
	./test-runnable.sh "$(CLIENTS)"
test-roard:
	./test-roard.sh
sandbox-install:
	mkdir sandbox
	sh -c 'cd ..; DESTDIR=`pwd`/tests/sandbox make install'
sandbox-clean:
	rm -rf sandbox/

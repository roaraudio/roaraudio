include ../Makefile.conf

CLIENTS=roar-config roarbidir roarcat roarcat2sock roarcatad roarcatpassfh roarcatplay roarcatsendfile roarcatvio roarctl roarfilt roarmon roarradio roarshout roarsin roarsockconnect roarsocktypes roartypes roarvorbis roarvumeter


all:
	echo Warning: There is nothing to build
clean:
	echo Warning: There is nothing to clean

new: clean all

test: test-runnable test-roard

test-runnable:
	./test-runnable.sh "$(CLIENTS)"
test-roard:
	./test-roard.sh

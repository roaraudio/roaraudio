TARGETS=roarcat roarcatad roarctl roarmon roarsockconnect

DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -L../lib/ -lroar

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarcat: roarcat.o
roarcatad: roarcatad.o
roarctl: roarctl.o
roarmon: roarmon.o
roarsockconnect: roarsockconnect.o

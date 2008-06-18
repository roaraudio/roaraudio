TARGETS=roarfish

OBJ=roarfish.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -L../lib/ -lroar -lfishsound

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarfish: ${OBJ}

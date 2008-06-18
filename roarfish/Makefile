TARGETS=roarfish

OBJ=roarfish.o play.o

#DEFINES        = -DDEBUG
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -L../lib/ -lroar -lfishsound -loggz

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roarfish: ${OBJ}

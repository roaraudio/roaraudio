TARGETS=roard

OBJ=roard.o driver.o mixer.o output.o signals.o loop.o clients.o streams.o network.o commands.o req.o sources.o sample.o
DRV=driver_esd.o driver_ao.o driver_roar.o driver_raw.o

#DEFINES        = -DDEBUG
#DEFINES        = -DMONITOR_LATENCY
INCLUDE = -I../include -I./include
CFLAGS  = -g -Wall -O2 $(DEFINES) $(INCLUDE)
LDFLAGS = -g -L../lib/ -lroar -lesd -lao

all: ${TARGETS}
	cp -v ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

roard: ${OBJ} ${DRV}

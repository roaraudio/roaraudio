TARGETS=esdcat.r

include ../Makefile.conf
include ../Makefile.inc

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

.a.r:
	./ast2roar $@ $<

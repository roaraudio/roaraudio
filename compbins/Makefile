TARGETS_ESD  = esdcat.r esdmon.r esdfilt.r esdplay.r esd.r
TARGETS_ARTS = artscat.r artsd.r artsplay.r
TARGETS_NAS  = auplay.r
TARGETS=$(TARGETS_ESD) $(TARGETS_ARTS) $(TARGETS_NAS)

include ../Makefile.conf
include ../Makefile.inc

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

.a.r:
	./ast2roar $@ $<

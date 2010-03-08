TARGETS_ESD      = esdcat.r esdmon.r esdfilt.r esdplay.r esd.r esddsp.r
TARGETS_ARTS     = artscat.r artsd.r artsplay.r artsdsp.r
TARGETS_NAS      = auplay.r audial.r audiooss.r
TARGETS_YIFF     = yplay.r yshutdown.r yiff.r
TARGETS_PA       = pacat.r paplay.r padsp.r
TARGETS_DTMFDIAL = dtmfdial.r
TARGETS=$(TARGETS_ESD) $(TARGETS_ARTS) $(TARGETS_NAS) $(TARGETS_YIFF) $(TARGETS_PA) $(TARGETS_DTMFDIAL)

include ../Makefile.conf
include ../Makefile.inc

.SUFFIXES: .a .r .bat

all: ${TARGETS}
	cp $(cp_v) ${TARGETS} ../lib/
clean:
	rm -f ${TARGETS} *.o
new: clean all

.a.r:
	./ast2roar $@ $<
.a.bat:
	./ast2roar --dos $@ $<

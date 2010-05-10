SUFFIX           = .r
TARGETS_ESD      = esdcat$(SUFFIX)  esdmon$(SUFFIX)    esdfilt$(SUFFIX)  esdplay$(SUFFIX) esd$(SUFFIX) esddsp$(SUFFIX)
TARGETS_ARTS     = artscat$(SUFFIX) artsd$(SUFFIX)     artsplay$(SUFFIX) artsdsp$(SUFFIX)
TARGETS_NAS      = auplay$(SUFFIX)  audial$(SUFFIX)    audiooss$(SUFFIX)
TARGETS_YIFF     = yplay$(SUFFIX)   yshutdown$(SUFFIX) yiff$(SUFFIX)
TARGETS_PA       = pacat$(SUFFIX)   paplay$(SUFFIX)    padsp$(SUFFIX)
TARGETS_DTMFDIAL = dtmfdial$(SUFFIX)
TARGETS_RSOUND   = ross$(SUFFIX)    rsd$(SUFFIX)       rsdplay$(SUFFIX)
TARGETS=$(TARGETS_ESD) $(TARGETS_ARTS) $(TARGETS_NAS) $(TARGETS_YIFF) $(TARGETS_PA) $(TARGETS_DTMFDIAL) $(TARGETS_RSOUND)

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

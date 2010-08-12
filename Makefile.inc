# Common:
COMMON_V_MAJOR  = 0
COMMON_V_MINOR  = 3
COMMON_V_REV    = 7
COMMON_V_MM     = $(COMMON_V_MAJOR).$(COMMON_V_MINOR)
COMMON_VERSION  = $(COMMON_V_MM).$(COMMON_V_REV)
COMMON_SOVERSION= 0

# libroar:
LIBROAR_V       = $(COMMON_SOVERSION)
NETLIBS         = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
AUXLIBS         = $(lib_slp) $(lib_ssl) $(lib_sendfile) $(lib_dl) $(lib_x11)
LIBROAR_NS      = $(AUXLIBS) $(NETLIBS)
LIBROAR         = -lroar $(LIBROAR_NS)

# libroardsp:
LIBROARDSP_V    = $(COMMON_SOVERSION)
DSP_CODECLIBS   = $(lib_celt) $(lib_celt0) $(lib_speex) $(lib_speexdsp)
DSP_AUXLIBS     = $(lib_samplerate)
LIBROARDSP_NS   = $(DSP_CODECLIBS) $(DSP_AUXLIBS) $(LIBROAR) $(lib_m)
LIBROARDSP      = -lroardsp $(LIBROARDSP_NS)

# libroarmidi:
LIBROARMIDI_V   = $(COMMON_SOVERSION)
LIBROARMIDI_NS  = $(LIBROARDSP)
LIBROARMIDI     = -lroarmidi $(LIBROARMIDI_NS)

# libroarlight:
LIBROARLIGHT_V  = $(COMMON_SOVERSION)
LIBROARLIGHT_NS = $(LIBROAR) $(lib_m)
LIBROARLIGHT    = -lroarlight $(LIBROARLIGHT_NS)

# libroareio:
LIBROAREIO_V    = $(COMMON_SOVERSION)
EIO_CDRIVERLIBS = $(lib_ossaudio)
LIBROAREIO_NS   = $(EIO_CDRIVERLIBS) $(LIBROAR)
LIBROAREIO      = -lroareio $(LIBROAREIO_NS)

#ll

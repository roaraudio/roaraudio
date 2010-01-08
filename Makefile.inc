# Common:
COMMON_VERSION  = 0.3.3

# libroar:
LIBROAR_V       = $(COMMON_VERSION)
NETLIBS         = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
AUXLIBS         = $(lib_slp) $(lib_ssl) $(lib_sendfile)
LIBROAR_NS      = $(AUXLIBS) $(NETLIBS)
LIBROAR         = -lroar $(LIBROAR_NS)

# libroardsp:
LIBROARDSP_V    = $(COMMON_VERSION)
DSP_CODECLIBS   = $(lib_celt) $(lib_speex) $(lib_speexdsp)
DSP_AUXLIBS     = $(lib_samplerate)
LIBROARDSP_NS   = $(DSP_CODECLIBS) $(DSP_AUXLIBS) $(LIBROAR) $(lib_m)
LIBROARDSP      = -lroardsp $(LIBROARDSP_NS)

# libroarmidi:
LIBROARMIDI_V   = $(COMMON_VERSION)
LIBROARMIDI_NS  = $(LIBROARDSP)
LIBROARMIDI     = -lroarmidi $(LIBROARMIDI_NS)

# libroarlight:
LIBROARLIGHT_V  = $(COMMON_VERSION)
LIBROARLIGHT_NS = $(LIBROAR) $(lib_m)
LIBROARLIGHT    = -lroarlight $(LIBROARLIGHT_NS)

# libroareio:
LIBROAREIO_V    = $(COMMON_VERSION)
EIO_CDRIVERLIBS = $(lib_ossaudio)
LIBROAREIO_NS   = $(EIO_CDRIVERLIBS) $(LIBROAR)
LIBROAREIO      = -lroareio $(LIBROAREIO_NS)

#ll

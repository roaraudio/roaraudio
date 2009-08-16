# libroar:
NETLIBS         = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
AUXLIBS         = $(lib_slp) $(lib_ssl) $(lib_sendfile)
LIBROAR_NS      = $(AUXLIBS) $(NETLIBS)
LIBROAR         = -lroar $(LIBROAR_NS)

# libroardsp:
DSP_CODECLIBS   = $(lib_celt) $(lib_speex)
LIBROARDSP_NS   = $(DSP_CODECLIBS) $(LIBROAR) $(lib_m)
LIBROARDSP      = -lroardsp $(LIBROARDSP_NS)

# libroarmidi:
LIBROARMIDI_NS  = $(LIBROARDSP)
LIBROARMIDI     = -lroarmidi $(LIBROARMIDI_NS)

# libroarlight:
LIBROARLIGHT_NS = $(LIBROAR) $(lib_m)
LIBROARLIGHT    = -lroarlight $(LIBROARLIGHT_NS)

# libroareio:
EIO_CDRIVERLIBS = $(lib_ossaudio)
LIBROAREIO_NS   = $(EIO_CDRIVERLIBS) $(LIBROAR)
LIBROAREIO      = -lroareio $(LIBROAREIO_NS)

#ll

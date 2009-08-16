# libroar:
NETLIBS         = $(lib_dnet) $(lib_wsock32) $(lib_ws2_32)
AUXLIBS         = $(lib_slp) $(lib_ssl) $(lib_sendfile)
LIBROAR         = $(AUXLIBS) $(NETLIBS)

# libroardsp:
DSP_CODECLIBS   = $(lib_celt) $(lib_speex)
LIBROARDSP      = $(DSP_CODECLIBS) $(LIBROAR) $(lib_m)

# libroarmidi:
LIBROARMIDI     = $(LIBROARDSP)

# libroarlight:
LIBROARLIGHT    = $(LIBROAR) $(lib_m)

# libroareio:
EIO_CDRIVERLIBS = $(lib_ossaudio)
LIBROAREIO      = $(EIO_CDRIVERLIBS) $(LIBROAR)

#ll

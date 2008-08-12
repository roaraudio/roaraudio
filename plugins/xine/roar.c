//roar.c:

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <roaraudio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <inttypes.h>

#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/audio_out.h>
#include <xine/metronom.h>

#define AO_OUT_ROAR_IFACE_VERSION 8

#define GAP_TOLERANCE         5000

typedef struct esd_driver_s {
  ao_driver_t      ao_driver;

  xine_t          *xine;

  int              capabilities;

  struct roar_connection con;
  struct roar_stream     stream;
  int fh;

} roar_driver_t;

typedef struct {
  audio_driver_class_t driver_class;
  xine_t          *xine;
} roar_class_t;

static int ao_roar_open(ao_driver_t *this_gen,
                       uint32_t bits, uint32_t rate, int mode) {
 return -1;
}
static void ao_roar_close(ao_driver_t *this_gen) {
 roar_driver_t *this = (roar_driver_t *) this_gen;

 if ( this->fh != -1 )
  close(this->fh);
 this->fh = -1;
}

static int ao_roar_num_channels(ao_driver_t *this_gen) {
 return -1;
}

static int ao_roar_bytes_per_frame(ao_driver_t *this_gen) {
  roar_driver_t *this = (roar_driver_t *) this_gen;
  return (this->stream.info.bits * this->stream.info.channels)/8;
}

static int ao_roar_delay(ao_driver_t *this_gen) {
 return 0;
}

static int ao_roar_write(ao_driver_t *this_gen,
                        int16_t* frame_buffer, uint32_t num_frames) {
 roar_driver_t *this = (roar_driver_t *) this_gen;
 int bpf = (this->stream.info.bits * this->stream.info.channels)/8;
 return write(this->fh, frame_buffer, num_frames*bpf) / bpf;
}
static int ao_roar_get_gap_tolerance (ao_driver_t *this_gen) {
  return GAP_TOLERANCE;
}

static void ao_roar_exit(ao_driver_t *this_gen) {
 roar_driver_t *this = (roar_driver_t *) this_gen;

 ao_roar_close(this_gen);

 roar_disconnect(&(this->con));

 free(this);
}

static int ao_roar_get_property (ao_driver_t *this_gen, int property) {
 return 0;
}
static int ao_roar_set_property (ao_driver_t *this_gen, int property, int value) {
 return ~value;
}

static int ao_roar_ctrl(ao_driver_t *this_gen, int cmd, ...) {
 return 0;
}

static uint32_t ao_roar_get_capabilities (ao_driver_t *this_gen) {
  roar_driver_t *this = (roar_driver_t *) this_gen;
  return this->capabilities;
}

static ao_driver_t *open_plugin (audio_driver_class_t *class_gen,
                                 const void *data) {
  roar_driver_t      *this;

  this                     = (roar_driver_t *) xine_xmalloc(sizeof(roar_driver_t));
  if ( !this )
   return NULL;

  if ( roar_simple_connect(&(this->con), NULL, "libxine") == -1 ) {
   free(this);
   return NULL;
  }

  this->fh = -1;

  this->capabilities       = AO_CAP_MODE_MONO | AO_CAP_MODE_STEREO | AO_CAP_MIXER_VOL | AO_CAP_MUTE_VOL;


  this->ao_driver.get_capabilities    = ao_roar_get_capabilities;
  this->ao_driver.get_property        = ao_roar_get_property;
  this->ao_driver.set_property        = ao_roar_set_property;
  this->ao_driver.open                = ao_roar_open;
  this->ao_driver.num_channels        = ao_roar_num_channels;
  this->ao_driver.bytes_per_frame     = ao_roar_bytes_per_frame;
  this->ao_driver.get_gap_tolerance   = ao_roar_get_gap_tolerance;
  this->ao_driver.delay               = ao_roar_delay;
  this->ao_driver.write               = ao_roar_write;
  this->ao_driver.close               = ao_roar_close;
  this->ao_driver.exit                = ao_roar_exit;
  this->ao_driver.control             = ao_roar_ctrl;

  return &(this->ao_driver);
}


static char* get_identifier (audio_driver_class_t *this_gen) {
  return "roar";
}

static char* get_description (audio_driver_class_t *this_gen) {
  return _("xine audio output plugin using RoarAudio");
}

static void dispose_class (audio_driver_class_t *this_gen) {

  roar_class_t *this = (roar_class_t *) this_gen;

  free (this);
}

static void *init_class (xine_t *xine, void *data) {

  roar_class_t        *this;

  this = (roar_class_t *) xine_xmalloc(sizeof(roar_class_t));

  this->driver_class.open_plugin     = open_plugin;
  this->driver_class.get_identifier  = get_identifier;
  this->driver_class.get_description = get_description;
  this->driver_class.dispose         = dispose_class;

  this->xine = xine;

  return this;
}

static const ao_info_t ao_info_roar = {
  4
};

const plugin_info_t xine_plugin_info[] = {
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_AUDIO_OUT, AO_OUT_ROAR_IFACE_VERSION, "roar", XINE_VERSION_CODE, &ao_info_roar, init_class },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};

//ll

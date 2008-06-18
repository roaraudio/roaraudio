//driver.h:

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <roaraudio.h>

#define DRIVER_USERDATA_T void *

#ifdef ROAR_HAVE_ESD
#define ROAR_DRIVER_DEFAULT "esd"
#else
#define ROAR_DRIVER_DEFAULT "null"
#endif

#ifdef ROAR_HAVE_ESD
#include <esd.h>
#include "driver_esd.h"
#endif

#include "driver_roar.h"
#include "driver_raw.h"

#ifdef ROAR_HAVE_LIBAO
#include <ao/ao.h>
#include "driver_ao.h"
#endif


struct roar_driver {
 char * name;
 char * desc;
 char * devices;
 int (*open )(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
 int (*close)(DRIVER_USERDATA_T   inst);
 int (*pause)(DRIVER_USERDATA_T   inst, int newstate);
 int (*write)(DRIVER_USERDATA_T   inst, char * buf, int len);
 int (*read )(DRIVER_USERDATA_T   inst, char * buf, int len);
 int (*flush)(DRIVER_USERDATA_T   inst);
};

void print_driverlist (void);

int driver_open (DRIVER_USERDATA_T * inst,
                 int * driver_id, char * driver /* NOTE: this is not part of struct roar_driver's def! */,
                 char * device, struct roar_audio_info * info);
int driver_close(DRIVER_USERDATA_T   inst, int driver);
int driver_pause(DRIVER_USERDATA_T   inst, int driver, int newstate);
int driver_write(DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_flush(DRIVER_USERDATA_T   inst, int driver);

#endif

//ll

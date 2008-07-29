//playback.c:

#include <libroaryiff.h>

YID YStartPlaySoundObjectSimple (YConnection *con, const char *path) {
 return YStartPlaySoundObject(con, path, NULL);
}

YID YStartPlaySoundObject (YConnection *con, const char *path, YEventSoundPlay *value) {
 if ( !con )
  return YIDNULL;

 if ( !path )
  return YIDNULL;

 // hm,... find out how to do this.
 // need to start ssize_t roar_file_play (struct roar_connection * con, char * file, int exec)
 // in background


 return YIDNULL;
}

void YDestroyPlaySoundObject(YConnection *con, YID yid) {
 struct roar_connection rcon;

 if ( !con )
  return ;

 if ( yid == YIDNULL )
  return;

 rcon.fh = con->fd;

 roar_kick(&rcon, ROAR_OT_STREAM, ROARYIFF_YID2ROAR(yid));
}

//ll

//audiocd.c:

#include <libroaryiff.h>

int YEjectAudioCD(YConnection *con) {
 return system("eject") == 0 ? 0 : -1;
}

//ll

//meta.h:

#ifndef _ROARAUDIO_META_H_
#define _ROARAUDIO_META_H_

#define ROAR_META_TYPE_NONE          0
#define ROAR_META_TYPE_TITLE         1
#define ROAR_META_TYPE_ALBUM         2
#define ROAR_META_TYPE_AUTOR         3
#define ROAR_META_TYPE_ARTIST        ROAR_META_TYPE_AUTOR
#define ROAR_META_TYPE_VERSION       4
#define ROAR_META_TYPE_DATE          5
#define ROAR_META_TYPE_LICENSE       6
#define ROAR_META_TYPE_TRACKNUMBER   7
#define ROAR_META_TYPE_ORGANIZATION  8
#define ROAR_META_TYPE_DESCRIPTION   9
#define ROAR_META_TYPE_GENRE         10
#define ROAR_META_TYPE_LOCATION      11
#define ROAR_META_TYPE_CONTACT       12
#define ROAR_META_TYPE_STREAMURL     13
#define ROAR_META_TYPE_HOMEPAGE      14
#define ROAR_META_TYPE_THUMBNAIL     15
#define ROAR_META_TYPE_LENGTH        16
#define ROAR_META_TYPE_COMMENT       17
#define ROAR_META_TYPE_OTHER         18
#define ROAR_META_TYPE_FILENAME      19
#define ROAR_META_TYPE_FILEURL       20
#define ROAR_META_TYPE_SERVER        21


#define ROAR_META_MODE_SET           0
#define ROAR_META_MODE_ADD           1
#define ROAR_META_MODE_DELETE        2
#define ROAR_META_MODE_CLEAR         3

#define ROAR_META_MAX_NAMELEN 32

#define ROAR_META_MAX_PER_STREAM 16

struct roar_meta {
 int    type;
 char   key[ROAR_META_MAX_NAMELEN];
 char * value;
};

#endif

//ll

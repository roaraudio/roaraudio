//porto.h:

#ifndef _ROARAUDIO_PORTO_H_
#define _ROARAUDIO_PORTO_H_

#define ROAR_CMD_EOL           -1 /* end of list */
#define ROAR_CMD_NOOP           0 /* do nothing */
#define ROAR_CMD_IDENTIFY       1 /* after connect we have to identify ourself */
#define ROAR_CMD_AUTH           2 /* send an auth cookie */
#define ROAR_CMD_NEW_STREAM     3 /* request a new stream id */
#define ROAR_CMD_SET_META       4 /* update meta data of streamed data */
#define ROAR_CMD_EXEC_STREAM    5 /* close all data channels and make the controll socket a raw data socket */
#define ROAR_CMD_QUIT           6 /* quits the connection */

#define ROAR_CMD_GET_STANDBY    7
#define ROAR_CMD_SET_STANDBY    8

#define ROAR_CMD_SERVER_INFO    9 /* Server version ... */
#define ROAR_CMD_SERVER_STATS  10 /* Server stats: number of clients, streams,.. */
#define ROAR_CMD_SERVER_OINFO  11 /* Output info, roar_audio_info sa */

#define ROAR_CMD_ADD_DATA      12 /* add some data to the input buffer */

//#define ROAR_CMD_

#define ROAR_CMD_EXIT          13 /* make the server quit */
#define ROAR_CMD_LIST_STREAMS  14 /* list all streams (not only playback streams like esd does) */
#define ROAR_CMD_LIST_CLIENTS  15 /* list all clients */

#define ROAR_CMD_GET_CLIENT    16 /* get infos about a client */
#define ROAR_CMD_GET_STREAM    17 /* get infos about a stream */

#define ROAR_CMD_KICK          18 /* kick a client, stream, sample or source */

#define ROAR_CMD_SET_VOL       19 /* change volume */
#define ROAR_CMD_GET_VOL       20 /* get volume */

#define ROAR_CMD_OK           254 /* return value OK */
#define ROAR_CMD_ERROR        255 /* return value ERROR */


#define ROAR_STANDBY_ACTIVE     1
#define ROAR_STANDBY_INACTIVE   0

// object types...
#define ROAR_OT_CLIENT 1
#define ROAR_OT_STREAM 2
#define ROAR_OT_SOURCE 3
#define ROAR_OT_SAMPLE 4

#endif

//ll

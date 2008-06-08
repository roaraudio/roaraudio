//esdbasic.c:

#include "libroaresd.h"

/* opens channel, authenticates connection, and prefares for protos */
/* returns EsounD socket for communication, result < 0 = error */
/* server = listen socket (localhost:5001, 192.168.168.0:9999 */
/* rate, format = (bits | channels | stream | func) */
int esd_open_sound( const char *host ) {
 struct roar_connection con;

 if ( roar_simple_connect(&con, host, NULL) == -1 ) {
  ROAR_DBG("esd_open_sound(*): roar_simple_connect() faild!");
  return -1;
 }

 return con.fh;
}

/* send the authorization cookie, create one if needed */
int esd_send_auth( int sock );

/* closes fd, previously obtained by esd_open */
int esd_close( int esd ) {
 return roar_simple_close(esd);
}

/* get the stream latency to esound (latency is number of samples  */
/* at 44.1khz stereo 16 bit - you'll have to adjust if oyur input  */
/* sampling rate is less (in bytes per second)                     */
/* so if you're at 44.1khz stereo 16bit in your stream - your lag  */
/* in bytes woudl be lag * 2 * 2 bytes (2 for stereo, 2 for 16bit) */
/* if your stream is at 22.05 Khz it'll be double this - in mono   */
/* double again ... etc.                                           */
int esd_get_latency(int esd) {
 struct timeval         try, ans;
 struct roar_message    m;
 struct roar_connection con;

 m.cmd = ROAR_CMD_NOOP;
 m.datalen = 0;

 con.fh = esd;

 gettimeofday(&try, NULL);
 roar_req(&con, &m, NULL);
 gettimeofday(&ans, NULL);

 while (ans.tv_sec > try.tv_sec) {
  ans.tv_sec--;
  ans.tv_usec += 1000000;
 }
 ans.tv_usec -= try.tv_usec;

/*
 1   -> 44100
 0.5 -> 22050
*/

 return 441*ans.tv_usec/10000;
}


//ll

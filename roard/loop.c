//loop.c:

#include "roard.h"

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa) {
 void ** streams_input = NULL;
 uint32_t pos = 0;

 ROAR_DBG("main_loop(*) = ?");
 alive = 1;

 while (alive) {
  ROAR_DBG("main_loop(*): looping...");

  ROAR_DBG("main_loop(*): check for new clients...");
  net_check_listen();

  ROAR_DBG("main_loop(*): check for new data...");
  clients_check_all();

  ROAR_DBG("main_loop(*): mixing clients...");
  if ( g_standby ) {
   // while in standby we still neet to get the buffers to free input buffer space.
   streams_get_mixbuffers(&streams_input, sa, pos);
  } else {
   if ( streams_get_mixbuffers(&streams_input, sa, pos) == 0 ) {
    mix_clients(g_output_buffer, sa->bits, streams_input, ROAR_OUTPUT_BUFFER_SAMPLES * sa->channels);
   }
  }

/*
  // while in standby we still need to write out our buffer to not run in an endless loop without
  // a break
*/

  if ( g_standby ) {
   usleep((1000000 * ROAR_OUTPUT_BUFFER_SAMPLES) / sa->rate);
  } else {
   clients_send_filter(sa, pos);
   output_buffer_flush(driver_inst, driver);
   clients_send_mon(sa, pos);
  }
//  output_buffer_reinit();

  pos = ROAR_MATH_OVERFLOW_ADD(pos, ROAR_OUTPUT_BUFFER_SAMPLES);
  ROAR_DBG("main_loop(*): current pos: %u", pos);
 }

 return -1;
}

//ll

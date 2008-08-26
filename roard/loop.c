//loop.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa) {
 void ** streams_input = NULL;
 int     term = 0;
 int     streams;
#ifdef MONITOR_LATENCY
 struct timeval         try, ans;
 long int ans_1last = 0, ans_2last = 0, ans_3last = 0;
 long int loopc = 0;

 printf("\n\e[s");
 fflush(stdout);
#endif

 ROAR_DBG("main_loop(*) = ?");
 alive = 1;
 g_pos = 0;

 while (alive) {
#ifdef MONITOR_LATENCY
 gettimeofday(&try, NULL);
#endif

  ROAR_DBG("main_loop(*): looping...");

  if ( g_listen_socket != -1 ) {
   ROAR_DBG("main_loop(*): check for new clients...");
   net_check_listen();
  }

  ROAR_DBG("main_loop(*): check for new data...");
  if ( clients_check_all() == 0 && g_terminate && g_listen_socket == -1 ) {
   term  = 1;
  }

  ROAR_DBG("main_loop(*): mixing clients...");
  if ( g_standby ) {
   // while in standby we still neet to get the buffers to free input buffer space.
   streams = streams_get_mixbuffers(&streams_input, sa, g_pos);
  } else {
   if ( ( streams = streams_get_mixbuffers(&streams_input, sa, g_pos)) != -1 ) {
    mix_clients(g_output_buffer, sa->bits, streams_input, ROAR_OUTPUT_BUFFER_SAMPLES * sa->channels);
   }
  }

  if ( term && streams < 1 )
   alive = 0;

/*
  // while in standby we still need to write out our buffer to not run in an endless loop without
  // a break
*/

  if ( g_standby ) {
   usleep((1000000 * ROAR_OUTPUT_BUFFER_SAMPLES) / sa->rate);
   printf("usleep(%u) = ?\n", (1000000 * ROAR_OUTPUT_BUFFER_SAMPLES) / sa->rate);
  } else {
   clients_send_filter(sa, g_pos);
   output_buffer_flush(driver_inst, driver);
   clients_send_mon(sa, g_pos);
  }

  midi_cb_update();
//  output_buffer_reinit();

  g_pos = ROAR_MATH_OVERFLOW_ADD(g_pos, ROAR_OUTPUT_BUFFER_SAMPLES);
  ROAR_DBG("main_loop(*): current pos: %u", g_pos);
#ifdef MONITOR_LATENCY
 gettimeofday(&ans, NULL);

 while (ans.tv_sec > try.tv_sec) {
  ans.tv_sec--;
  ans.tv_usec += 1000000;
 }
 ans.tv_usec -= try.tv_usec;

 if ( loopc % 128 ) {
  printf("\e[ucurrent latency: %.3fms  average: %.3fms   ",  ans.tv_usec                               / (double)1000,
                                                            (ans.tv_usec+ans_3last+ans_2last+ans_1last)/ (double)4000);
  fflush(stdout);
 }

 ans_3last = ans_2last;
 ans_2last = ans_1last;
 ans_1last = ans.tv_usec;
 loopc++;
#endif
 }

 return -1;
}

//ll

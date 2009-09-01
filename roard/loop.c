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

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa, int sysclocksync) {
 void ** streams_input = NULL;
 int     term = 0;
 int     streams;
#ifdef ROAR_HAVE_GETTIMEOFDAY
 long int loopc = 0;
 struct timeval         try, ans;
 float  freq;
#endif
#ifdef ROAR_SUPPORT_LISTEN
 int i;
 int have_listen;
#endif
#ifdef MONITOR_LATENCY
 long int ans_1last = 0, ans_2last = 0, ans_3last = 0;

 printf("\n\e[s");
 fflush(stdout);
#endif

 ROAR_DBG("main_loop(*) = ?");
// alive = 1;
 g_pos = 0;

#ifdef ROAR_HAVE_GETTIMEOFDAY
 if ( sysclocksync ) {
  gettimeofday(&try, NULL);
 }
#endif

 while (alive) {
#if defined(MONITOR_LATENCY) && defined(ROAR_HAVE_GETTIMEOFDAY)
 gettimeofday(&try, NULL);
#endif

  ROAR_DBG("main_loop(*): looping...");

#ifdef ROAR_SUPPORT_LISTEN
  have_listen = 0;
  for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
   if ( g_listen_socket[i] != -1 ) {
    have_listen = 1;
    break;
   }
  }

  if ( have_listen ) {
   ROAR_DBG("main_loop(*): check for new clients...");
   net_check_listen();
  }
#endif

  ROAR_DBG("main_loop(*): check for new data...");
#ifdef ROAR_SUPPORT_LISTEN
  if ( clients_check_all() == 0 && g_terminate && !have_listen ) {
#else
  if ( clients_check_all() == 0 && g_terminate ) {
#endif
   term  = 1;
  }

#ifndef ROAR_WITHOUT_DCOMP_MIDI
  ROAR_DBG("main_loop(*): updating midi subsystem...");
  midi_update();
#endif

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


  ROAR_DBG("main_loop(*): sending output data...");

#ifdef ROAR_HAVE_USLEEP
  if ( g_standby || (streams < 1 && g_autostandby) ) {
   usleep((1000000 * ROAR_OUTPUT_BUFFER_SAMPLES) / sa->rate);
   ROAR_DBG("usleep(%u) = ?\n", (1000000 * ROAR_OUTPUT_BUFFER_SAMPLES) / sa->rate);
  } else {
#endif
   clients_send_filter(sa, g_pos);
   output_buffer_flush(driver_inst, driver);
   clients_send_mon(sa, g_pos);
#ifdef ROAR_HAVE_USLEEP
  }
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIDI
  midi_reinit();
#endif
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
  ssynth_update();
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
  light_reinit();
#endif

//  output_buffer_reinit();

  g_pos = ROAR_MATH_OVERFLOW_ADD(g_pos, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels);
  ROAR_DBG("main_loop(*): current pos: %u", g_pos);
#if defined(MONITOR_LATENCY) && defined(ROAR_HAVE_GETTIMEOFDAY)
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
#endif

#ifdef ROAR_HAVE_GETTIMEOFDAY
  if ( sysclocksync && !(loopc % sysclocksync) ) {
   gettimeofday(&ans, NULL);

   while (ans.tv_sec > try.tv_sec) {
    ans.tv_sec--;
    ans.tv_usec += 1000000;
   }
   ans.tv_usec -= try.tv_usec;


   freq = (sysclocksync * ROAR_OUTPUT_BUFFER_SAMPLES) / (ans.tv_usec / 1e6);
   printf("SYNC: f_conf=%iHz, f_real=%.2fHz diff=%+.3f%%\n", sa->rate, freq, 100*(freq/sa->rate - 1));

//   memcpy(&try, &ans, sizeof(try));
   gettimeofday(&try, NULL);
  }

  if ( sysclocksync )
   loopc++;
#endif
 }

 return -1;
}

//ll

//esdctl.c:

#include "libroaresd.h"

/* lock/unlock will disable/enable foreign clients from connecting */

// we do not have such a thing at the moment...
// so always return -1
int esd_lock( int esd ) {
 return -1;
}
int esd_unlock( int esd ) {
 return -1;
}

/* standby/resume will free/reclaim audio device so others may use it */
int esd_standby( int esd ) {
 struct roar_connection con;

 con.fh = esd;

 return roar_set_standby(&con, ROAR_STANDBY_ACTIVE);
}

int esd_resume( int esd ) {
 struct roar_connection con;

 con.fh = esd;

 return roar_set_standby(&con, ROAR_STANDBY_INACTIVE);
}

/* print server into to stdout */
void esd_print_server_info( esd_server_info_t *server_info ) {
 char buf[80] = "";

 if ( server_info->format & ESD_BITS16 )
  strcat(buf, "16 bit ");
 else
  strcat(buf, "8 bit ");

 if ( server_info->format & ESD_STEREO )
  strcat(buf, "stereo ");
 else
  strcat(buf, "mono ");

 printf("server version   = %i\n",        server_info->version);
 printf("server format    = 0x%08x %s\n", server_info->format, buf);
 printf("server rate      = %i\n",        server_info->rate);
}


void esd_print_player_info( esd_player_info_t *player_info ) {
 char buf[80] = "";

 if ( player_info->format & ESD_BITS16 )
  strcat(buf, "16 bit ");
 else
  strcat(buf, "8 bit ");

 if ( player_info->format & ESD_MONO )
  strcat(buf, "stereo ");
 else
  strcat(buf, "mono ");

 printf("player %i name    = %s\n",        player_info->source_id, player_info->name);
 printf("player %i format  = 0x%08x %s\n", player_info->source_id, player_info->format, buf);
 printf("player %i rate    = %i\n",        player_info->source_id, player_info->rate);
 printf("player %i left    = %i\n",        player_info->source_id, player_info->left_vol_scale );
 printf("player %i right   = %i\n",        player_info->source_id, player_info->right_vol_scale );
}

void esd_print_sample_info( esd_sample_info_t *sample_info ) {
}

/* print all info to stdout */
void esd_print_all_info( esd_info_t *all_info ) {
 esd_player_info_t *player;
 esd_sample_info_t *sample;


 esd_print_server_info(all_info->server);

 player = all_info->player_list;

 while (player) {
  esd_print_player_info(player);
  player = player->next;
 }

 sample = all_info->sample_list;

 while (sample) {
  esd_print_sample_info(sample);
  sample = sample->next;
 }

}

/* retrieve server properties (sample rate, format, version number) */
esd_server_info_t *esd_get_server_info( int esd ) {
 esd_server_info_t * r;
 struct roar_stream     s;
 struct roar_connection con;
 struct roar_message    m;

 r = malloc(sizeof(esd_server_info_t));

 if ( r == NULL )
  return NULL;

 r->version = 0; // seems to be static

 con.fh    = esd;
 m.cmd     = ROAR_CMD_SERVER_OINFO;
 m.datalen = 0;

 if ( roar_req(&con, &m, NULL) == -1 ) {
  free(r);
  return NULL;
 }

 if ( m.cmd != ROAR_CMD_OK ) {
  free(r);
  return NULL;
 }

 if ( roar_stream_m2s(&s, &m) == -1 ) {
  free(r);
  return NULL;
 }

 r->rate   = s.info.rate;
 r->format = 0;

 if ( s.info.channels  == 1 )
  r->format |= ESD_MONO;
 else
  r->format |= ESD_STEREO;

 if ( s.info.bits == 8 )
  r->format |= ESD_BITS8;
 else
  r->format |= ESD_BITS16;

 return r;
}
/* release all memory allocated for the server properties structure */
void esd_free_server_info( esd_server_info_t *server_info ) {
 if (server_info)
  free(server_info);
}

/* retrieve all information from server */
esd_info_t *esd_get_all_info( int esd ) {
 esd_info_t * r;
 int i;
 int num;
 int clients[ROAR_CLIENTS_MAX];
 struct roar_client c;
 struct roar_stream s;
 struct roar_connection con[1];
 struct roar_mixer_settings mixer;
 int channels;
 esd_player_info_t * new_player, * cur = NULL; // = NULL to avoid gcc warning

 con->fh = esd;

 r = malloc(sizeof(esd_info_t));

 if ( r == NULL )
  return NULL;

 r->server      = esd_get_server_info(esd);
 r->player_list = NULL;
 r->sample_list = NULL;

 if ( (num = roar_list_clients(con, clients, ROAR_CLIENTS_MAX)) == -1 ) {
  ROAR_ERR("esd_get_all_info(*): can not get client list");
  num = 0;
 }

 for (i = 0; i < num; i++) {
  if ( roar_get_client(con, &c, clients[i]) == -1 ) {
   ROAR_ERR("esd_get_all_info(*): can not get client info");
   continue;
  }

  if ( c.execed != -1 ) {
   if ( roar_get_stream(con, &s, c.execed) == -1 ) {
    ROAR_ERR("esd_get_all_info(*): can not get stream info");
    continue;
   }

   if ( (new_player = malloc(sizeof(esd_player_info_t))) == NULL ) {
    ROAR_ERR("esd_get_all_info(*): can not alloc memory for new player! BAD");
    continue;
   }

   new_player->next = NULL;

   new_player->source_id      = c.execed;
   new_player->rate           = s.info.rate;

   new_player->format         = ROAR_S2ESD(&s);
//   sprintf(new_player->name, "roar stream %i", c.execed);
   strncpy(new_player->name, c.name, ESD_NAME_MAX < ROAR_BUFFER_NAME ? ESD_NAME_MAX : ESD_NAME_MAX);

   new_player->server         = r->server;

   if ( roar_get_vol(con, c.execed, &mixer, &channels) == -1 ) {
    ROAR_ERR("esd_get_all_info(*): can not get stream mixer info");
    new_player->left_vol_scale = new_player->right_vol_scale = 256;
   } else {
    if ( channels == 1 ) {
     new_player->left_vol_scale = new_player->right_vol_scale = mixer.mixer[0] == 65536 ? 256 : mixer.mixer[0] / 256;
    } else {
     if ( channels != 2 ) {
      ROAR_ERR("esd_get_all_info(*): server seems to run in > 2 channel mode. ignoring any but the first two channels!");
     }
     new_player->left_vol_scale  = mixer.mixer[0] == 65536 ? 256 : mixer.mixer[0] / 256;
     new_player->right_vol_scale = mixer.mixer[1] == 65536 ? 256 : mixer.mixer[1] / 256;
    }
   }


   if ( r->player_list == NULL ) {
     r->player_list = cur = new_player;
   } else {
     cur->next = new_player; // add to old player
     cur       = new_player; // hop to next player
   }

  }
 }

 return r;
}

/* retrieve all information from server, and update until unsubsribed or closed */
esd_info_t *esd_subscribe_all_info( int esd );

/* call to update the info structure with new information, and call callbacks */
esd_info_t *esd_update_info( int esd, esd_info_t *info,
                             esd_update_info_callbacks_t *callbacks );
esd_info_t *esd_unsubscribe_info( int esd );

/* release all memory allocated for the esd info structure */
void esd_free_all_info( esd_info_t *info ) {
 esd_player_info_t *player, *oplayer;
 esd_sample_info_t *sample, *osample;

 if ( info != NULL ) {

/*
  if ( info->server )
   free(info->server);
*/
  esd_free_server_info(info->server);


/*
  // TODO: do we need to free more?
  if ( info->player_list )
   free(info->player_list);

  if ( info->sample_list )
   free(info->sample_list);
*/

 player = info->player_list;

 while (player) {
  oplayer = player;
  player  = player->next;

  free(oplayer);
 }

 sample = info->sample_list;

 while (sample) {
  osample = sample;
  sample  = sample->next;

  free(osample);
 }

  free(info);
 }
}


/* reset the volume panning for a stream */
int esd_set_stream_pan( int esd, int stream_id,
                        int left_scale, int right_scale ) {
 struct roar_connection con;
 struct roar_mixer_settings mixer;

 con.fh = esd;

 mixer.mixer[0] = left_scale  == 256 ? 65535 : left_scale  * 256;
 mixer.mixer[1] = right_scale == 256 ? 65535 : right_scale * 256;

 ROAR_DBG("esd_set_stream_pan(esd=%i, stream_id=%i, left_scale=%i, right_scale=%i) = ?", 
                esd, stream_id, left_scale, right_scale);

 return roar_set_vol(&con, stream_id, &mixer, 2);
}

/* reset the default volume panning for a sample */
int esd_set_default_sample_pan( int esd, int sample_id,
                                int left_scale, int right_scale ) {
 return -1;
}

/* see if the server is in stnaby, autostandby, etc */
esd_standby_mode_t esd_get_standby_mode( int esd ) {
 return roar_simple_get_standby(esd);
}

//ll

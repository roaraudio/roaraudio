//roarctl.c:

#include <roaraudio.h>

int display_mixer (struct roar_connection * con, int stream);

void usage (void) {
 printf("roarcat [OPTIONS]... COMMAND [OPTS] [COMMAND [OPTS] [COMMAND [OPTS] [...]]]\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --help             - Show this help\n"
       );

 printf("\nCommands:\n\n");
 printf(
        "  help               - Show this help\n"
        "\n"
        "  standby, off       - Go into standby mode\n"
        "  resume, on         - Go into active mode\n"
        "  standbymode        - Show current standby mode\n"
        "  exit               - Quits the roard (must be used as last command)\n"
        "\n"
        "  kick TYPE ID       - Kicks object of TYPE with id ID\n"
        "                       Types: client stream sample source\n"
        "\n"
        "  serveroinfo        - Gets Informations about server output\n"
        "  listclients        - Gets Informations about clients\n"
        "  liststreams        - Gets Informations about streams\n"
        "  allinfo            - Get all infos\n"
       );
}

void server_oinfo (struct roar_connection * con) {
 struct roar_stream s;

 if ( roar_server_oinfo(con, &s) == -1 ) {
  fprintf(stderr, "Error: can not get server output info\n");
  return;
 }

 printf("Stream direction      : %s\n", roar_dir2str(s.dir));
 printf("Server Output rate    : %i\n", s.info.rate);
 printf("Server Output bits    : %i\n", s.info.bits);
 printf("Server Output channels: %i\n", s.info.channels);
 printf("Server Output codec   : %i (%s%s)\n", s.info.codec, roar_codec2str(s.info.codec),
                                     s.info.codec == ROAR_CODEC_DEFAULT ? " native" : "");
// printf("Server Output rate: %i", s.info.rate);
}

void list_clients (struct roar_connection * con) {
 int i;
 int num;
 int h;
 int id[ROAR_CLIENTS_MAX];
 struct roar_client c;

 if ( (num = roar_list_clients(con, id, ROAR_CLIENTS_MAX)) == -1 ) {
  fprintf(stderr, "Error: can not get client list\n");
  return;
 }

 for (i = 0; i < num; i++) {
  printf("client %i:\n", id[i]);
  if ( roar_get_client(con, &c, id[i]) == -1 ) {
   fprintf(stderr, "Error: can not get client info\n");
   continue;
  }
  printf("Player name           : %s\n", c.name);
  if ( c.execed != -1 )
   printf("Execed stream         : %i\n", c.execed);

  for (h = 0; h < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; h++)
   if ( c.streams[h] != -1 )
    printf("stream                : %i\n", c.streams[h]);
 }

}

void list_streams (struct roar_connection * con) {
 int i;
 int num;
 int id[ROAR_STREAMS_MAX];
 struct roar_stream s;


 if ( (num = roar_list_streams(con, id, ROAR_STREAMS_MAX)) == -1 ) {
  fprintf(stderr, "Error: can not get stream list\n");
  return;
 }

 for (i = 0; i < num; i++) {
  printf("stream %i:\n", id[i]);
  if ( roar_get_stream(con, &s, id[i]) == -1 ) {
   fprintf(stderr, "Error: can not get stream info\n");
   continue;
  }
  printf("Stream direction      : %s\n", roar_dir2str(s.dir));
  if ( s.pos_rel_id == -1 )
   printf("Relativ position id   : none (stream not synchronized)\n");
  else
   printf("Relativ position id   : %i\n", s.pos_rel_id);
  printf("Input rate            : %i\n", s.info.rate);
  printf("Input bits            : %i\n", s.info.bits);
  printf("Input channels        : %i\n", s.info.channels);
  printf("Input codec           : %i (%s%s)\n", s.info.codec, roar_codec2str(s.info.codec),
                                      s.info.codec == ROAR_CODEC_DEFAULT ? " native" : "");
  display_mixer(con, id[i]);
 }

}

int display_mixer (struct roar_connection * con, int stream) {
 int channels;
 struct roar_mixer_settings mixer;
 int i;

 if ( roar_get_vol(con, stream, &mixer, &channels) == -1 ) {
  fprintf(stderr, "Error: can not get stream mixer info\n");
  return -1;
 }

 for (i = 0; i < channels; i++)
  printf("Mixer volume chan %2i  : %i (%.2f%%)\n", i, mixer.mixer[i], (float)mixer.mixer[i]/655.35);

 return 0;
}

int main (int argc, char * argv[]) {
 struct roar_connection con;
 char * server   = NULL;
 char * k = NULL;
 int    i;
 int    t = 0;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( *k == '-' ) {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  } else {
   break;
  }
 }

 // connect

 if ( roar_connect(&con, server) == -1 ) {
  fprintf(stderr, "Error: Can not connect to server\n");
  return 1;
 }

 if ( roar_identify(&con, "roarctl") == -1 ) {
  fprintf(stderr, "Error: Can not identify to server\n");
  return 1;
 }

 if ( i == argc ) {
  fprintf(stderr, "Error: No Commands given\n");
  return 0; // this is not a fatal error...
 }

 for (; i < argc; i++) {
  k = argv[i];
  // cmd is in k

  printf("--- [ %s ] ---\n", k);

  if ( !strcmp(k, "help") ) {
   usage();


  } else if ( !strcmp(k, "standby") || !strcmp(k, "off") ) {
   if ( roar_set_standby(&con, ROAR_STANDBY_ACTIVE) == -1 ) {
    fprintf(stderr, "Error: can not set mode to standby\n");
   } else {
    printf("going into standby\n");
   }
  } else if ( !strcmp(k, "resume") || !strcmp(k, "on") ) {
   if ( roar_set_standby(&con, ROAR_STANDBY_INACTIVE) == -1 ) {
    fprintf(stderr, "Error: can not set mode to active\n");
   } else {
    printf("going into active mode\n");
   }

  } else if ( !strcmp(k, "exit") ) {
   if ( roar_exit(&con) == -1 ) {
    fprintf(stderr, "Error: can not quit server\n");
   } else {
    printf("Server quited\n");
    break;
   }

  } else if ( !strcmp(k, "standbymode") ) {
   t = roar_get_standby(&con);
   if ( t == -1 ) {
    fprintf(stderr, "Error: can not get stanby mode\n");
   } else if ( t == ROAR_STANDBY_ACTIVE ) {
    printf("Server is in standby\n");
   } else if ( t == ROAR_STANDBY_INACTIVE ) {
    printf("Server is active\n");
   } else {
    fprintf(stderr, "Error: unknown standby mode: %i\n", t);
   }

  } else if ( !strcmp(k, "serveroinfo") ) {
   server_oinfo(&con);
  } else if ( !strcmp(k, "listclients") ) {
   list_clients(&con);
  } else if ( !strcmp(k, "liststreams") ) {
   list_streams(&con);
  } else if ( !strcmp(k, "allinfo") ) {
   server_oinfo(&con);
   printf("\n");
   list_clients(&con);
   printf("\n");
   list_streams(&con);

  } else if ( !strcmp(k, "kick") ) {
   k = argv[++i];
   if ( !strcmp(k, "client") ) {
    t = ROAR_OT_CLIENT;
   } else if ( !strcmp(k, "stream") ) {
    t = ROAR_OT_STREAM;
   } else if ( !strcmp(k, "sample") ) {
    t = ROAR_OT_SAMPLE;
   } else if ( !strcmp(k, "source") ) {
    t = ROAR_OT_SOURCE;
   } else {
    fprintf(stderr, "Error: unknown type: %s\n", k);
    continue;
   }
   //t = atoi(argv[i++]);
   if ( roar_kick(&con, t, atoi(argv[++i])) == -1 ) {
    fprintf(stderr, "Error: can not kick %s\n", k);
   } else {
    printf("%s kicked\n", k);
   }

  } else {
   fprintf(stderr, "Error: invalid command: %s\n", k);
  }

 }

 roar_disconnect(&con);

 return 0;
}

//ll

//roard.c:

#include "roard.h"

char * server = ROAR_DEFAULT_SOCK_GLOBAL; // global server address

void usage (void) {
 printf("Usage: roard [OPTIONS]...\n\n");

 printf("Misc Options:\n\n");
 printf(
        " --demon               - Bring the server into background after init\n"
        " --terminate           - Terminate after last client quited\n"
        " --restart             - Trys to stop an old instance and start a new with new settings\n"
        " --realtime            - Trys to get realtime priority,\n"
        "                         give multible times for being more realtime\n"
       );

 printf("\nAudio Options:\n\n");
 printf(
        " -R  --rate   RATE     - Set server rate\n"
        " -B  --bits   BITS     - Set server bits\n"
        " -C  --chans  CHANNELS - Set server channels\n"
       );

 printf("\nDriver Options:\n\n");
 printf(" -d  --driver DRV      - Set the driver, use '-d list' to get a list (default: %s)\n", ROAR_DRIVER_DEFAULT);
 printf(" -D  --device DEV      - Set the device\n");
 printf(" -dO OPTS              - Set output options\n");

 printf("\nSource Options:\n\n");
 printf(" -s  --source DRV      - Use DRV as input driver\n"
        " -S           DEV      - Use DEV as input device\n"
        " -sO          OPTS     - Use OPTS as input options\n"
        " -sP                   - Make souce as primary\n"
       );

 printf("\nCodec Filter Options:\n\n");
 printf(" --list-cf             - List all codec filter\n"
       );

 printf("\nServer Options:\n\n");
 printf(" -t  --tcp             - Use TCP listen socket\n"
        " -u  --unix            - Use UNIX Domain listen socket (default)\n"
        " -p  --port            - TCP Port to bind to\n"
        " -b  --bind            - IP/Hostname to bind to\n"
        " -s  --sock            - Filename for UNIX Domain Socket\n"
        " -G  GROUP             - Sets the group for the UNIX Domain Socket, (default: audio)\n"
        "                         You need the permittions to change the GID\n"
        " --no-listen           - Do not listen for new clients (only usefull for relaing)\n"
        " --client-fh           - Comunicate with a client over this handle\n"
        "                         (only usefull for relaing)\n"
       );
// printf("\n Options:\n\n");
 printf("\n");
}

int main (int argc, char * argv[]) {
 int i;
 char * k;
 char user_sock[80] = {0};
 struct roar_audio_info sa;
 int    demon    = 0;
 int    realtime = 0;
 char * driver = getenv("ROAR_DRIVER");
 char * device = getenv("ROAR_DEVICE");
 char * opts   = NULL;
// char * server = ROAR_DEFAULT_SOCK_GLOBAL;
 int      port = ROAR_DEFAULT_PORT;
 int               drvid;
 char * s_dev  = NULL;
 char * s_con  = NULL;
 char * s_opt  = NULL;
 int    s_prim = 0;
 char * sock_grp = "audio";
 struct group * grp;
 DRIVER_USERDATA_T drvinst;
 struct roar_client * self = NULL;

 g_listen_socket = -1;
 g_standby       =  0;

 sa.bits     = ROAR_BITS_DEFAULT;
 sa.channels = ROAR_CHANNELS_DEFAULT;
 sa.rate     = ROAR_RATE_DEFAULT;
 sa.codec    = ROAR_CODEC_DEFAULT;

 g_sa = &sa;


 if ( getuid() != 0 && getenv("HOME") ) {
  snprintf(user_sock, 79, "%s/%s", getenv("HOME"), ROAR_DEFAULT_SOCK_USER);
  server = user_sock;
 }

 if ( getenv("ROAR_SERVER") != NULL )
  server = getenv("ROAR_SERVER");

 if ( clients_init() == -1 ) {
  ROAR_ERR("Can not init clients!");
  return 1;
 }

 if ( streams_init() == -1 ) {
  ROAR_ERR("Can not init streams!");
  return 1;
 }

 if ( (g_self_client = clients_new()) == -1 ) {
  ROAR_ERR("Can not create self client!");
  return 1;
 }

 if ( sources_init() == -1 ) {
  ROAR_ERR("Can not init sources!");
  return 1;
 }

 if ( (sources_set_client(g_self_client)) == -1 ) {
  ROAR_ERR("Can not init set source client!");
  return 1;
 }

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "-h") == 0 || strcmp(k, "--help") == 0 ) {
   usage();
   return 0;

  } else if ( strcmp(k, "--demon") == 0 ) {
   demon = 1;
  } else if ( strcmp(k, "--terminate") == 0 ) {
   g_terminate = 1;
  } else if ( strcmp(k, "--realtime") == 0 ) {
   realtime++;

  } else if ( strcmp(k, "--list-cf") == 0 ) {
   print_codecfilterlist();
   return 0;

  } else if ( strcmp(k, "-R") == 0 || strcmp(k, "--rate") == 0 ) {
   sa.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "-B") == 0 || strcmp(k, "--bits") == 0 ) {
   sa.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "-C") == 0 || strcmp(k, "--chans") == 0 ) {
   sa.channels = atoi(argv[++i]);

  } else if ( strcmp(k, "-d") == 0 || strcmp(k, "--driver") == 0 ) {
   driver = argv[++i];
   if ( strcmp(driver, "list") == 0 ) {
    print_driverlist();
   }
  } else if ( strcmp(k, "-D") == 0 || strcmp(k, "--device") == 0 ) {
   device = argv[++i];
  } else if ( strcmp(k, "-dO") == 0 ) {
   opts = argv[++i];

  } else if ( strcmp(k, "-s") == 0 || strcmp(k, "--source") == 0 ) {
   k = argv[++i];
   if ( sources_add(k, s_dev, s_con, s_opt, s_prim) == -1 ) {
    ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, k);
   }
   s_opt = s_dev = s_con = NULL;
   s_prim = 0;
  } else if ( strcmp(k, "-S") == 0 ) {
   s_dev = argv[++i];
  } else if ( strcmp(k, "-sO") == 0 ) {
   s_opt = argv[++i];
  } else if ( strcmp(k, "-sC") == 0 ) {
   s_con = argv[++i];
  } else if ( strcmp(k, "-sP") == 0 ) {
   s_prim = 1;

  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--port") == 0 ) {
   port = atoi(argv[++i]);
  } else if ( strcmp(k, "-b") == 0 || strcmp(k, "--bind") == 0 || strcmp(k, "-s") == 0 || strcmp(k, "--sock") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "-t") == 0 ) {
   server = ROAR_DEFAULT_HOST;
  } else if ( strcmp(k, "-u") == 0 ) {
   // ignore this case as it is the default behavor.
  } else if ( strcmp(k, "-G") == 0 ) {
   sock_grp = argv[++i];

  } else if ( strcmp(k, "--no-listen") == 0 ) {
   *server = 0;
  } else if ( strcmp(k, "--client-fh") == 0 ) {
   if ( clients_set_fh(clients_new(), atoi(argv[++i])) == -1 ) {
    ROAR_ERR("main(*): Can not set client's fh");
    return 1;
   }

  } else {
   usage();
   return 1;
  }

 }

 ROAR_DBG("Server config: rate=%i, bits=%i, chans=%i", sa.rate, sa.bits, sa.channels);

 if ( midi_init() == -1 )
  ROAR_ERR("Can not initialize MIDI subsystem");

 if ( *server != 0 ) {
  if ( (g_listen_socket = roar_socket_listen(ROAR_SOCKET_TYPE_UNKNOWN, server, port)) == -1 ) {
   ROAR_ERR("Can not open listen socket!");
   return 1;
  }

  if ( *server == '/' ) {
   if ( (grp = getgrnam(sock_grp)) == NULL ) {
    ROAR_ERR("Can not get GID for group %s: %s", sock_grp, strerror(errno));
   } else {
    chown(server, -1, grp->gr_gid);
    if ( getuid() == 0 )
     chmod(server, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
   }
  }
 }

 if ( output_buffer_init(&sa) == -1 ) {
  ROAR_ERR("Can not init output buffer!");
  return 1;
 }

 if ( driver_open(&drvinst, &drvid, driver, device, &sa) == -1 ) {
  ROAR_ERR("Can not open output driver!");
  return 1;
 }

 if ( samples_init() == -1 ) {
  ROAR_ERR("Can not init samples!");
  return 1;
 }


 signal(SIGINT,  on_sig_int);
 signal(SIGCHLD, on_sig_chld);
 signal(SIGPIPE, SIG_IGN);  // ignore broken pipes

 if ( realtime ) {
#ifdef DEBUG
  ROAR_WARN("compiled with -DDEBUG but realtime is enabled: for real realtime support compiel without -DDEBUG");
#endif

  errno = 0;
  nice(-5*realtime); // -5 for each --realtime
  if ( errno )
   ROAR_WARN("Can not decrease nice value by 5: %s", strerror(errno));
/*
#ifdef __linux__
  if ( ioprio_set(IOPRIO_WHO_PROCESS, getpid(), IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, 0)) == -1 )
   ROAR_WARN("Can not set io priority: %s", strerror(errno));
#endif
*/
 }


 clients_set_pid(g_self_client, getpid());
 clients_get(g_self_client, &self);

 if ( self == NULL ) {
  ROAR_ERR("Can not get self client!");
  return 1;
 }

 strcpy(self->name, "RoarAudio demon internal");

 if ( demon ) {
  close(ROAR_STDIN );
  close(ROAR_STDOUT);
  close(ROAR_STDERR);
  if ( fork() )
   _exit(0);
 }

 // start main loop...
 main_loop(drvid, drvinst, &sa);

 // clean up.
 clean_quit_prep();
 driver_close(drvinst, drvid);
 output_buffer_free();

 return 0;
}

void clean_quit_prep (void) {
 close(g_listen_socket);

 if ( *server == '/' )
  unlink(server);


 sources_free();
 streams_free();
 clients_free();
 midi_cb_stop(); // stop console beep
 midi_free();
}

void clean_quit (void) {
 clean_quit_prep();
// driver_close(drvinst, drvid);
// output_buffer_free();
 exit(0);
}

//ll

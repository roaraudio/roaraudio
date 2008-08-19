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
        " --chroot DIR          - chroots to the given dir\n"
        " --setgid              - GroupID to the audio group as specified via -G\n"
        " --setuid              - UserID to the audio user as specified via -U\n"
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
#ifdef ROAR_HAVE_LIBDNET
        " -n  --decnet          - use DECnet listen socket\n"
#endif
        " -4                    - Use IPv4 connections (implies -t)\n"
#ifdef PF_INET6
        " -6                    - Use IPv6 connections (implies -t)\n"
#endif
#ifdef IPV6_ADDRFORM
        " -64                   - Try to downgrade sockets from IPv6 into IPv4,\n"
        "                         this is normaly not usefull.\n"
#endif
        " -p  --port            - TCP Port to bind to\n"
        " -b  --bind            - IP/Hostname to bind to\n"
        " -s  --sock            - Filename for UNIX Domain Socket\n"
        " -G  GROUP             - Sets the group for the UNIX Domain Socket, (default: %s)\n"
        "                         You need the permittions to change the GID\n"
        " -U  USER              - Sets the user for the UNIX Domain Socket, (default: do not set)\n"
        "                         You need the permittions to change the UID (normaly only root has)\n"
        " --no-listen           - Do not listen for new clients\n"
        "                         (only usefull for relaing, impleys --terminate)\n"
        " --client-fh           - Comunicate with a client over this handle\n"
        "                         (only usefull for relaing)\n"
        " --close-fh            - Closes the given fh\n",
        ROAR_DEFAULT_SOCKGRP
       );
// printf("\n Options:\n\n");
 printf("\n");
}

#define R_SETUID 1
#define R_SETGID 2

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
 char * s_drv     = "cf";
 char * s_con     = NULL;
 char * s_opt     = NULL;
 int    s_prim    = 0;
 char * sock_grp  = ROAR_DEFAULT_SOCKGRP;
 char * sock_user = NULL;
 int    sock_type = ROAR_SOCKET_TYPE_UNKNOWN;
 char * chrootdir = NULL;
 int    setids    = 0;
 struct group   * grp  = NULL;
 struct passwd  * pwd  = NULL;
 struct servent * serv = NULL;
 DRIVER_USERDATA_T drvinst;
 struct roar_client * self = NULL;
#ifdef ROAR_HAVE_LIBDNET
 char decnethost[80];
#endif

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
  } else if ( strcmp(k, "--chroot") == 0 ) {
   chrootdir = argv[++i];
  } else if ( strcmp(k, "--setgid") == 0 ) {
   setids |= R_SETGID;
  } else if ( strcmp(k, "--setuid") == 0 ) {
   setids |= R_SETUID;

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
   s_drv = argv[++i];
  } else if ( strcmp(k, "-S") == 0 ) {
   k = argv[++i];
   if ( sources_add(s_drv, k, s_con, s_opt, s_prim) == -1 ) {
    ROAR_ERR("main(*): adding source '%s' via '%s' failed!", k, s_drv);
   }
   s_opt = s_con = NULL;
   s_prim = 0;
  } else if ( strcmp(k, "-sO") == 0 ) {
   s_opt = argv[++i];
  } else if ( strcmp(k, "-sC") == 0 ) {
   s_con = argv[++i];
  } else if ( strcmp(k, "-sP") == 0 ) {
   s_prim = 1;

  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--port") == 0 ) {
   // This is only usefull in INET not UNIX mode.
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;

   errno = 0;
   if ( (port = atoi(argv[++i])) < 1 ) {
    if ( (serv = getservbyname(argv[i], "tcp")) == NULL ) {
     ROAR_ERR("Unknown service: %s: %s", argv[i], strerror(errno));
     return 1;
    }
    // NOTE: we need to use ROAR_NET2HOST16() here even if s_port is of type int!
    ROAR_DBG("main(*): serv = {s_name='%s', s_aliases={...}, s_port=%i, s_proto='%s'}",
            serv->s_name, ROAR_NET2HOST16(serv->s_port), serv->s_proto);
    port = ROAR_NET2HOST16(serv->s_port);
   }
  } else if ( strcmp(k, "-b") == 0 || strcmp(k, "--bind") == 0 || strcmp(k, "-s") == 0 || strcmp(k, "--sock") == 0 ) {
   server = argv[++i];

  } else if ( strcmp(k, "-t") == 0 || strcmp(k, "--tcp") == 0 ) {
   if ( sock_type != ROAR_SOCKET_TYPE_TCP && sock_type != ROAR_SOCKET_TYPE_TCP6 )
    sock_type = ROAR_SOCKET_TYPE_TCP;

   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;

  } else if ( strcmp(k, "-4") == 0 ) {
   sock_type = ROAR_SOCKET_TYPE_TCP;
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;
  } else if ( strcmp(k, "-6") == 0 ) {
#ifdef PF_INET6
   sock_type = ROAR_SOCKET_TYPE_TCP6;
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;
#else
    ROAR_ERR("No IPv6 support compiled in!");
    return 1;
#endif

  } else if ( strcmp(k, "-u") == 0 || strcmp(k, "--unix") == 0 ) {
   // ignore this case as it is the default behavor.
   sock_type = ROAR_SOCKET_TYPE_UNIX;

  } else if ( strcmp(k, "-n") == 0 || strcmp(k, "--decnet") == 0 ) {
#ifdef ROAR_HAVE_LIBDNET
    port   = ROAR_DEFAULT_NUM;
    strcpy(decnethost, ROAR_DEFAULT_LISTEN_OBJECT);
    server = decnethost;
    sock_type = ROAR_SOCKET_TYPE_DECNET;
#else
    ROAR_ERR("No DECnet support compiled in!");
    return 1;
#endif

  } else if ( strcmp(k, "-G") == 0 ) {
   sock_grp  = argv[++i];
  } else if ( strcmp(k, "-U") == 0 ) {
   sock_user = argv[++i];

  } else if ( strcmp(k, "--no-listen") == 0 ) {
   *server     = 0;
   g_terminate = 1;
  } else if ( strcmp(k, "--client-fh") == 0 ) {
   if ( clients_set_fh(clients_new(), atoi(argv[++i])) == -1 ) {
    ROAR_ERR("main(*): Can not set client's fh");
    return 1;
   }
  } else if ( strcmp(k, "--close-fh") == 0 ) {
   close(atoi(argv[++i]));

  } else {
   usage();
   return 1;
  }

 }

 ROAR_DBG("Server config: rate=%i, bits=%i, chans=%i", sa.rate, sa.bits, sa.channels);

 if ( midi_init() == -1 )
  ROAR_ERR("Can not initialize MIDI subsystem");

 if ( *server != 0 ) {
  if ( (g_listen_socket = roar_socket_listen(sock_type, server, port)) == -1 ) {
   if ( *server == '/' ) {
    if ( (i = roar_socket_connect(server, port)) != -1 ) {
     close(i);
     ROAR_ERR("Can not open listen socket!");
     return 1;
    } else {
     unlink(server);
     if ( (g_listen_socket = roar_socket_listen(sock_type, server, port)) == -1 ) {
      ROAR_ERR("Can not open listen socket!");
      return 1;
     }
    }
   } else {
    ROAR_ERR("Can not open listen socket!");
    return 1;
   }
  }

  if ( (grp = getgrnam(sock_grp)) == NULL ) {
   ROAR_ERR("Can not get GID for group %s: %s", sock_grp, strerror(errno));
  }
  if ( sock_user || (setids & R_SETUID) ) {
   if ( (pwd = getpwnam(sock_user)) == NULL ) {
    ROAR_ERR("Can not get UID for user %s: %s", sock_user, strerror(errno));
   }
  }

  if ( *server == '/' ) {
   if ( grp ) {
    if ( pwd ) {
     chown(server, pwd->pw_uid, grp->gr_gid);
    } else {
     chown(server, -1, grp->gr_gid);
    }
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

 if ( setids & R_SETGID ) {
  if ( setgroups(0, (const gid_t *) NULL) == -1 ) {
   ROAR_ERR("Can not clear supplementary group IDs: %s", strerror(errno));
  }
  if ( !grp || setgid(grp->gr_gid) == -1 ) {
   ROAR_ERR("Can not set GroupID: %s", strerror(errno));
  }
 }


 clients_set_pid(g_self_client, getpid());
 clients_set_uid(g_self_client, getuid());
 clients_set_gid(g_self_client, getgid());
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
  setsid();
  if ( fork() )
   _exit(0);
 }

 if (chrootdir) {
  if ( chroot(chrootdir) == -1 ) {
   ROAR_ERR("Can not chroot to %s: %s", chrootdir, strerror(errno));
   return 2;
  }
  if ( chdir("/") == -1 ) {
   ROAR_ERR("Can not chdir to /: %s", strerror(errno));
   return 2;
  }
 }

 if ( setids & R_SETUID ) {
  if ( !pwd || setuid(pwd->pw_uid) == -1 ) {
   ROAR_ERR("Can not set UserID: %s", strerror(errno));
   return 3;
  }
  clients_set_uid(g_self_client, getuid());
 }

 // start main loop...
 main_loop(drvid, drvinst, &sa);

 // clean up.
 clean_quit_prep();
 driver_close(drvinst, drvid);
 output_buffer_free();

 return 0;
}

void cleanup_listen_socket (int terminate) {
 close(g_listen_socket);

 g_listen_socket = -1;

 if ( *server == '/' )
  unlink(server);

 if ( terminate )
  g_terminate = 1;
}

void clean_quit_prep (void) {
 cleanup_listen_socket(0);

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

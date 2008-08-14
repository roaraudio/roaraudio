//acl.h:

#ifndef _ROARAUDIO_ACL_H_
#define _ROARAUDIO_ACL_H_

// basic rule constants
#define ROAR_ACL_DENY      0
#define ROAR_ACL_REJECT    ROAR_ACL_DENY
#define ROAR_ACL_ALLOW     1
#define ROAR_ACL_ALLOW_OWN 2

struct roar_acl_target {
 int connect;
 int stream;
 int connect_to;
 int exit;
 int standby;
 int kick;
 int list;
 int meta;
 int volume;
 int get_client;
 int get_stream;
 int beep;
};

#define ROAR_ACL_CHK(r,w,o) (( ((r)->w) == ROAR_ACL_ALLOW || (((r)->w) == ROAR_ACL_ALLOW_OWN && (o)) ) ? 1 : 0)

#define ROAR_ACL_CMP_NOT     0x10
#define ROAR_ACL_CMP_EQ      0x01

#define ROAR_ACL_TYPE_ID     1
#define ROAR_ACL_TYPE_INT    1
#define ROAR_ACL_TYPE_VOID   1

struct roar_acl_cmp {
 int op;
 int type;
 union {
  id_t   id;
  int    i;
  void * p;
 } data;
 int datalen;
};

// A rule.
// All entrys may be NULL to indicate not to compare them.
// The only option is addr_use_inet_port which is used to disable checking of
// the remote IP.
struct roar_acl_rule {
 pid_t           * pid;
 uid_t           * uid;
 gid_t           * gid;
 char            * username;
 int             * cookie;
 struct sockaddr * addr;
 socklen_t         addrlen;
 int               addr_use_inet_port;
};

#endif

//ll

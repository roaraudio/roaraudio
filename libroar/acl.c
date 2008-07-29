//acl.c:

#include "libroar.h"

int roar_acl_rule_chk_connect (struct roar_acl_rule * rule, struct sockaddr * addr, socklen_t addrlen) {

 if ( !rule )
  return -1;

 if ( rule->pid || rule->uid || rule->gid || rule->cookie )
  return 0;

 if ( rule->addr && !addr )
  return 0;

 return -1;
}

//ll

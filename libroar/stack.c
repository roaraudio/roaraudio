//stack.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_stack_new(struct roar_stack * stack) {
 if ( stack == NULL )
  return -1;

 memset(stack, 0, sizeof(struct roar_stack));

 stack->flags = ROAR_STACK_FLAG_FREE_DATA;
 stack->free  = free;

 return 0;
}

struct roar_stack * roar_stack_newalloc(void) {
 struct roar_stack * stack = malloc(sizeof(struct roar_stack));

 if ( stack == NULL )
  return NULL;

 if ( roar_stack_new(stack) == -1 ) {
  free(stack);
  return NULL;
 }

 if ( roar_stack_set_flag(stack, ROAR_STACK_FLAG_FREE_SELF, 0) == -1 ) {
  free(stack);
  return NULL;
 }

 return stack;
}

int roar_stack_free(struct roar_stack * stack) {
 int i;

 if ( stack == NULL )
  return -1;

 if ( (stack->flags & ROAR_STACK_FLAG_FREE_DATA) && stack->free != NULL ) {
  for (i = 0; i < stack->next; i++)
   stack->free(stack->slot[i]);
 }

 if ( stack->flags & ROAR_STACK_FLAG_FREE_SELF ) {
  free(stack);
 } else {
  memset(stack, 0, sizeof(struct roar_stack)); // just to be sure...
 }

 return 0;
}

int roar_stack_set_free(struct roar_stack * stack, void (*func)(void*)) {
 if ( stack == NULL )
  return -1;

 stack->free = func;

 return 0;
}

int roar_stack_set_flag(struct roar_stack * stack, int flag, int reset) {
 if ( stack == NULL )
  return -1;

 stack->flags  |= flag;

 if ( reset )
  stack->flags -= flag;

 return 0;
}

int roar_stack_push    (struct roar_stack * stack, void *  ptr) {
 if ( stack == NULL )
  return -1;

 if ( stack->next == ROAR_STACK_SIZE ) /* stack is full */
  return -1;

 stack->slot[stack->next++] = ptr;

 return 0;
}

int roar_stack_pop     (struct roar_stack * stack, void ** ptr) {
 if ( stack == NULL ) /* it is valid to have ptr = NULL, */
  return -1;          /* to just pop a value without want to take it */

 if ( ptr != NULL ) /* just to be sure */
  *ptr = NULL;

 if ( stack->next == 0 ) /* nothing in stack */
  return -1;

 stack->next--;

 if ( ptr != NULL )
  *ptr = stack->slot[stack->next];

 return 0;
}

int roar_stack_get_cur (struct roar_stack * stack, void ** ptr) {
 if ( stack == NULL || ptr == NULL )
  return -1;

 *ptr = NULL; /* just to be sure */

 if ( stack->next == 0 ) /* nothing in the stack */
  return -1;

 *ptr = stack->slot[stack->next - 1];

 return 0;
}

//ll

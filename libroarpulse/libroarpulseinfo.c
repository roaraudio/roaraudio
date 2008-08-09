//*.c:

#include <libroarpulse/libroarpulse.h>

int main (void) {
 struct { char * name; int len; } types[] = {
//  { "pa_simple",      sizeof(pa_simple)      },
  { "pa_sample_spec", sizeof(pa_sample_spec) },
  { NULL, 0 }
 }, * c = types - 1;

 printf("Types:\n");
 while ((++c)->name != NULL)
  printf("%-20s = %3i Bytes = %4i Bits\n", c->name, c->len, c->len * 8);

 return 0;
}

//ll

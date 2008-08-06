//polytest.c:

#include <roaraudio.h>

#define MAX 1024

int main (void) {
 int16_t  in[MAX];
 int16_t out[MAX];
 size_t il = 10;
 size_t ol = 5;
 int i;

 for (i = 0; i < il; i++)
  scanf("%i", &in[i]);

 roar_conv_poly4_16(out, in, ol, il);

 for (i = 0; i < ol; i++)
  printf("%i\n", out[i]);

 return 0;
}

//ll

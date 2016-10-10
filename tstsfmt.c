#include "sfmt.h"

int main(void)
{
    int i;
    uint* Mtrix;

    Mtrix = SeedGen();

    printf("10 outputs of GenRandom_UInt()\n");

    for (i=0; i<10; i++) {
      printf("%10lu ", GenRandom_UInt(Mtrix));
      if (i%5==4) printf("\n");
    }

    printf("\n10 outputs of GenRandom_UReal()\n");
    for (i=0; i<10; i++) {
      printf("%10.8f ", GenRandom_UReal(Mtrix));
      if (i%5==4) printf("\n");
    }

    free(Mtrix);

    return 0;
}

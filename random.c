#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define RS_SCALE (1.0 / (1.0 + RAND_MAX))


int randbiased (double x) {
    for (;;) {
        double p = rand() * RS_SCALE;
        if (p >= x) return 0;
        if (p + RS_SCALE <= x) return 1;
        /* p < x < p+RS_SCALE */
        x = (x - p) * (1.0 + RAND_MAX);
    }
}

unsigned int randrange (unsigned int n) {
    double xhi;
    double resolution = n * RS_SCALE;
    double x = resolution * rand (); /* x in [0,n) */
    unsigned int lo = (unsigned int) floor (x);

    xhi = x + resolution;

    for (;;) {
        lo++;
        if (lo >= xhi || randbiased ((lo - x) / (xhi - x))) return lo-1;
        x = lo;
    }
}

int main (int argn, char* argc[])
{
    unsigned int t;

    srand (time (NULL));

    printf(" %ld - %ld - %ld - %ld\n", randrange (10), randrange (100), randrange (1000), randrange (10000));

    printf(" %02x %02x %02x \n", randrange (256), randrange (256), randrange (256) );
    printf(" %02x %02x %02x \n", randrange (256), randrange (256), randrange (256) );



    return 1;
}


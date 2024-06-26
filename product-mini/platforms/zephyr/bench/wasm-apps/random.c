#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>

#define random_LENGTH 1000000 // 40000000
#define random_IA 3877
#define random_IC 29573
#define random_IM 139968

static inline double
gen_random(double max, long ia, long ic, long im)
{
    static long last = 42;
    last = (last * ia + ic) % im;
    return max * last / im;
}

int bench_random()
{
    long ia = random_IA;
    long ic = random_IC;
    long im = random_IM;
    int n = random_LENGTH;
    BLACK_BOX(n);

    printf("[random] generating random number in %d iterations\n", n);
    bench_start();
    while (n--) {
        gen_random(100.0, ia, ic, im);
    }
    double res = gen_random(100.0, ia, ic, im);
    bench_end();
    printf("[random] returned %f\n", res);

    BLACK_BOX(res);
}
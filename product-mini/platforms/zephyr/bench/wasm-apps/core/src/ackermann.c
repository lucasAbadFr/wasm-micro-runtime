#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../benchmark.h"

int ackermann(int M, int N)
{
    if (M == 0)
    {
        return N + 1;
    }
    if (N == 0)
    {
        return ackermann(M - 1, 1);
    }
    return ackermann(M - 1, ackermann(M, (N - 1)));
}

int bench_ackermann()
{
    int M = 3;
    int N = 3;
    printf("[ackermann] running with M = %d and N = %d\n", M, N);

    bench_start();
    int result = ackermann(M, N);
    bench_end();

    printf("[ackermann] returned %d\n", result);
}
#include "../../benchmark.h"

#include <stdio.h>
#include <stdlib.h>

#define sieve_LENGTH 1400 // 17000
#define sieve_SIZE 256 // 8192

int bench_sieve()
{
    static char flags[sieve_SIZE + 1];
    unsigned long res;
    int n = sieve_LENGTH;
    long i, k;
    int count;

    res = 0;
    printf("[sieve] running sieve with n = %d\n", n);
    bench_start();
    while (n--)
    {
        count = 0;
        for (i = 2; i <= sieve_SIZE; i++)
        {
            flags[i] = 1;
        }
        for (i = 2; i <= sieve_SIZE; i++)
        {
            if (flags[i])
            {
                /* remove all multiples of prime: i */
                for (k = i + i; k <= sieve_SIZE; k += i)
                {
                    flags[k] = 0;
                }
                count++;
            }
        }
        res += count;
    }
    bench_end();

    BLACK_BOX(res);
    printf("[sieve] returned %d\n", res);
}
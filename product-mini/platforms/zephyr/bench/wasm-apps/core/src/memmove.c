#include "../../benchmark.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define memmove_STR_SIZE 100 // 10000
#define memmove_ITERATIONS 5 // 10

int bench_memmove()
{
    size_t str_size = memmove_STR_SIZE;
    char* str = calloc(str_size, (size_t)1U);
    if(str == NULL) return -1;

    bench_start();
    int i;
    size_t j;
    for (i = 0; i < memmove_ITERATIONS; i++) {
        BLACK_BOX(str);
        for (j = (size_t)0U; j < str_size - (size_t)1U; j++) {
            memmove(&str[j], str, str_size - j);
        }
        for (j = (size_t)0U; j < str_size - (size_t)1U; j++) {
            memmove(str, &str[j], str_size - j);
        }
    }
    bench_end();

    char ret = str[0];
    BLACK_BOX(str);
    free(str);
}
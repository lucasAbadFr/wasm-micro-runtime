#ifndef CONFIG_H
#define CONFIG_H

#if ENABLE_BENCHMARK_ACKERMANN
    #define BENCHMARK_ACKERMANN 1
#else
    #define BENCHMARK_ACKERMANN 0
#endif

#if ENABLE_BENCHMARK_BASE64
    #define BENCHMARK_BASE64 1
#else
    #define BENCHMARK_BASE64 0
#endif

#if ENABLE_BENCHMARK_FIB2
    #define BENCHMARK_FIB2 1
#else
    #define BENCHMARK_FIB2 0
#endif

#if ENABLE_BENCHMARK_GIMLI
    #define BENCHMARK_GIMLI 1
#else
    #define BENCHMARK_GIMLI 0
#endif

#if ENABLE_BENCHMARK_MATRIX
    #define BENCHMARK_MATRIX 1
#else
    #define BENCHMARK_MATRIX 0
#endif

#if ENABLE_BENCHMARK_MEMMOVE
    #define BENCHMARK_MEMMOVE 1
#else
    #define BENCHMARK_MEMMOVE 0
#endif

#if ENABLE_BENCHMARK_RANDOM
    #define BENCHMARK_RANDOM 1
#else
    #define BENCHMARK_RANDOM 0
#endif

#if ENABLE_BENCHMARK_SIEVE
    #define BENCHMARK_SIEVE 1
#else
    #define BENCHMARK_SIEVE 0
#endif

#if ENABLE_BENCHMARK_HEAPSORT
    #define BENCHMARK_HEAPSORT 1
#else
    #define BENCHMARK_HEAPSORT 0
#endif

#if ENABLE_BENCHMARK_NESTEDLOOP
    #define BENCHMARK_NESTEDLOOP 1
#else
    #define BENCHMARK_NESTEDLOOP 0
#endif

#if ENABLE_BENCHMARK_DUMMY
    #define BENCHMARK_DUMMY 1
#else
    #define BENCHMARK_DUMMY 0
#endif

#endif /* CONFIG_H */
#include "native_benchmark.h"
#include <string.h>
#include <stdlib.h>

// Implementation is not good because we initialize the wasm
// modules in bulk so it consumes more memory than necessary
// We should initialize the wasm modules one by one
// We need to refactor this code to have a more generalized interface
// eg: 
// ```c
// typedef void (*BenchmarkFunction)();

// typedef struct {
//     char* name;
//     BenchmarkFunction native_function; 
//     uint8_t* wasm_module;              
//     size_t wasm_module_size;           
// } Benchmark;
// ```
// We also need to avoid macros usage as it greatly dicreases the 
// readability of the code.

/* import native benchmarks main functions*/
extern int bench_ackermann();
extern int bench_base64();
extern int bench_fib2();
extern int bench_gimli();
extern int bench_matrix();
extern int bench_memmove();
extern int bench_random();
extern int bench_sieve();
extern int bench_heapsort();
extern int bench_nestedloop();
extern int bench_dummy();

/* Global counter values */
static uint64_t native_bench_start_time = 0;
static uint64_t native_bench_end_time = 0;

/* function to ensure compatibility */
void bench_start(void){
    native_bench_start_time = k_cycle_get_64(); 
}

void bench_end(void){
    native_bench_end_time = k_cycle_get_64();
}

void reset_times(){
    native_bench_start_time = 0;
    native_bench_end_time = 0;
}

//-------------------------------------------------------------------------------------------//
void benchmark_banner(void){
    printk("[INFO] Performing benchmarks:\n");
    printk("\t - ackermann :             %s\n", BENCHMARK_ACKERMANN ? "YES" : "NO");
    printk("\t - base64 :                %s\n", BENCHMARK_BASE64 ? "YES" : "NO");
    printk("\t - fib2 :                  %s\n", BENCHMARK_FIB2 ? "YES" : "NO");
    printk("\t - gimli :                 %s\n", BENCHMARK_GIMLI ? "YES" : "NO");
    printk("\t - matrix :                %s\n", BENCHMARK_MATRIX ? "YES" : "NO");
    printk("\t - memmove :               %s\n", BENCHMARK_MEMMOVE ? "YES" : "NO");
    printk("\t - random :                %s\n", BENCHMARK_RANDOM ? "YES" : "NO");
    printk("\t - sieve :                 %s\n", BENCHMARK_SIEVE ? "YES" : "NO");
    printk("\t - heapsort (unsupported): %s\n", BENCHMARK_HEAPSORT ? "YES" : "NO");
    printk("\t - nestedloop :            %s\n", BENCHMARK_NESTEDLOOP ? "YES" : "NO");
    printk("\t - dummy :                 %s\n", BENCHMARK_DUMMY ? "YES" : "NO");
}

//-------------------------------------------------------------------------------------------//
void call_bench_main(void){
#if BENCHMARK_ACKERMANN
    bench_ackermann();
    printk("[ackermann] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_BASE64
    bench_base64();
    printk("[base64] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FIB2
    bench_fib2();
    printk("[fib2] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_GIMLI
    bench_gimli();
    printk("[gimli] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_MATRIX
    bench_matrix();
    printk("[matrix] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_MEMMOVE
    bench_memmove();
    printk("[memmove] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_RANDOM
    bench_random();
    printk("[random] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_SIEVE
    bench_sieve();
    printk("[sieve] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_HEAPSORT
    bench_heapsort();
    printk("[heapsort] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_NESTEDLOOP
    bench_nestedloop();
    printk("[nestedloop] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_DUMMY
    bench_dummy();
    printk("[dummy] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif
}

//-------------------------------------------------------------------------------------------//
#define MAX_BENCHMARKS 11

Benchmark benchmarks[MAX_BENCHMARKS];
int bench_nb = -1;

// mutex to protect the benchmark array
K_MUTEX_DEFINE(bench_array_mutex);

void allocate_benchmark(const char* func_name, const uint8_t* wasm_module, size_t wasm_module_size) {
    Benchmark *ptr = NULL;

    k_mutex_lock(&bench_array_mutex, K_FOREVER);
    for (int i = 0; i < MAX_BENCHMARKS; ++i) {
        // Find an unused entry
        if (benchmarks[i].func_name == NULL) { 
            ptr = &benchmarks[i];
            ptr->func_name = strdup(func_name);
            ptr->wasm_module = malloc(wasm_module_size);
            memcpy(ptr->wasm_module, wasm_module, wasm_module_size);
            ptr->wasm_module_size = wasm_module_size;
            bench_nb++;
            break;
        }
    }
    k_mutex_unlock(&bench_array_mutex);
}

void debugBenchmark(const Benchmark* benchmark) {
    printk("Function Name: %s\n", benchmark->func_name);
    printk("WASM Module: ");
    for (size_t i = 0; i < benchmark->wasm_module_size; ++i) {
        printk("%02X ", benchmark->wasm_module[i]);
    }
    printk("\n");
    printk("WASM Module Size: %zu bytes\n", benchmark->wasm_module_size);
}

void deallocate_benchmark(Benchmark* bench) {
    if (!bench) return;
    free(bench->func_name);
    free(bench->wasm_module);
    bench->func_name = NULL;
    bench->wasm_module = NULL;
}

void cleanup_benchmarks() {
    k_mutex_lock(&bench_array_mutex, K_FOREVER);
    for (int i = 0; i <= bench_nb; ++i) {
        deallocate_benchmark(&benchmarks[i]);
    }
    bench_nb = -1; // Reset the benchmark counter
    k_mutex_unlock(&bench_array_mutex);
}

/* Include all webassembly modules */
void initialize_benchmarks() {
    uint8_t *wasm_file_buf = NULL;
    size_t wasm_file_size = 0;

printk("[INFO] Initializing webassembly benchmarks\n");

#if BENCHMARK_ACKERMANN
#include "ackermann.h"
    wasm_file_buf = (uint8_t *)wasm_ackermann;
    wasm_file_size = sizeof(wasm_ackermann);

    allocate_benchmark("bench_ackermann", wasm_file_buf, wasm_file_size);
#endif

#if BENCHMARK_BASE64
#include "base64.h"
    wasm_file_buf = (uint8_t *)wasm_base64;
    wasm_file_size = sizeof(wasm_base64);

    allocate_benchmark("bench_base64", wasm_file_buf, wasm_file_size);
#endif

#if BENCHMARK_FIB2
#include "fib2.h"
    wasm_file_buf = (uint8_t *)wasm_fib2;
    wasm_file_size = sizeof(wasm_fib2);

    allocate_benchmark("bench_fib2", wasm_fib2, wasm_file_size);
#endif

#if BENCHMARK_GIMLI
#include "gimli.h"
    wasm_file_buf = (uint8_t *)wasm_gimli;
    wasm_file_size = sizeof(wasm_gimli);

    allocate_benchmark("bench_gimli", wasm_gimli, wasm_file_size);
#endif

#if BENCHMARK_MATRIX
#include "matrix.h"
    wasm_file_buf = (uint8_t *)wasm_matrix;
    wasm_file_size = sizeof(wasm_matrix);

    allocate_benchmark("bench_matrix", wasm_matrix, wasm_file_size);
#endif

#if BENCHMARK_MEMMOVE
#include "memmove.h"
    wasm_file_buf = (uint8_t *)wasm_memmove;
    wasm_file_size = sizeof(wasm_memmove);

    allocate_benchmark("bench_memmove", wasm_memmove, wasm_file_size);
#endif

#if BENCHMARK_RANDOM
#include "random.h"
    wasm_file_buf = (uint8_t *)wasm_random;
    wasm_file_size = sizeof(wasm_random);

    allocate_benchmark("bench_random", wasm_random, wasm_file_size);
#endif

#if BENCHMARK_SIEVE
#include "sieve.h"
    wasm_file_buf = (uint8_t *)wasm_sieve;
    wasm_file_size = sizeof(wasm_sieve);

    allocate_benchmark("bench_sieve", wasm_sieve, wasm_file_size);
#endif

#if BENCHMARK_HEAPSORT
#include "heapsort.h"
    wasm_file_buf = (uint8_t *)wasm_heapsort;
    wasm_file_size = sizeof(wasm_heapsort);

    allocate_benchmark("bench_heapsort", wasm_heapsort, wasm_file_size);
#endif

#if BENCHMARK_NESTEDLOOP
#include "nestedloop.h"
    wasm_file_buf = (uint8_t *)wasm_nestedloop;
    wasm_file_size = sizeof(wasm_nestedloop);

    allocate_benchmark("bench_nestedloop", wasm_nestedloop, wasm_file_size);
#endif

#if BENCHMARK_DUMMY
#include "dummy.h"
    wasm_file_buf = (uint8_t *)wasm_dummy;
    wasm_file_size = sizeof(wasm_dummy);

    allocate_benchmark("bench_dummy", wasm_dummy, wasm_file_size);
#endif
}
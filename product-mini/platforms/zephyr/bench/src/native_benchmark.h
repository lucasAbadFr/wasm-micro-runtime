#ifndef BENCHMARK_H
#define BENCHMARK_H 

#include <zephyr/kernel.h>
#include "config.h" 

/* types and functions definitions */
typedef struct {
    char* func_name;            // Name of the function to run
    const uint8_t *wasm_module;   // Pointer to the WASM module array
    size_t wasm_module_size;    // Size of the WASM module array
} Benchmark;

void call_bench_main(void);
void initialize_benchmarks();
void cleanup_benchmarks();
void deallocate_benchmark(Benchmark* bench);
void benchmark_banner(void);

#endif /* BENCHMARK_H */
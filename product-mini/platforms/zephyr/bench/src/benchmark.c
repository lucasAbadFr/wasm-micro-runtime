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
extern int bench_socket_create();
extern int bench_socket_connect();
extern int bench_socket_bind();
extern int bench_socket_sendto();
extern int bench_socket_recvfrom();
extern int bench_fs_fopen();
extern int bench_fs_fwrite();
extern int bench_fs_fread();
extern int bench_fs_fseek();
extern int bench_fs_fclose();
extern int bench_fs_unlink();
extern int bench_fs_mkdir();
extern int bench_fs_rename();
extern int bench_fs_perf();

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
    printk("\t - socket_create :         %s\n", BENCHMARK_SOCKET_CREATE ? "YES" : "NO");
    printk("\t - socket_connect :        %s\n", BENCHMARK_SOCKET_CONNECT ? "YES" : "NO");
    printk("\t - socket_bind :           %s\n", BENCHMARK_SOCKET_BIND ? "YES" : "NO");
    printk("\t - socket_sendto :         %s\n", BENCHMARK_SOCKET_SENDTO ? "YES" : "NO");
    printk("\t - socket_recvfrom :       %s\n", BENCHMARK_SOCKET_RECVFROM ? "YES" : "NO");
    printk("\t - fs_fopen :              %s\n", BENCHMARK_FS_FOPEN ? "YES" : "NO");
    printk("\t - fs_fwrite :             %s\n", BENCHMARK_FS_FWRITE ? "YES" : "NO");
    printk("\t - fs_fread :              %s\n", BENCHMARK_FS_FREAD ? "YES" : "NO");
    printk("\t - fs_fseek :              %s\n", BENCHMARK_FS_FSEEK ? "YES" : "NO");
    printk("\t - fs_fclose :             %s\n", BENCHMARK_FS_FCLOSE ? "YES" : "NO");
    printk("\t - fs_unlink :             %s\n", BENCHMARK_FS_UNLINK ? "YES" : "NO");
    printk("\t - fs_mkdir :              %s\n", BENCHMARK_FS_MKDIR ? "YES" : "NO");
    printk("\t - fs_rename :             %s\n", BENCHMARK_FS_RENAME ? "YES" : "NO");
    printk("\t - fs_perf :               %s\n", BENCHMARK_FS_PERF ? "YES" : "NO");
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

#if BENCHMARK_SOCKET_CREATE
    bench_socket_create();
    printk("[socket_create] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_SOCKET_CONNECT
    bench_socket_connect();
    printk("[socket_connect] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_SOCKET_BIND
    bench_socket_bind();
    printk("[socket_bind] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_SOCKET_SENDTO
    bench_socket_sendto();
    printk("[socket_sendto] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_SOCKET_RECVFROM
    bench_socket_recvfrom();
    printk("[socket_recvfrom] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_FOPEN
    bench_fs_fopen();
    printk("[fs_fopen] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_FWRITE
    bench_fs_fwrite();
    printk("[fs_fwrite] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_FREAD
    bench_fs_fread();
    printk("[fs_fread] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_FSEEK
    bench_fs_fseek();
    printk("[fs_fseek] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_FCLOSE
    bench_fs_fclose();
    printk("[fs_fclose] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_UNLINK
    bench_fs_unlink();
    printk("[fs_unlink] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_MKDIR
    bench_fs_mkdir();
    printk("[fs_mkdir] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_RENAME
    bench_fs_rename();
    printk("[fs_rename] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif

#if BENCHMARK_FS_PERF
    bench_fs_perf();
    printk("[fs_perf] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
    reset_times();
#endif
}

//-------------------------------------------------------------------------------------------//
#define MAX_BENCHMARKS 15

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

#if BENCHMARK_SOCKET_CREATE
#include "socket_create.h"
    wasm_file_buf = (uint8_t *)wasm_socket_create;
    wasm_file_size = sizeof(wasm_socket_create);
    allocate_benchmark("bench_socket_create", wasm_socket_create, wasm_file_size);
#endif

#if BENCHMARK_SOCKET_CONNECT
#include "socket_connect.h"
    wasm_file_buf = (uint8_t *)wasm_socket_connect;
    wasm_file_size = sizeof(wasm_socket_connect);
    allocate_benchmark("bench_socket_connect", wasm_socket_connect, wasm_file_size);
#endif

#if BENCHMARK_SOCKET_BIND
#include "socket_bind.h"
    wasm_file_buf = (uint8_t *)wasm_socket_bind;
    wasm_file_size = sizeof(wasm_socket_bind);
    allocate_benchmark("bench_socket_bind", wasm_socket_bind, wasm_file_size);
#endif

#if BENCHMARK_SOCKET_SENDTO
#include "socket_sendto.h"
    wasm_file_buf = (uint8_t *)wasm_socket_sendto;
    wasm_file_size = sizeof(wasm_socket_sendto);
    allocate_benchmark("bench_socket_sendto", wasm_socket_sendto, wasm_file_size);
#endif

#if BENCHMARK_SOCKET_RECVFROM
#include "socket_recvfrom.h"
    wasm_file_buf = (uint8_t *)wasm_socket_recvfrom;
    wasm_file_size = sizeof(wasm_socket_recvfrom);
    allocate_benchmark("bench_socket_recvfrom", wasm_socket_recvfrom, wasm_file_size);
#endif

#if BENCHMARK_FS_FOPEN
#include "fs_fopen.h"
    wasm_file_buf = (uint8_t *)wasm_fs_fopen;
    wasm_file_size = sizeof(wasm_fs_fopen);
    allocate_benchmark("bench_fs_fopen", wasm_fs_fopen, wasm_file_size);
#endif

#if BENCHMARK_FS_FWRITE
#include "fs_fwrite.h"
    wasm_file_buf = (uint8_t *)wasm_fs_fwrite;
    wasm_file_size = sizeof(wasm_fs_fwrite);
    allocate_benchmark("bench_fs_fwrite", wasm_fs_fwrite, wasm_file_size);
#endif

#if BENCHMARK_FS_FREAD
#include "fs_fread.h"
    wasm_file_buf = (uint8_t *)wasm_fs_fread;
    wasm_file_size = sizeof(wasm_fs_fread);
    allocate_benchmark("bench_fs_fread", wasm_fs_fread, wasm_file_size);
#endif

#if BENCHMARK_FS_FSEEK
#include "fs_fseek.h"
    wasm_file_buf = (uint8_t *)wasm_fs_fseek;
    wasm_file_size = sizeof(wasm_fs_fseek);
    allocate_benchmark("bench_fs_fseek", wasm_fs_fseek, wasm_file_size);
#endif

#if BENCHMARK_FS_FCLOSE
#include "fs_fclose.h"
    wasm_file_buf = (uint8_t *)wasm_fs_fclose;
    wasm_file_size = sizeof(wasm_fs_fclose);
    allocate_benchmark("bench_fs_fclose", wasm_fs_fclose, wasm_file_size);
#endif

#if BENCHMARK_FS_UNLINK
#include "fs_unlink.h"
    wasm_file_buf = (uint8_t *)wasm_fs_unlink;
    wasm_file_size = sizeof(wasm_fs_unlink);
    allocate_benchmark("bench_fs_unlink", wasm_fs_unlink, wasm_file_size);
#endif

#if BENCHMARK_FS_MKDIR
#include "fs_mkdir.h"
    wasm_file_buf = (uint8_t *)wasm_fs_mkdir;
    wasm_file_size = sizeof(wasm_fs_mkdir);
    allocate_benchmark("bench_fs_mkdir", wasm_fs_mkdir, wasm_file_size);
#endif

#if BENCHMARK_FS_RENAME
#include "fs_rename.h"
    wasm_file_buf = (uint8_t *)wasm_fs_rename;
    wasm_file_size = sizeof(wasm_fs_rename);
    allocate_benchmark("bench_fs_rename", wasm_fs_rename, wasm_file_size);
#endif

#if BENCHMARK_FS_PERF
#include "fs_perf.h"
    wasm_file_buf = (uint8_t *)wasm_fs_perf;
    wasm_file_size = sizeof(wasm_fs_perf);
    allocate_benchmark("bench_fs_perf", wasm_fs_perf, wasm_file_size);
#endif
}
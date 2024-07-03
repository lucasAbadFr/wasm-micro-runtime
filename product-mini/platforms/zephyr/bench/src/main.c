/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// #include <autoconf.h>

#include <stdlib.h>
#include <string.h>
#include "bh_platform.h"
#include "bh_assert.h"
#include "bh_log.h"
#include "wasm_export.h"

/* Include wasm module as a header */
#include "ackermann.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <unistd.h>

#define CONFIG_HEAP_MEM_POOL_SIZE WASM_GLOBAL_HEAP_SIZE
#define CONFIG_APP_STACK_SIZE 8192 // 16384 // 8192 
#define CONFIG_APP_HEAP_SIZE 8192 // 16384 // 8192

LOG_MODULE_REGISTER(main);

static char global_heap_buf[CONFIG_HEAP_MEM_POOL_SIZE] = { 0 };

static int app_argc;
static char **app_argv;

//-------------------------------------------------------------------------------------------//
#if WASM_ENABLE_LIBC_WASI != 0
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

static int littlefs_flash_erase(unsigned int id)
{
	const struct flash_area *pfa;
	int rc;

	rc = flash_area_open(id, &pfa);
	if (rc < 0) {
		LOG_ERR("FAIL: unable to find flash area %u: %d\n",
			id, rc);
		return rc;
	}

	LOG_PRINTK("Area %u at 0x%x on %s for %u bytes\n",
		   id, (unsigned int)pfa->fa_off, pfa->fa_dev->name,
		   (unsigned int)pfa->fa_size);

	/* Optional wipe flash contents */
	if (IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
		LOG_ERR("Erasing flash area ... %d", rc);
	}

	flash_area_close(pfa);
	return rc;
}
#define PARTITION_NODE DT_NODELABEL(lfs1)

#if DT_NODE_EXISTS(PARTITION_NODE)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);
#else /* PARTITION_NODE */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
static struct fs_mount_t lfs_storage_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &storage,
	.storage_dev = (void *)FIXED_PARTITION_ID(storage_partition),
	.mnt_point = "/lfs",
};
#endif /* PARTITION_NODE */

	struct fs_mount_t *mountpoint =
#if DT_NODE_EXISTS(PARTITION_NODE)
		&FS_FSTAB_ENTRY(PARTITION_NODE)
#else
		&lfs_storage_mnt
#endif
		;

static int littlefs_mount(struct fs_mount_t *mp)
{
	int rc;

	rc = littlefs_flash_erase((uintptr_t)mp->storage_dev);
	if (rc < 0) {
		return rc;
	}

	/* Do not mount if auto-mount has been enabled */
#if !DT_NODE_EXISTS(PARTITION_NODE) ||						\
	!(FSTAB_ENTRY_DT_MOUNT_FLAGS(PARTITION_NODE) & FS_MOUNT_FLAG_AUTOMOUNT)
	rc = fs_mount(mp);
	if (rc < 0) {
		LOG_PRINTK("FAIL: mount id %" PRIuPTR " at %s: %d\n",
		       (uintptr_t)mp->storage_dev, mp->mnt_point, rc);
		return rc;
	}
	LOG_PRINTK("%s mount: %d\n", mp->mnt_point, rc);
#else
	LOG_PRINTK("%s automounted\n", mp->mnt_point);
#endif

	return 0;
}
#endif 

//-------------------------------------------------------------------------------------------//
static uint64_t wasm_bench_start_time = 0;
static uint64_t wasm_bench_end_time = 0;

void wasm_bench_start(wasm_exec_env_t exec_env){
    wasm_bench_start_time =  k_cycle_get_64(); 
}

void wasm_bench_end(wasm_exec_env_t exec_env){
    wasm_bench_end_time =  k_cycle_get_64();
}

void wasm_reset_times(){
    wasm_bench_start_time = 0;
    wasm_bench_end_time = 0;
}
static NativeSymbol native_symbols[] =
{
    {
        "bench_start",
     	wasm_bench_start,
        "()"
    },
    {
        "bench_end",
        wasm_bench_end,
        "()"
    }
};

//-------------------------------------------------------------------------------------------//
#include "native_benchmark.h"
/* array conataining every benchmark info*/
extern Benchmark benchmarks[10];
extern int bench_nb;

/* fucntion to load an run a benchmark */
bool load_and_run_bench(const Benchmark *benchmark) {
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    wasm_function_inst_t func = NULL;
    const char *exception;
    int func_argv[1];
    char error_buf[128];

    /* Load WASM module */
    if (!(wasm_module = wasm_runtime_load(benchmark->wasm_module, benchmark->wasm_module_size, error_buf, sizeof(error_buf)))) {
        LOG_ERR("Failed to load module: %s", error_buf);
        return false;
    }

        /* Set the WASI context */
#if WASM_ENABLE_LIBC_WASI != 0
    #define ADDRESS_POOL_SIZE 1
    const char *addr_pool[ADDRESS_POOL_SIZE] = {
        "192.0.2.10/24"};

    wasm_runtime_set_wasi_addr_pool(wasm_module, addr_pool, ADDRESS_POOL_SIZE);
#endif /* WASM_ENABLE_LIBC_WASI */

    /* Instantiate the module */
    if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module, CONFIG_APP_STACK_SIZE, CONFIG_APP_HEAP_SIZE, error_buf, sizeof(error_buf)))) {
        LOG_ERR("Failed to instantiate module: %s", error_buf);
        wasm_runtime_unload(wasm_module);
        return false;
    }

    /* Invoke the main function */
    if ((func = wasm_runtime_lookup_function(wasm_module_inst, benchmark->func_name))) {
        wasm_exec_env_t exec_env = wasm_runtime_create_exec_env(wasm_module_inst, CONFIG_APP_STACK_SIZE);
        if (!exec_env) {
            LOG_ERR("Create wasm execution environment failed.");
            return -1;
        }
        wasm_runtime_call_wasm(exec_env, func, 1, func_argv);
        k_sleep(K_MSEC(1000)); // wait for execution to finish 
        wasm_runtime_destroy_exec_env(exec_env);
    }

    if ((exception = wasm_runtime_get_exception(wasm_module_inst)))
    LOG_ERR("get exception: %s", exception);
    
#if WASM_ENABLE_LIBC_WASI != 0
    int rc = wasm_runtime_get_wasi_exit_code(wasm_module_inst);
    LOG_INF("wasi exit code: %d", rc);
#endif

    /* Deinstantiate the module */
    wasm_runtime_deinstantiate(wasm_module_inst);

    /* Unload the module */
    wasm_runtime_unload(wasm_module);

    return true; // Return true if everything went well
}


//-------------------------------------------------------------------------------------------//
int main(void)
{
    int start, end;
    start = k_uptime_get_32();
    uint8 *wasm_file_buf = NULL;
    uint32 wasm_file_size;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    RuntimeInitArgs init_args;
    char error_buf[128];
    const char *exception;
    int rc;

    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    memset(benchmarks, 0, sizeof(benchmarks));

    printk("timer has 64bits : %s\n", CONFIG_TIMER_HAS_64BIT_CYCLE_COUNTER ? "yes" : "no");

    /* Display info about benchmarks run */
    benchmark_banner();
    printk("\n");

    /* Initialize every benchmarks information in the benchmarks array*/
    initialize_benchmarks();

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
    LOG_INF("global heap size: %d", sizeof(global_heap_buf));
#else
#error "memory allocation scheme is not defined."
#endif

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        LOG_ERR("Init runtime environment failed.");
        return;
    }

    /* register native function */
    int n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    if (!wasm_runtime_register_natives("env",
                                   native_symbols,
                                   n_native_symbols)){
        LOG_ERR("Register natives failed.");
    }

    /* Load and run all wasm benchmarks */
    printk("[INFO] Running wasm benchmarks\n");
    for (int i = 0; i <= bench_nb; ++i) { 
        if(benchmarks[i].func_name != NULL){
            if(load_and_run_bench(&benchmarks[i])){
                printk("[%s] elapsed: %llu ticks\n", benchmarks[i].func_name + 6, (wasm_bench_end_time - wasm_bench_start_time));
                wasm_reset_times();
                deallocate_benchmark(&benchmarks[i]); // Deallocate the benchmark
            } else {
                LOG_ERR("Failed to run benchmark: %s", benchmarks[i].func_name);
                deallocate_benchmark(&benchmarks[i]); // Deallocate the benchmark
            } 
        }
    }

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();
    cleanup_benchmarks();
    end = k_uptime_get_32();
    
    printk("\n");
    LOG_INF("runtime elapsed: %d ms", (end - start));
    printk("\n");
    
    /* perform native benchmarks */
    printk("[INFO] Running native benchmarks\n");
    call_bench_main();

    return 0;
}
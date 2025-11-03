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

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/client.h>

/* Hello World WASM module */
#include "wasi_nn_module.h"

#define CONFIG_HEAP_MEM_POOL_SIZE WASM_GLOBAL_HEAP_SIZE
#define CONFIG_APP_STACK_SIZE 8192
#define CONFIG_APP_HEAP_SIZE  8192

static char global_heap_buf[CONFIG_HEAP_MEM_POOL_SIZE] = { 0 };

LOG_MODULE_REGISTER(main);

int
main(void)
{
    wasm_module_inst_t wasm_module_inst = NULL;
    wasm_module_t wasm_module           = NULL;
    uint8 *wasm_file_buf                = NULL;
    uint32 wasm_file_size               = 0;
    RuntimeInitArgs init_args;
    char error_buf[128];
    const char *exception;
    int start, end;
    
    start = k_uptime_get_32();
    
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
    init_args.mem_alloc_type                  = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf  = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    LOG_INF("global heap size: %d\n", sizeof(global_heap_buf));
#else
#error "memory allocation scheme is not defined."
#endif

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        LOG_ERR("Init runtime environment failed.\n");
        return 0;
    }


    /* load WASM byte buffer from byte buffer of include file */
    wasm_file_buf  = (uint8 *)wasi_nn_module_wasm;
    wasm_file_size = wasi_nn_module_wasm_len;
    LOG_INF("Wasm file size: %d\n", wasm_file_size);

    /* load WASM module */
    wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                    error_buf, sizeof(error_buf));
    if (!wasm_module) {
        LOG_ERR("Failed to load module: %s\n", error_buf);
        goto cleanup;
    }

    /* Set the WASI context */
#if WASM_ENABLE_LIBC_WASI != 0
// #define ADDRESS_POOL_SIZE 1
//     const char *addr_pool[ADDRESS_POOL_SIZE] = {
//         "192.0.2.10/24",
//     };
//     /* No dir list => No file system
//      * dir_cont = 0
//      * No mapped dir list => No file system
//      * map_dir_cont = 0
//      * No environment variables
//      * env_count = 0
//      * No command line arguments
//      * argv  0
//      */
//     wasm_runtime_set_wasi_args(wasm_module, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
//     wasm_runtime_set_wasi_addr_pool(wasm_module, addr_pool, ADDRESS_POOL_SIZE);
//     wasm_runtime_set_wasi_ns_lookup_pool(wasm_module, NULL, 0);
#endif

    /* instantiate the module */
    wasm_module_inst = wasm_runtime_instantiate(wasm_module, CONFIG_APP_STACK_SIZE,
                                    CONFIG_APP_HEAP_SIZE, error_buf, sizeof(error_buf));
    if (!wasm_module_inst) {
        LOG_ERR("Failed to instantiate module: %s\n", error_buf);
        goto unload;
    }

    /*
     * Lookup for usual function name
     *
     * TODO: remove later, it is strange to export only main. 
     */
    wasm_function_inst_t main_args_inst = NULL;
    wasm_function_inst_t start_inst     = NULL;
    wasm_function_inst_t main_inst      = NULL;

    start_inst      = wasm_runtime_lookup_function(wasm_module_inst, "_start");
    main_inst       = wasm_runtime_lookup_function(wasm_module_inst, "main"); 
    main_args_inst  = wasm_runtime_lookup_function(wasm_module_inst, "__main_argc_argv");

    if ((start_inst == NULL) && (main_inst == NULL) && (main_args_inst == NULL)) {
        LOG_ERR("Failed to lookup function main\n");
        return -1;
    }
    
    /* invoke the main function */
    wasm_application_execute_main(wasm_module_inst, 0, NULL);
    
    LOG_INF("main executed\n");

    if ((exception = wasm_runtime_get_exception(wasm_module_inst)))
        LOG_INF("%s\n", exception);

    int rc = wasm_runtime_get_wasi_exit_code(wasm_module_inst);
    LOG_INF("wasi exit code: %d\n", rc);

    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);

unload:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

cleanup:
    /* destroy runtime environment */
    wasm_runtime_destroy();

    end = k_uptime_get_32();

    LOG_INF("elapsed: %dms\n", (end - start));

    return 0;
}
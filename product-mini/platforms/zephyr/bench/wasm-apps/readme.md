# Benchmarking wasm applications on Zephyr
> 🛠️ **Work in progress:** The benchmarking environement and process is still in development and not mature enough.  

## Benchmarks setup
### Benchmark types
* **wasm performance:** We compare the performance of a wasm module with the performance of the same function implemented in C and compiled to run on Zephyr.
We plan to add more benchmarks in the future:
* **wasi-thread:** We compare the performance of a wasm module that uses threads with the performance of the same function implemented in C and compiled to run on Zephyr.
* **wasi-sockets:** We compare the performance of a wasm module that uses sockets with the performance of the same function implemented in C and compiled to run on Zephyr.
* **wasi-file-system:** We compare the performance of a wasm module that uses the file system with the performance of the same function implemented in C and compiled to run on Zephyr.

### Runtime configuration
* **wasm-performance:** The runtime configuration is the default one: 
    ```cmake
    set (WAMR_BUILD_INTERP 1)
    set (WAMR_BUILD_FAST_INTERP 0)
    set (WAMR_BUILD_AOT 1)
    set (WAMR_BUILD_LIBC_BUILTIN 1) 
    set (WAMR_BUILD_LIBC_WASI 0)
    set (WAMR_BUILD_LIB_PTHREAD 0)
    set (WAMR_BUILD_GLOBAL_HEAP_POOL 1)
    set (WAMR_BUILD_GLOBAL_HEAP_SIZE 131072) # 128 KB
    ``` 
    We removed the `WAMR_BUILD_LIBC_WASI` to prevent WASI overhead and use `WAMR_BUILD_LIBC_BUILTIN` to use the `printf` function provided by the runtime.
* **wasi-thread:**

    TODO
* **wasi-scoket and wasi-file-system:**
    ```cmake
    set (WAMR_BUILD_INTERP 1)
    set (WAMR_BUILD_FAST_INTERP 0)
    set (WAMR_BUILD_AOT 1)
    set (WAMR_BUILD_LIBC_BUILTIN 1) 
    set (WAMR_BUILD_LIBC_WASI 1)
    set (WAMR_BUILD_LIB_PTHREAD 0)
    set (WAMR_BUILD_GLOBAL_HEAP_POOL 1)
    set (WAMR_BUILD_GLOBAL_HEAP_SIZE 131072) # 128 KB
    ```
    More details on configuration can be found in this [readme](socket/readme.md) for socket.

### Time Measurement
We use the `k_cycle_get_64` function to measure the CPU ticks.

>📝 **Quote:**
>
>❝ The kernel presents a “cycle” count via the`k_cycle_get_32()` and `k_cycle_get_64()` APIs. The intent is that this counter represents the fastest cycle counter that the operating system is able to present to the user (for example, a CPU cycle counter) and that the read operation is very fast. The expectation is that very sensitive application code might use this in a polling manner to achieve maximal precision. The frequency of this counter is required to be steady over time, and is available from `sys_clock_hw_cycles_per_sec()` (which on almost all platforms is a runtime constant that evaluates to `CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC`). ❞
>
>_source: [zephyr kernel timing documentation](https://docs.zephyrproject.org/latest/kernel/services/timing/clocks.html)_

* **wasm module:** By leveraging WAMR capabilities to call Native APIs, we register a set of wrapper function to call the `k_cycle_get_64` function.
    ```c
    static uint64_t wasm_bench_start_time = 0;
    static uint64_t wasm_bench_end_time = 0;

    void wasm_bench_start(wasm_exec_env_t exec_env){
        wasm_bench_start_time =  k_cycle_get_64(); 
    }

    void wasm_bench_end(wasm_exec_env_t exec_env){
        wasm_bench_end_time =  k_cycle_get_64();
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
    ```
* **native code:** We also call a set of wrapper functions to measure the time taken by the function.
    ```c
    static uint64_t native_bench_start_time = 0;
    static uint64_t native_bench_end_time = 0;

    void native_bench_start(void){
        native_bench_start_time = k_cycle_get_64(); 
    }

    void native_bench_end(void){
        native_bench_end_time = k_cycle_get_64();
    }
    ```

### Compilation
We compile the wasm module and the native code standalone and then integrate them into the main application.
Both are compiled with `-O2` to optimize the code, `-O3` showed to enable hardware optimizations for native code.

For native code we have no access to the Zephyr build system, so we use an interface to make the code compile.
Then at the end of the compilation, we link the object file with the main application.

## Automated Benchmarks
It is possible to run the selected benchmarks semi-automatically.

1. Build the benchmarks with the build script:
    ```bash
    python3 build.py -s <suite>
    ```
    where `<suite>` is the benchmark suite to run. The available suites are:
    * `core`: Core benchmarks
    * `socket`: Socket benchmarks

2. Select benchmark to run in subfolder `wasm-apps`'s `CMakeLists.txt`:
    ```cmake
    set(BENCHMARK_SUITE_CORE 0)
        set(BENCHMARK_ACKERMANN  0)
        set(BENCHMARK_BASE64     0)
        set(BENCHMARK_FIB2       0)
        set(BENCHMARK_GIMLI      0)
        set(BENCHMARK_MATRIX     0)
        set(BENCHMARK_MEMMOVE    0)
        set(BENCHMARK_RANDOM     0)
        set(BENCHMARK_SIEVE      0)
        set(BENCHMARK_HEAPSORT   0)
        set(BENCHMARK_NESTEDLOOP 0)
        set(BENCHMARK_DUMMY      0)

    set(BENCHMARK_SUITE_SOCKET 1)
        set(BENCHMARK_SOCKET_CREATE       0)
        set(BENCHMARK_SOCKET_CONNECT      0)
        set(BENCHMARK_SOCKET_BIND         0)
        set(BENCHMARK_SOCKET_SENDTO       0)
        set(BENCHMARK_SOCKET_RECVFROM     1)
    ``` 
    It is also important to put the BENCMARK_SUITE to 1 to enable the suite.

3. Build the main application:
    ```bash
    ZEPHYR_BASE=~/zephyrproject/zephyr \
    WAMR_ROOT_DIR=~/wasm-micro-runtime \
    WASI_SDK_PATH=~/wasi-sdk-21.0 \
    WAMR_APP_FRAMEWORK_DIR=~/wamr-app-framework \
    west build . -b nucleo_h563zi -p always 
    ```

4. Flash the board:
    ```bash
    ZEPHYR_BASE=~/zephyrproject/zephyr west flash
    ```

5. Read the time taken by the wasm module and the native code from the serial output.

6. Clean the generated files with the build script:
    ```bash
    python3 build.py -s <suite> --clean
    ```

We will explore the possibility of running benchmarks in loop and automatically calculating the results in the future.

Also note that for `core` suite it is possible to run multiple benchmarks but not for `socket` suite due to important wasm file size.

## Old Workflow
1. Compile the wasm module with the provided compilation command.
2. Generate a header file with the wasm module content.
3. Implement the same function in C and compile it to run on Zephyr.
    1. Create a runtime and run the bench module.
    2. Clear the runtime after execution.
    3. Run the native code.  
4. Flash the board with the Zephyr application.
5. Write the time taken by the wasm module and the native code.

> 📄 **Notes:** The workflow process is performed manually and each measurement is done manually by changing the revelant values.

## Adding a new benchmark

* Add your code to one of the suite or create one.
    Ensure that main is called:
    ```c
    int benchmark_xxx() {
    ```
* Add the benchmark to the `build.py` file.
    1. In the `BENCHMARKS` dictionary.
    2. Optionally create a new benchmark suite.
* Add the benchmark to the `wasm-apps/CMakeLists.txt` file.
    1. Create a new declaration
        ```cmake
        set(BENCHMARK_XXX     1)
        ```
    2. set a new definition
        ```cmake
        set(BENCHMARK_DEFINITIONS
        "-DENABLE_BENCHMARK_ACKERMANN=${BENCHMARK_ACKERMANN}"
        ...
        "-DENABLE_BENCHMARK_XXX=${BENCHMARK_XXX}"
        PARENT_SCOPE
        )
        ```
    3. Add the benchmark to the list
        ```cmake
        add_benchmark_source(BENCHMARK_XXX xxx)
        ```
* Add the benchmark to the `src/config.h` file
    ```c
    #if ENABLE_BENCHMARK_XXX
        #define BENCHMARK_XXX 1
    #else
        #define BENCHMARK_XXX 0
    #endif
    ```
* Add the benchmark to the `src/benchmark.c` file
    1. add the extern definition
        ```c
        extern int benchmark_xxx();
        ``` 
    2. In the `benchmark_banner` function:
        ```c
        void benchmark_banner(void){
            printk("[INFO] Performing benchmarks:\n");
            printk("\t - ackermann :             %s\n", BENCHMARK_ACKERMANN ? "YES" : "NO");
            ...
            printk("\t - xxx :                   %s\n", BENCHMARK_XXX ? "YES" : "NO");
        }
        ```
    3. In the `call_bench_main` function:
        ```c
        void call_bench_main(void){
        #if BENCHMARK_ACKERMANN
            bench_ackermann();
            printk("[ackermann] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
            reset_times();
        #endif
        ...
        #if BENCHMARK_XXX
            benchmark_xxx();
            printk("[xxx] elapsed: %llu ticks\n", (native_bench_end_time - native_bench_start_time));
            reset_times();
        #endif
        }
        ```
    4. In the `initialize_benchmarks` function:
        ```c
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
        ...
        #if BENCHMARK_XXX
        #include "xxx.h"
            wasm_file_buf = (uint8_t *)wasm_xxx;
            wasm_file_size = sizeof(wasm_xxx);

            allocate_benchmark("bench_xxx", wasm_file_buf, wasm_file_size);
        #endif
        }
        ```

## TODO
* The benchmark interface is not that great a refactoring is needed.
* The benchmark results are not automatically generated.
* The benchmarks are not automatically run.
* Adding a new benchmark is a little painful and needs to be improved: code in python + C + cmake..
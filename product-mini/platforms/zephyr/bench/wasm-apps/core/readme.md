# Benchmark core

>📄 **Notes:** The result presented here are outdated, because we choose to change benchmark build. But they can still provide an insight of wasm / zephyr performance.

## Different cases
To run these benchmark we can distinguish four cases:

1. **Case 1:** Zephyr and wasm module are not optimized.
    * **`prj.conf`:** Ensure that `CONFIG_NO_OPTIMIZATIONS=y` is set.
    * **module compilation:** Use the provided compilation command.

2. **Case 2:** Zephyr is optimized and wasm module is not optimized.
    * **`prj.conf`:** Ensure that `CONFIG_NO_OPTIMIZATIONS=n` is set, or by default it is set to `n`.
    * **module compilation:**  Use the provided compilation command.

3. **Case 3:** Zephyr is not optimized and wasm module is optimized.
    * **`prj.conf`:** Ensure that `CONFIG_NO_OPTIMIZATIONS=y` is set.
    * **module compilation:**  Use the provided compilation command, but add `-O3`, `-flto` and `Wl,--lto-O3` flag to the command.

4. **Case 4:** Zephyr and wasm module are optimized.
    * **`prj.conf`:** Ensure that `CONFIG_NO_OPTIMIZATIONS=y` is set.
    * **module compilation:**  Use the provided compilation command, but add `-O3`, `-flto` and `Wl,--lto-O3` flag to the command.

## Ackermann benchmark
### Description
The Ackermann function is a classic example of a recursive function, which is a function that calls itself.

* **Nature:** The Ackermann function is a non-primitive recursive function that grows very rapidly. It's more complex than the Fibonacci sequence and can push the limits of a system's recursion stack.

* **Focus:** This benchmark is more about testing the limits of recursion depth and the efficiency of handling deeply recursive calls. It's less about arithmetic operations and more about recursion and stack management.

* **Usefulness:** It's used to test the system's capability to manage complex recursive calls and stack depth, which can be critical for certain types of applications.

### Compilation
The default compilation command for the Ackermann function is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/ackermann.wasm wasm-apps/ackermann.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```
* **--allow-undefined** To prevent error for undefined symbols `printf`, `bench_start` and `bench_end`. 
    * `printf` provided by the runtime `lib-builtin`.
    * `bench_start` and `bench_end` are provided by passing native symbols to the runtime in the `main` implementation.
* **-z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end** Are usual symbols when compiling a wasm module to be run on a restricted environment.
* **-nodefaultlibs** To prevent the default linking of the standard C library. Binary will be lighter and we will not call WASI functions (`printf` call `__wasi_fd_write`).

### Measurement
We measure the time taken to calculate the Ackermann function for:
* `M` = [1, 2, 3]
* `N` = [0, ..., 9] and for `M`=3 we stop when wasm overflows.

Then we calculate the sum of the CPU ticks for each case and compare the results.

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 19053,6 | 1424013,76 | 252,597572 |
| 2 | 1872,48 | 477242,84 | 254,8720627 |
| 3 | 19053,6 | 823645,16 | 43,22779737 |
| 4 | 5996,30| 242327,9 | 40,41286304 |

> 📄 **Notes:** The 3th and 4th case were actually performed on one more value of `n`. 


## Gimli benchmark
### Description
The Gimli function is a cryptographic function that is used in the Gimli permutation. The Gimli permutation is a cryptographic primitive that is used in the Gimli block cipher.

* **Nature:** The Gimli benchmark is based on the Gimli cryptographic hash function, which is designed for efficiency in a wide range of applications. It's a non-trivial example used to measure the performance of cryptographic operations, including hashing, encryption, and decryption processes.

* **Focus:** It primarily tests the computational efficiency and speed of executing cryptographic algorithms within a system. This includes assessing the performance of bitwise operations, data shuffling, and the handling of cryptographic primitives.

* **Usefulness:** It's especially valuable for evaluating the performance of cryptographic operations in a language or runtime environment. This benchmark is crucial for applications that require secure data handling, encryption, and decryption, providing insights into the suitability of the environment for cryptographic tasks.

### Compilation
The default compilation command for the Grimli benchmark is:
```bash	
 ~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/gimli.wasm wasm-apps/gimli.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We performed different measure by changing the number of iterations of the function.

The limit was fixed at 10000 iterations. If the code managed to get here we stopped the execution. Else we stopped where the overflow occured with a variation of ± 100 iterations.

* **case 1:** stopped at 900 with 10 measurements
* **case 2:** stopped at 2500 with 24 measurements
* **case 3:** reached 10000 with 27 measurements
* **case 4:** reached 10000 with 10 measurements

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 4512602,375 | 1098880751 | 243,5137555 |
| 2 | 3826534,083 | 1093497957 | 285,7672068 |
| 3 | 27330987,63 | 567701258,2 | 20,77134079 |
| 4 | 16836225,6 | 481321784,3 | 28,58846132 |


## Fibonacci benchmark
The Fibonacci benchmark is a widely recognized method for assessing computational performance across various programming paradigms and environments.

* **Nature:** The Fibonacci sequence is a classic example used to measure the performance of recursive function calls and, in some implementations, the efficiency of memoization or dynamic programming techniques.

* **Focus:** It primarily tests the ability of a system to handle simple arithmetic operations and recursion efficiently. In iterative implementations, it can also measure loop efficiency.

* **Usefulness:** It's particularly useful for evaluating the overhead of function calls and recursion in a language or runtime environment.

### Compilation
The default compilation command for the fibonacci benchmark is:
```bash	
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/fib2.wasm wasm-apps/fib2.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Fibonacci sequence for:
* `N` = [0, ..., 20] with `N` += 2

Then we calculate the sum of the CPU ticks for each case and compare the results.

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 976191 | 289713508 | 296,7795319 |
| 2 | 478923 | 104819515 | 218,8650681 |
| 3 | 976194 | 51498838 | 52,75471679 |
| 4 | 478923 | 17264308 | 36,04819146 |

## Heap sort benchmark
### Description
The Heap Sort algorithm is a comparison-based sorting algorithm that uses a binary heap data structure to sort elements. It's known for its efficiency and is often used in various applications that require sorting large datasets.

* **Nature:** The Heap Sort benchmark is designed to evaluate the performance of sorting algorithms and the efficiency of handling large datasets. It's a non-trivial example that tests the system's ability to manage complex data structures and sorting operations.

* **Focus:** It primarily focuses on the computational efficiency of sorting algorithms and the handling of data structures like binary heaps. This benchmark is crucial for assessing the performance of sorting operations in a language or runtime environment.

* **Usefulness:** It's valuable for evaluating the efficiency of sorting algorithms and the performance of handling large datasets. This benchmark provides insights into the suitability of the environment for sorting and managing data structures efficiently.

### Compilation
The default compilation command for the Heap Sort benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/heapsort.wasm wasm-apps/heapsort.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
>⚠️**Warning:** The measurement was aborted because we were unable to find reasonable values for the parameters `LENGTH` and `ITERATIONS`.


## Sieve of Eratosthenes benchmark
### Description
The Sieve of Eratosthenes is an ancient algorithm used to find all prime numbers up to a given limit. It's a simple yet effective method for identifying prime numbers and is often used in various mathematical and computational applications.

* **Nature:** The Sieve of Eratosthenes benchmark is designed to evaluate the performance of algorithms that identify prime numbers and the efficiency of handling large datasets. It's a non-trivial example that tests the system's ability to manage complex computational tasks.

* **Focus:** It primarily focuses on the computational efficiency of algorithms for identifying prime numbers and the handling of large datasets. This benchmark is crucial for assessing the performance of prime number identification operations in a language or runtime environment.

* **Usefulness:** It's valuable for evaluating the efficiency of prime number identification algorithms and the performance of handling large datasets. This benchmark provides insights into the suitability of the environment for computational tasks involving prime numbers.

### Compilation
The default compilation command for the Sieve of Eratosthenes benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/sieve.wasm wasm-apps/sieve.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Sieve of Eratosthenes for:
* `SIZE` = [0, 64, 128, ..., 2048] with `SIZE` *= 2
* `LENGTH` = [0, 200, ..., 1400] with `LENGTH` += 200

To not have too many measurements we fix a parameter when measuring the other. 
* When measuring  `SIZE` we set `LENGTH` = 100.
* When measuring `LENGTH` we set `SIZE` = 256.

If the code didn't reach upper limit we stopped where the overflow occured with a variation of ± 100 iterations.

* **case 1:** stopped at:
    * `LENGTH` = 400 
    * `SIZE` = 1024 

Then we calculate the sum of the CPU ticks for each case and compare the results.

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 25013547 | 5593169976 | 223,6056316 |
| 2 | 62302122 | 9993236192 | 160,3996119 |
| 3 | 129988714 | 7780726439 | 59,85693834 |
| 4 | 62301321 | 2711444743 | 43,52146471 |

## Matrix multiplication benchmark

### Description
Matrix multiplication is a fundamental operation in linear algebra and is used in various scientific, engineering, and computational applications. It's a computationally intensive task that involves multiplying two matrices to produce a third matrix.

* **Nature:** The Matrix Multiplication benchmark is designed to evaluate the performance of matrix multiplication algorithms and the efficiency of handling large datasets. It's a non-trivial example that tests the system's ability to manage complex computational tasks.

* **Focus:** It primarily focuses on the computational efficiency of matrix multiplication algorithms and the handling of large datasets. This benchmark is crucial for assessing the performance of matrix multiplication operations in a language or runtime environment.

* **Usefulness:** It's valuable for evaluating the efficiency of matrix multiplication algorithms and the performance of handling large datasets. This benchmark provides insights into the suitability of the environment for computational tasks involving matrix operations.

### Compilation
The default compilation command for the Matrix Multiplication benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/matrix.wasm wasm-apps/matrix.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Matrix Multiplication for:
* `SIZE` = [5, ..., 10] with `SIZE` += 1
* `ITERATIONS` = [1, 10, ..., 10000] with `ITERATIONS` *= 10

To not have too many measurements we fix a parameter when measuring the other. 
* When measuring  `SIZE` we set `ITERATIONS` = 1.
* When measuring `ITERATIONS` we set `SIZE` = 5.

> 📄 **Notes:**  `SIZE` = 5 is the minimum size possible due to the implementation.

* **case 1:** stopped at `LENGTH` = 1000.
* **case 2:** stopped at `LENGTH` = 1000. 

Then we calculate the sum of the CPU ticks for each case and compare the results.

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 6708747 | 1352493274 | 201,6014725 |
| 2 | 2049227 | 464944312 | 226,8876567 |
| 3 | 65954411 |860644036 | 13,0490747 |
| 4 | 20149978 | 262297949 | 13,01728215 |

## Memory move benchmark
### Description
The Memory Move benchmark is designed to evaluate the performance of memory copy operations and the efficiency of handling large datasets. It's a non-trivial example that tests the system's ability to manage memory operations efficiently.

* **Nature:** The Memory Move benchmark focuses on the computational efficiency of memory copy operations and the handling of large datasets. It's crucial for assessing the performance of memory operations in a language or runtime environment.

* **Focus:** It primarily focuses on the efficiency of memory copy operations and the performance of handling large datasets. This benchmark provides insights into the suitability of the environment for memory-intensive tasks.

* **Usefulness:** It's valuable for evaluating the efficiency of memory copy operations and the performance of handling large datasets. This benchmark is essential for applications that require efficient memory management and data handling.

### Compilation
The default compilation command for the Memory Move benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/memmove.wasm wasm-apps/memmove.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Memory Move for:
* `STR_SIZE` = [10, ..., 10000] with `STR_SIZE` *= 10
* `ITERATIONS` = [1, ..., 5] with `ITERATIONS` += 1

To not have too many measurements we fix a parameter when measuring the other.
* When measuring  `STR_SIZE` we set `ITERATIONS` = 1.
* When measuring `ITERATIONS` we set `STR_SIZE` = 10.

> 📄 **Notes:** We couldn't try `STR_SIZE` = 100000 because of the runtime is not configurated to have so much bytes allocated.
>```bash
>[00:00:00:000 - 20001790]: warning: allocate 100000 bytes memory failed
>```

Then we calculate the sum of the CPU ticks for each case and compare the results.

In this case we can't establish a table comparison between the native and wasm code.
* **Observation 1:** The more `STR_SIZE` grow the more wasm tend to be equal to the native code. For every case at `STR_SIZE` = 10000 wasm over native ration is around 1.
    * **case 1:** 1,229328734
    * **case 2:** 1,114320903
    * **case 3:** 1,089586661
    * **case 4:** 1,044094462
* **Observation 2:** The more `ITERATIONS` grow the more precise we are on the measurement, but we couldn't apply a lot of iterations to big values of `STR_SIZE` because of the timer overflow.

## Random generation benchmark
### Description
The Random Generation benchmark is designed to evaluate the performance of random number generation algorithms and the efficiency of handling random data. It's a non-trivial example that tests the system's ability to generate random numbers efficiently.

* **Nature:** The Random Generation benchmark focuses on the computational efficiency of random number generation algorithms and the handling of random data. It's crucial for assessing the performance of random number generation in a language or runtime environment.

* **Focus:** It primarily focuses on the efficiency of random number generation algorithms and the performance of handling random data. This benchmark provides insights into the suitability of the environment for random data generation tasks.

* **Usefulness:** It's valuable for evaluating the efficiency of random number generation algorithms and the performance of handling random data. This benchmark is essential for applications that require efficient random data generation and processing.

### Compilation
The default compilation command for the Random Generation benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/random.wasm wasm-apps/random.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Random Generation for:
* `ITERATIONS` = [1, ..., 1000000] with `ITERATIONS` *= 10

* **case 1:** stopped at `ITERATIONS` = 100000.
* **case 2:** stopped at `ITERATIONS` = 100000. 

Then we calculate the sum of the CPU ticks for each case and compare the results.

| case | native sum | wasm sum | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 98811443 | 1587387390 | 16,06481336 |
| 2 | 2451253 | 565693687 | 230,7773563 |
| 3 | 989338688 | 1133187702 | 1,14539916 |
| 4 | 24453582 | 460560896 | 18,83408721 |

> 📄 **Notes:** A better ratio always doesn't mean that it is a good solution. The case 4 will always be the fastest and case 1 the slowest. 


## Nested loop benchmark
### Description 
The Nested Loop benchmark is designed to evaluate the performance of nested loop operations and the efficiency of handling complex computational tasks. It's a non-trivial example that tests the system's ability to manage nested loop operations efficiently.

* **Nature:** The Nested Loop benchmark focuses on the computational efficiency of nested loop operations and the handling of complex computational tasks. It's crucial for assessing the performance of nested loop operations in a language or runtime environment.

* **Focus:** It primarily focuses on the efficiency of nested loop operations and the performance of handling complex computational tasks. This benchmark provides insights into the suitability of the environment for handling nested loop operations.

* **Usefulness:** It's valuable for evaluating the efficiency of nested loop operations and the performance of handling complex computational tasks. This benchmark is essential for applications that require efficient handling of nested loop operations.

### Compilation
The default compilation command for the Nested Loop benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/nestedloop.wasm wasm-apps/nestedloop.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Nested Loop for:
* `LENGTH` = [0, ..., 10] with `LENGTH` += 2

* **case 1:** stopped at `LENGTH` = 8.

Then we calculate the sum of the CPU ticks for each case and compare the results.

>⚠️**Warning:** The measurement showed that this case was not fair because the optimized wasm code seem to benefit from loop optimization. It is probably due to the fact that the wasm code is compiled with `-O3` and the native code is not.
>
>| case | native sum | wasm sum | wasm / native  |
>| :--: | ---------: | -------: | :------------: | 
>| 1 | 5203802 | 1231484483 | 236,6509108 |
>| 2 | 11338470 | 1673662365 | 147,6091893 |
>| 3 | 21096214 | 21761 | 0,001031512 |
>| 4 | 11339980 | 7260 | 0,000640213 |

## Base64 conversion benchmark
### Description 

The Base64 Conversion benchmark is designed to evaluate the performance of Base64 encoding and decoding operations and the efficiency of handling data conversion tasks. It's a non-trivial example that tests the system's ability to manage Base64 operations efficiently.

* **Nature:** The Base64 Conversion benchmark focuses on the computational efficiency of Base64 encoding and decoding operations and the handling of data conversion tasks. It's crucial for assessing the performance of Base64 operations in a language or runtime environment.

* **Focus:** It primarily focuses on the efficiency of Base64 encoding and decoding operations and the performance of handling data conversion tasks. This benchmark provides insights into the suitability of the environment for handling Base64 operations.

* **Usefulness:** It's valuable for evaluating the efficiency of Base64 encoding and decoding operations and the performance of handling data conversion tasks. This benchmark is essential for applications that require efficient handling of data conversion operations.

### Compilation
The default compilation command for the Base64 Conversion benchmark is:
```bash
~/wasi-sdk-21.0/bin/clang --sysroot=/home/user/wasi-sdk-21.0/share/wasi-sysroot -nodefaultlibs -o wasm-apps/base64.wasm wasm-apps/base64.c -z stack-size=8192 -Wl,--initial-memory=65536 -Wl,--export=__heap_base -Wl,--export=__data_end -Wl,--allow-undefined -Wl,--export=main
```

### Measurement
We measure the time taken to calculate the Nested Loop for:
* `LENGTH` = [1, ..., 10000] with `LENGTH` *= 10
* `ITERATIONS` = [1, ..., 1000] with `ITERATIONS` *= 10

To not have too many measurements we fix a parameter when measuring the other.
* When measuring  `LENGTH` we set `ITERATIONS` = 1.
* When measuring `ITERATIONS` we set `LENGTH` = 10.

</br>

* **case 4:** only one measurement at `LENGTH` = 10000.

Then we calculate the sum of the CPU ticks for each case and compare the results. We can't establish a table comparison between the native and wasm code.

We will only give the result of `ITERATIONS` = 1000 and set `LENGTH` = 10, which is the most precise measurememnt. 

| case | native | wasm | wasm / native  |
| :--: | ---------: | -------: | :------------: | 
| 1 | 4161615 | 1543660783 | 370,9283014 |
| 2 | 2000432 | 635035050 | 317,448956 |
| 3 | 4161615 | 257963108 | 61,98629811 |
| 4 | 2000432 | 104465357 | 52,22139868 |

> 📄 **Notes:** This is likely not a real case as `LENGTH` = 10 seem too small, but due to timer constraint up to 2^32 ticks we can't set a higher value of `LENGTH` because it will reduce the `ITERATIONS` number. The measurement still give valuable insight of native and wasm performances.


## Linux benchmarks results
In this case both the native and wasm code were compiled with `-O3` flag.
Stangely the native is compiled with `gcc` and the wasm with `clang`.
To measure time the utility tool `/usr/bin/time` was used and it has a centisecond precision.

| Benchmark     | Native | Wasm | Wasm / Native |
|---------------|-------:|-----:|--------------:|
| base64        | 0.34   | 4.35 | 12.79         |
| fib2          | 0.69   | 15.08| 21.88         |
| gimli         | 0.17   | 2.68 | 15.76         |
| heapsort      | 0.91   | 6.06 | 6.66          |
| matrix        | 1.87   | 39.63| 21.19         |
| memmove       | 4.25   | 96.78| 22.77         |
| nestedloop    | 6.58   | 27.02| 4.11          |
| nestedloop2   | 4.45   | 19.06| 4.28          |
| nestedloop3   | 4.52   | 25.97| 5.74          |
| random        | 0.62   | 0.54 | 0.87          |
| seqhash       | 3.86   | 53.16| 13.77         |
| sieve         | 0.33   | 5.49 | 16.64         |
| strchr        | 0.18   | 5.69 | 31.61         |
| switch2       | 0.36   | 5.72 | 15.89         |

## New results 
This are the result obtained for the new benchmark build. The native and wasm code is compiled with `-O2` flag. 

| Benchmark   | WASM Ticks   | Native Ticks | Ratio (WASM/Native) |
|-------------|--------------|--------------|---------------------|
| ackermann   | 1,243,487    | 79,224       | 15.69               |
| base64      | 100,954,724  | 4,151,442    | 24.32               |
| fib2        | 87,105       | 4,964        | 17.55               |
| gimli       | 878,389,941  | 100,253,993  | 8.76                |
| matrix      | 238,041,689  | 59,241,932   | 4.02                |
| memmove     | 1,686,095    | 328,912      | 5.13                |
| random      | 408,215,160  | 861,499,547  | 0.47                |
| sieve       | 514,482,423  | 25,020,167   | 20.56               |
| nestedloop  | 1,261        | 15,889,569   | 0.000079            |
| dummy       | 635          | 72           | 8.82                |

> 📄 **Notes:** The `nestedloop` benchmarks, is faster in wasm because from `-O1` the compiler seem to perform loop elimination.
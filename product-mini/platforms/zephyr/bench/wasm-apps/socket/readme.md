# Readme

Please note that for every socket benchmark it is neccessary to run one at time because of the footprint introduced by `wasi-libc` around 27KB of memory is used.

Network configuration is as follow:
```
Internet Protocol Version 4 (TCP/IPv4) Properties:
    IP Address:         192.0.2.10
    Subnet Mask:        255.255.255.0
    Default Gateway:    192.0.2.2
```

Server is run like that:
```bash
python3 -m http.server --bind 0.0.0.0
```

To run the benchmarks `sendto` and `recvfrom` we need to adjust memory usage:

1. in `proj.conf` increase stack size (comments won't work in real file):
    ```diff
    -CONFIG_MAIN_STACK_SIZE=32768 # 32KB
    +CONFIG_MAIN_STACK_SIZE=49152 # 48KB
    ```
2. in `CMAkeList.txt` decrease WAMR heap size:
    ```diff
    -set (WAMR_BUILD_GLOBAL_HEAP_SIZE 131072) # 128 KB
    +set (WAMR_BUILD_GLOBAL_HEAP_SIZE 102400) # 100KB
    ```
3. in `main.c` reduce app (= wasm module) stack and heap size:
    ```diff
    -define CONFIG_APP_STACK_SIZE 16384 // 16KB
    -define CONFIG_APP_HEAP_SIZE 16384 // 16KB
    +define CONFIG_APP_STACK_SIZE 8192 // 8KB
    +define CONFIG_APP_HEAP_SIZE 8192 // 8KB 
    ```

The results we observed were the followings:
| bench | wasm | native | r  |
| :------: | --------: | --------: | ----: |
| create   | 861611    | 220019    | 3.92  |
| connect  | 73680354  | 37120399  | 1.98  |
| bind     | 931049    | 27744     | 33.56 |
| sendto   | 1578943   | 577384    | 2.73  |
| recvfrom | 432538833 | 160295187 | 2.70  |

We can see that appart from the `bind` benchmark the wasm is between 2 and 4 slower than native. Also the `connect`, `sendto` and `recvfrom` are subject to the network latency and the server load.

The `bind` benchmark is the most interesting as it is the only one where the ratio is above 10. This is due to the fact that the `bind` is a local operation and the overhead is due to the `wasi-libc` and runtime implementation.
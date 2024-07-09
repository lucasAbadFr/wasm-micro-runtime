cmake_minimum_required(VERSION 3.10)
project(wasm-apps)

# option(ENABLE_WASM_OPT "Enable WASM optimizations" OFF)

set(WASI_SDK_PREFIX "/home/user/wasi-sdk-21.0")
set(WAMR_ROOT_DIR "/home/user/wasm-micro-runtime")
set(CMAKE_SYSROOT "${WASI_SDK_PREFIX}/share/wasi-sysroot")
set(ZEPHYR_ARM_EABI "/home/user/zephyr-sdk-0.16.5-1/arm-zephyr-eabi")

include(${WASI_SDK_PREFIX}/share/cmake/wasi-sdk.cmake)

# Set benchmark variables to enable/disable benchmarks
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

set(BENCHMARK_SUITE_SOCKET 0)
  set(BENCHMARK_SOCKET_CREATE       0)
  set(BENCHMARK_SOCKET_CONNECT      0)
  set(BENCHMARK_SOCKET_BIND         0)
  set(BENCHMARK_SOCKET_SENDTO       0)
  set(BENCHMARK_SOCKET_RECVFROM     0)

set(BENCHMARK_SUITE_FILESYSTEM 1)
  set(BENCHMARK_FS_FOPEN       0)
  set(BENCHMARK_FS_FWRITE      0)
  set(BENCHMARK_FS_FREAD       0)
  set(BENCHMARK_FS_FSEEK       0)
  set(BENCHMARK_FS_FCLOSE      0)
  set(BENCHMARK_FS_UNLINK      0)
  set(BENCHMARK_FS_MKDIR       0)
  set(BENCHMARK_FS_RENAME      0)
  set(BENCHMARK_FS_PERF        1)

# Set definitions based on benchmark variables for the parent scope
set(BENCHMARK_DEFINITIONS
  "-DENABLE_BENCHMARK_ACKERMANN=${BENCHMARK_ACKERMANN}"
  "-DENABLE_BENCHMARK_BASE64=${BENCHMARK_BASE64}"
  "-DENABLE_BENCHMARK_FIB2=${BENCHMARK_FIB2}"
  "-DENABLE_BENCHMARK_GIMLI=${BENCHMARK_GIMLI}"
  "-DENABLE_BENCHMARK_MATRIX=${BENCHMARK_MATRIX}"
  "-DENABLE_BENCHMARK_MEMMOVE=${BENCHMARK_MEMMOVE}"
  "-DENABLE_BENCHMARK_RANDOM=${BENCHMARK_RANDOM}"
  "-DENABLE_BENCHMARK_SIEVE=${BENCHMARK_SIEVE}"
  "-DENABLE_BENCHMARK_HEAPSORT=${BENCHMARK_HEAPSORT}"
  "-DENABLE_BENCHMARK_NESTEDLOOP=${BENCHMARK_NESTEDLOOP}"
  "-DENABLE_BENCHMARK_DUMMY=${BENCHMARK_DUMMY}"
  "-DENABLE_BENCHMARK_SOCKET_CREATE=${BENCHMARK_SOCKET_CREATE}"
  "-DENABLE_BENCHMARK_SOCKET_CONNECT=${BENCHMARK_SOCKET_CONNECT}"
  "-DENABLE_BENCHMARK_SOCKET_BIND=${BENCHMARK_SOCKET_BIND}"
  "-DENABLE_BENCHMARK_SOCKET_SENDTO=${BENCHMARK_SOCKET_SENDTO}"
  "-DENABLE_BENCHMARK_SOCKET_RECVFROM=${BENCHMARK_SOCKET_RECVFROM}"
  "-DENABLE_BENCHMARK_FS_FOPEN=${BENCHMARK_FS_FOPEN}"
  "-DENABLE_BENCHMARK_FS_FWRITE=${BENCHMARK_FS_FWRITE}"
  "-DENABLE_BENCHMARK_FS_FREAD=${BENCHMARK_FS_FREAD}"
  "-DENABLE_BENCHMARK_FS_FSEEK=${BENCHMARK_FS_FSEEK}"
  "-DENABLE_BENCHMARK_FS_FCLOSE=${BENCHMARK_FS_FCLOSE}"
  "-DENABLE_BENCHMARK_FS_UNLINK=${BENCHMARK_FS_UNLINK}"
  "-DENABLE_BENCHMARK_FS_MKDIR=${BENCHMARK_FS_MKDIR}"
  "-DENABLE_BENCHMARK_FS_RENAME=${BENCHMARK_FS_RENAME}"
  "-DENABLE_BENCHMARK_FS_PERF=${BENCHMARK_FS_PERF}"
  PARENT_SCOPE
)

# Initialize an empty list to hold enabled benchmark source files
set(BENCHMARK_LIST "")

# Function to add benchmark source if the benchmark is enabled
function(add_benchmark_source BENCHMARK_VAR BENCHMARK_NAME)
  if(${BENCHMARK_VAR})
    list(APPEND BENCHMARK_LIST ${BENCHMARK_NAME})
    set(BENCHMARK_LIST ${BENCHMARK_LIST} PARENT_SCOPE) # Update the list in the parent scope
  endif()
endfunction()

# Add source files based on benchmark variables
add_benchmark_source(BENCHMARK_ACKERMANN ackermann)
add_benchmark_source(BENCHMARK_BASE64 base64)
add_benchmark_source(BENCHMARK_FIB2 fib2)
add_benchmark_source(BENCHMARK_GIMLI gimli)
add_benchmark_source(BENCHMARK_MATRIX matrix)
add_benchmark_source(BENCHMARK_MEMMOVE memmove)
add_benchmark_source(BENCHMARK_RANDOM random)
add_benchmark_source(BENCHMARK_SIEVE sieve)
add_benchmark_source(BENCHMARK_HEAPSORT heapsort)
add_benchmark_source(BENCHMARK_NESTEDLOOP nestedloop)
add_benchmark_source(BENCHMARK_DUMMY dummy)
add_benchmark_source(BENCHMARK_SOCKET_CREATE socket_create)
add_benchmark_source(BENCHMARK_SOCKET_CONNECT socket_connect)
add_benchmark_source(BENCHMARK_SOCKET_BIND socket_bind)
add_benchmark_source(BENCHMARK_SOCKET_SENDTO socket_sendto)
add_benchmark_source(BENCHMARK_SOCKET_RECVFROM socket_recvfrom)
add_benchmark_source(BENCHMARK_FS_FOPEN fs_fopen)
add_benchmark_source(BENCHMARK_FS_FWRITE fs_fwrite)
add_benchmark_source(BENCHMARK_FS_FREAD fs_fread)
add_benchmark_source(BENCHMARK_FS_FSEEK fs_fseek)
add_benchmark_source(BENCHMARK_FS_FCLOSE fs_fclose)
add_benchmark_source(BENCHMARK_FS_UNLINK fs_unlink)
add_benchmark_source(BENCHMARK_FS_MKDIR fs_mkdir)
add_benchmark_source(BENCHMARK_FS_RENAME fs_rename)
add_benchmark_source(BENCHMARK_FS_PERF fs_perf)

# Collect all benchmark object files
set(BENCH_NATIVE_OBJECT "" CACHE INTERNAL "List of benchmark object files" )
foreach(BENCHMARK ${BENCHMARK_LIST})
  if(${BENCHMARK_SUITE_CORE})
    list(APPEND BENCH_NATIVE_OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/core/native/${BENCHMARK}.o)
  elseif(${BENCHMARK_SUITE_SOCKET})
    list(APPEND BENCH_NATIVE_OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/socket/native/${BENCHMARK}.o)
  elseif(${BENCHMARK_SUITE_FILESYSTEM})
    list(APPEND BENCH_NATIVE_OBJECT ${CMAKE_CURRENT_SOURCE_DIR}/filesystem/native/${BENCHMARK}.o)
  endif()
endforeach()

# Set the benchmark object files to be used in the parent scope
set(BENCH_NATIVE_OBJECT ${BENCH_NATIVE_OBJECT} PARENT_SCOPE)

# Compilation:
# Now, BENCHMARK_LIST contains all enabled benchmark source files
# We can use this list to build the benchmarks
# foreach(BENCHMARK ${BENCHMARK_LIST})
#   add_custom_target(${BENCHMARK} ALL
#     COMMENT
#       "Building ${BENCHMARK}.wasm & ${BENCHMARK}.o .."
#     COMMAND
#       ${WASI_SDK_PREFIX}/bin/clang
#         --sysroot=${CMAKE_SYSROOT} -nostdlib -O2
#         -z stack-size=8192 -Wl,--initial-memory=65536
#         -Wl,--allow-undefined,--no-entry
#         -Wl,--export=__heap_base,--export=__data_end
#         -Wl,--export=bench_${BENCHMARK}
#         -o ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.wasm
#         ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.c
#     COMMAND
#       ${ZEPHYR_ARM_EABI}/bin/arm-zephyr-eabi-gcc 
#         -O2
#         -mcpu=cortex-m33+nodsp+nofp -march=armv8-m.main+nodsp+nofp -mthumb -mabi=aapcs
#         -o ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.o 
#         -c ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.c
#     DEPENDS
#       ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.c
#     BYPRODUCTS
#       ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.wasm
#       ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK}.o
#     USES_TERMINAL
#   )
# endforeach()
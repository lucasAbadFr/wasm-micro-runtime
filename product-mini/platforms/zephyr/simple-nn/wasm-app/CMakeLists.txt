cmake_minimum_required(VERSION 3.8.2)
project(wasi_nn_module C CXX)

# ---------------------------------------------------------
# Configure WASI SDK toolchain
# ---------------------------------------------------------
# You can override this from the command line:
#   cmake -DWASI_SDK_PATH=/opt/wasi-sdk ..
if(NOT DEFINED WASI_SDK_PATH)
    message(FATAL_ERROR "Please define WASI_SDK_PATH, e.g. -WASI_SDK_PATH=/opt/wasi-sdk")
endif()



set(WASI_SYSROOT       "${WASI_SDK_PATH}/share/wasi-sysroot")
set(CMAKE_SYSTEM_NAME  WASI)
set(CMAKE_C_COMPILER   "${WASI_SDK_PATH}/bin/clang")
set(CMAKE_CXX_COMPILER "${WASI_SDK_PATH}/bin/clang++")
set(CMAKE_AR           "${WASI_SDK_PATH}/bin/ar")
set(CMAKE_RANLIB       "${WASI_SDK_PATH}/bin/ranlib")

# ---------------------------------------------------------
# Project files
# ---------------------------------------------------------
set(SRC
    main.c
)

# ---------------------------------------------------------
# Include directories
# ---------------------------------------------------------
# Add path to wasi-nn headers (adjust if needed)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../core/iwasm/libraries/wasi-nn/include/
)

# ---------------------------------------------------------
# Compiler flags
# ---------------------------------------------------------
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    --target=wasm32-wasi \
    --sysroot=${WASI_SYSROOT} \
    -Oz \
    -Wall -Wextra -Wno-unused-parameter\
")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -Wl,--initial-memory=131072 \
    -Wl,--export=__heap_base \
    -Wl,--export=__data_end \
    -Wl,--allow-undefined \
")

# ---------------------------------------------------------
# Target
# ---------------------------------------------------------
add_executable(wasi_nn_module.wasm ${SRC})
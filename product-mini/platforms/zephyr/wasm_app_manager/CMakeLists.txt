# # Copyright (c) 2021 Intel Corporation
# # SPDX-Licensedentifier: Apache-2.0

cmake_minimum_required(VERSION 3.8.2)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(wasm_app_manager)

set (WAMR_BUILD_PLATFORM "zephyr")

if (NOT DEFINED WAMR_BUILD_TARGET)
  set (WAMR_BUILD_TARGET "X86_32")
endif ()

if (NOT DEFINED WAMR_BUILD_INTERP)
  # Enable Interpreter by default
  set (WAMR_BUILD_INTERP 1)
endif ()

if (NOT DEFINED WAMR_BUILD_AOT)
  # Enable AOT by default.
  set (WAMR_BUILD_AOT 1)
endif ()

if (NOT DEFINED WAMR_BUILD_LIBC_BUILTIN)
  # Enable libc builtin support by default
  set (WAMR_BUILD_LIBC_BUILTIN 1)
endif ()

if (NOT DEFINED WAMR_BUILD_LIBC_WASI)
  # Disable libc wasi support by default
  set (WAMR_BUILD_LIBC_WASI 0)
endif ()

if (WAMR_BUILD_TARGET STREQUAL "RISCV64_LP64" OR WAMR_BUILD_TARGET STREQUAL "RISCV32_ILP32")
  set (WAMR_BUILD_FAST_INTERP 1)
endif ()

# Override the global heap usage
if (NOT DEFINED WAMR_BUILD_GLOBAL_HEAP_POOL)
  set (WAMR_BUILD_GLOBAL_HEAP_POOL 1)
endif ()

# Override the global heap size for small devices
if (NOT DEFINED WAMR_BUILD_GLOBAL_HEAP_SIZE)
  set (WAMR_BUILD_GLOBAL_HEAP_SIZE 131072) # 128 KB
endif ()

if (NOT DEFINED WAMR_BUILD_APP_FRAMEWORK)
  set (WAMR_BUILD_APP_FRAMEWORK 0)
endif()
# WAMR Configuration:

# Environment variables check: 

# Check if WASI_SDK_PATH and is set 
if(NOT $ENV{WASI_SDK_PATH} STREQUAL "")
  set(WASI_SDK_PATH $ENV{WASI_SDK_PATH})
else()
  find_program(WASM_C_COMPILER clang /opt/wasi-sdk/bin NO_DEFAULT_PATH)
  if(NOT WASM_C_COMPILER)
    message(FATAL_ERROR "'wasi-sdk' not found, please ensure wasi-sdk is installed.\
                         You can download and install it from\
                         https://github.com/WebAssembly/wasi-sdk/releases")
  else()
    set(WASI_SDK_PATH /opt/wasi-sdk)
  endif()
endif()

message("wasi-sdk was found at ${WASI_SDK_PATH}")

# Check if WAMR_ROOT_DIR and is set 
if(DEFINED ENV{WAMR_ROOT_DIR})
  set(WAMR_ROOT_DIR $ENV{WAMR_ROOT_DIR})
else()
  message(FATAL_ERROR "'WAMR_ROOT_DIR' need to be specified")
endif()

message("wamr was found at ${WAMR_ROOT_DIR}")

# Check if WAMR_APP_FRAMEWORK_DIR is set
if (DEFINED ENV{WAMR_APP_FRAMEWORK_DIR})
  set(WAMR_APP_FRAMEWORK_DIR $ENV{WAMR_APP_FRAMEWORK_DIR})
else()
  message(FATAL_ERROR "'wamr-app-framework' not found, please ensure they are installed.\
                       You can download and install them from\
                       https://github.com/bytecodealliance/wamr-app-framework")
endif()

message("wamr-app-framework was found at ${WAMR_APP_FRAMEWORK_DIR}")

# set the WAMR_SDK_DIR with the path specified in the environment variable
set(WAMR_SDK_DIR
    ${WAMR_APP_FRAMEWORK_DIR}/wamr-sdk
)

# set the WAMR_LIBC_BUILTIN_DIR with the path specified in the environment variable
set(WAMR_LIBC_BUILTIN_DIR
    ${WAMR_SDK_DIR}/wamr-sdk/app/libc-builtin-sysroot
)

# set the WAMR_SDK_PACKAGE_OUT_DIR
set(WAMR_SDK_PACKAGE_OUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/wamr-sdk/app-sdk/wamr-app-framework
)

zephyr_compile_definitions_ifdef(WAMR_BUILD_APP_FRAMEWORK
WASM_ENABLE_APP_FRAMEWORK=1
WASM_ENABLE_BASE_LIB=1
attr_container_printf=os_printf
)

zephyr_include_directories_ifdef(WAMR_BUILD_APP_FRAMEWORK
${WAMR_APP_FRAMEWORK_DIR}/app-framework/app-native-shared
${WAMR_APP_FRAMEWORK_DIR}/app-framework/base/native
${WAMR_ROOT_DIR}/core/shared/coap/er-coap
${WAMR_ROOT_DIR}/core/shared/coap/extension
#${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-mgr-shared
)

zephyr_library_sources_ifdef(WAMR_BUILD_APP_FRAMEWORK
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/app_manager.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/app_manager_host.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/ble_msg.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/event.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/message.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/module_utils.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/module_wasm_app.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/module_wasm_lib.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/resource_reg.c
${WAMR_APP_FRAMEWORK_DIR}/app-mgr/app-manager/watchdog.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/app_ext_lib_export.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/app-native-shared/attr_container.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/app-native-shared/restful_utils.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/base/native/base_lib_export.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/base/native/request_response.c
${WAMR_APP_FRAMEWORK_DIR}/app-framework/base/native/timer_wrapper.c
)

add_dependencies(app host-tool wamr-sdk)

# temp 
set(ZEPHYR_BASE $ENV{ZEPHYR_BASE})
include (${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)
# ~temp
include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)

#target_link_libraries(app PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/app/libapp.a)
target_sources(app PRIVATE ${WAMR_RUNTIME_LIB_SOURCE} src/main.c)

include(ExternalProject)

ExternalProject_Add(wamr-sdk
  SOURCE_DIR
    ${WAMR_SDK_DIR}/app
  BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/wamr-sdk
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} ${WAMR_SDK_DIR}/app
      -DWAMR_BUILD_SDK_PROFILE=simple
      -DCONFIG_PATH=${CMAKE_CURRENT_SOURCE_DIR}/wamr_sdk_config.cmake
      -DCMAKE_TOOLCHAIN_FILE=${WAMR_SDK_DIR}/app/wamr_toolchain.cmake
      -DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/wamr-sdk
      -DWASI_SDK_DIR=${WASI_SDK_PATH}
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build .
  INSTALL_COMMAND
    ""
  BUILD_BYPRODUCTS
    ${WAMR_SDK_PACKAGE_OUT_DIR}/include/wasm_app.h
    ${WAMR_SDK_PACKAGE_OUT_DIR}/lib/libapp_framework.a
  USES_TERMINAL
)

set(WASM_APPS
    timer
    request_handler
    request_sender
    event_publisher
    event_subscriber
)

foreach(WASM_APP ${WASM_APPS})
  add_custom_target(${WASM_APP} ALL
    COMMENT
      "Building ${WASM_APP}.wasm .."
    COMMAND
      ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wasm-apps
    COMMAND
      ${WASI_SDK_PATH}/bin/clang -O3
        -I${WAMR_LIBC_BUILTIN_DIR}/include
        -I${WAMR_SDK_PACKAGE_OUT_DIR}/include
        -L${WAMR_SDK_PACKAGE_OUT_DIR}/lib
        -lapp_framework
        -z stack-size=8192 -Wl,--initial-memory=65536
        -Wl,--no-entry -nostdlib -Wl,--allow-undefined
        -Wl,--export=__heap_base,--export=__data_end
        -Wl,--export=on_init -Wl,--export=on_destroy
        -Wl,--export=on_request -Wl,--export=on_response
        -Wl,--export=on_sensor_event -Wl,--export=on_timer_callback
        -Wl,--export=on_connection_data
        -o ${CMAKE_CURRENT_BINARY_DIR}/wasm-apps/${WASM_APP}.wasm
        ${CMAKE_CURRENT_SOURCE_DIR}/wasm-apps/${WASM_APP}.c
    DEPENDS
      wamr-sdk
      ${CMAKE_CURRENT_SOURCE_DIR}/wasm-apps/${WASM_APP}.c
    BYPRODUCTS
      ${CMAKE_CURRENT_BINARY_DIR}/wasm-apps/${WASM_APP}.wasm
    USES_TERMINAL
  )
endforeach()

ExternalProject_Add(host-tool
  SOURCE_DIR
    ${WAMR_APP_FRAMEWORK_DIR}/test-tools/host-tool
  BINARY_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/host-tool-build
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} ${WAMR_APP_FRAMEWORK_DIR}/test-tools/host-tool
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build .
  INSTALL_COMMAND
    ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/host-tool-build/host_tool
      ${CMAKE_CURRENT_BINARY_DIR}/host_tool
  BUILD_BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/host_tool
  USES_TERMINAL
)

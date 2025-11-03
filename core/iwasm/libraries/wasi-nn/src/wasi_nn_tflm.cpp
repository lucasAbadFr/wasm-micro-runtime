/*
 * placeholder - add some includes 
 */

 /*
  * FIXME: Temporary workaround.
  * Zephyr’s <zephyr/sys/cbprintf_cxx.h> causes duplicate declarations when
  * included through bh_platform.h in C++ files.
  * This macro prevents its inclusion to allow building the Zephyr backend.
  */
#define ZEPHYR_INCLUDE_SYS_CBPRINTF_CXX_H_
#include "bh_platform.h"
#undef ZEPHYR_INCLUDE_SYS_CBPRINTF_CXX_H_

#include "wasi_nn_backend.h"
#include "wasm_export.h"

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>

/* 
 * Compilation Guard - Don't try to compile with legacy and unsupported
 */
#if 1 == WAMR_BUILD_WASI_NN
#error The TensorFlow Lite Micro (Zephyr) backend does not support legacy "wasi_nn" ABI. Please use WASM_ENABLE_WASI_EPHEMERAL_NN.
#endif

#if 1 == WASM_ENABLE_WASI_NN_GPU
#error The TensorFlow Lite Micro (Zephyr) backend does not support GPU. Please disable WASM_ENABLE_WASI_NN_GPU.
#endif

#if 1 == WASM_ENABLE_WASI_NN_EXTERNAL_DELEGATE
#error The TensorFlow Lite Micro (Zephyr) backend does not support delegation (NPU, TPU, DSP, etc...). Please disable WASM_ENABLE_WASI_NN_EXTERNAL_DELEGATE.
#endif


/* Maximum number of graphs per WASM instance */
#define WASM_INST_MAX_GRAPHS 1
/* Maximum number of graph execution context per WASM instance*/
#define WASM_INST_MAX_GRAPH_EXEC_CTX 1


/* User configs */
#ifndef TFLM_TENSOR_ARENA_SIZE 
#define TFLM_TENSOR_ARENA_SIZE 10 * 1024 // 10 KB
#endif


typedef struct {
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    tflite::MicroMutableOpResolver<10> resolver;
    uint8_t tensor_arena[TFLM_TENSOR_ARENA_SIZE];
} TFLMGraph;

typedef struct {
    uint32_t current_models;
    TFLMGraph graphs[WASM_INST_MAX_GRAPHS];
    korp_mutex g_lock;
} TFLMContext;

/*
 * TensorFlow Lite Micro is not officially supported by the WASI-NN proposal.
 * However, since it reimplements a subset of TensorFlow Lite’s functionality
 * tailored for microcontrollers, its integration within the same abstraction
 * layer should be feasible.
 *
 * This file provides a proof-of-concept integration of TFLM into WAMR’s
 * WASI-NN layer for Zephyr. For now, all API functions are placeholders
 * returning success.
 */

/*
 * WASI NN - API
 */

// Load a `graph` from an opaque sequence of bytes to use for inference.
__attribute__((visibility("default"))) wasi_nn_error
load(void *tflite_ctx, graph_builder_array *builder, graph_encoding encoding,
     execution_target target, graph *g)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
load_by_name(void *tflite_ctx, const char *filename, uint32_t filename_len,
             graph *g)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
init_execution_context(void *tflite_ctx, graph g, graph_execution_context *ctx)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
set_input(void *tflite_ctx, graph_execution_context ctx, uint32_t index,
          tensor *input_tensor)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
compute(void *tflite_ctx, graph_execution_context ctx)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
get_output(void *tflite_ctx, graph_execution_context ctx, uint32_t index,
           tensor_data *output_tensor, uint32_t *output_tensor_size)
{
    return invalid_argument;
}


/*
 * Backend - API
 */
__attribute__((visibility("default"))) wasi_nn_error
init_backend(void **tflite_ctx)
{
    return invalid_argument;
}


__attribute__((visibility("default"))) wasi_nn_error
deinit_backend(void *tflite_ctx)
{
    return invalid_argument;
}
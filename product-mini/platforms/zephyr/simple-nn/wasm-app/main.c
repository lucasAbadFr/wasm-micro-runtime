#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "wasi_ephemeral_nn.h"
#include "wasi_nn_types.h"

#include "model.h"

/*
 * The types defined inside the `wasi_nn_types` header don't really map to the
 * `wasi-nn.wit` interface definition.  
 *
 *     | error code            | header | wit |
 *     +-----------------------+--------+-----+
 *     | success               |   X    |     |
 *     | invalid argument      |   X    |  X  |
 *     | invalid encoding      |   X    |  X  |
 *     | timeout               |        |  X  |
 *     | missing memory        |   X    |     |
 *     | runtime error         |   X    |  X  |
 *     | busy                  |   X    |     |
 *     | unsupported operation |   X    |  X  |
 *     | too large             |   X    |  X  |
 *     | not found             |   X    |  X  |
 *     | security              |        |  X  |
 *     | unknown               |        |  X  |
 */
static const char *
wasi_nn_strerr(wasi_ephemeral_nn_error err)
{
    switch(err){
        case wasi_ephemeral_nn_error_invalid_argument: 
            return "caller module passed an invalid argument";
        case wasi_ephemeral_nn_error_invalid_encoding:
            return "invalid graph encoding";
        case wasi_ephemeral_nn_error_missing_memory:
            return "runtime or backend missing memory";
        case wasi_ephemeral_nn_error_busy:
            return "operation timeout or backend is busy";
        case wasi_ephemeral_nn_error_runtime_error:
            return "runtime error";
        case wasi_ephemeral_nn_error_unsupported_operation:
            return "unsupported operation";
        case wasi_ephemeral_nn_error_too_large:
            return "graph too large";
        case wasi_ephemeral_nn_error_not_found:
            return "graph not found";        
        default:
            /* We don't care about WasmEdge error code - they are not in WASI-NN */
            return "unknown/undefined error (out-of-range code)";
    }
}   

int
main(int argc, char **argv)
{
    printf("[wasm-mod]: WASI-NN (TFLM backend) - wasm module\n");

    wasi_ephemeral_nn_graph_builder_array wasm_g_array;
    wasi_ephemeral_nn_graph_encoding wasm_g_encoding;
    wasi_ephemeral_nn_execution_target bck_exec_env;
    wasi_ephemeral_nn_graph wasm_g;
    wasi_ephemeral_nn_graph_execution_context wasm_g_ctx;

    wasm_g_array.size        = 1;
    wasm_g_array.buf[0].size = g_model_len;

    memcpy(wasm_g_array.buf[0].buf, g_model, wasm_g_array.buf[0].size);

    wasm_g_encoding = wasi_ephemeral_nn_encoding_tensorflowlite;
    bck_exec_env    = wasi_ephemeral_nn_target_cpu;

    /* 
     * Loading the model
     *
     * We don't use the `load_by_name` variant because it is intended to work with files
     * meanwhile this module is not intended to use a File System. Here the model is 
     * loaded staticaly and not dynamically. 
     */
    wasi_ephemeral_nn_error res = wasi_ephemeral_nn_load(wasm_g_array.buf, (uint32_t)g_model_len, wasm_g_encoding, bck_exec_env, &wasm_g);
    if (0 != res){
        printf("[wasm-mod]<ERROR>: %d - %s\n",res, wasi_nn_strerr(res));
        return -1;
    }

    /* 
     * Initializing the graph execution context
     */
    res = wasi_ephemeral_nn_init_execution_context(wasm_g, &wasm_g_ctx);
    if (0 != res){
        printf("[wasm-mod]<ERROR>: %d - %s\n",res, wasi_nn_strerr(res));
        return -1;
    }

    wasi_ephemeral_nn_tensor wasm_t_in  = {0};
    int8_t input = 32;
    /* 
     * The tensor characteritics are the following:
     *
     *    - The tensor dimension's is a simple scalar value [1].
     *    - The tensor type is `u8` but it is in fact a `float` model quantized
     *      to `int8`.
     *    - The input is arbitrarily set to 32. 
     */

    wasm_t_in.dimensions.size   = 1;
    wasm_t_in.dimensions.buf[0] = 1;
    wasm_t_in.type              = wasi_ephemeral_nn_type_u8; 
    wasm_t_in.data.buf          = &input; /* -Wpointer-sign */
    wasm_t_in.data.size         = sizeof(input);    

    /*
     * Setting the model inputs. 
     */ 
    res = wasi_ephemeral_nn_set_input(wasm_g_ctx, 0, &wasm_t_in);
    if (0 != res){
        printf("[wasm-mod]<ERROR>: %d - %s\n",res, wasi_nn_strerr(res));
        return -1;
    }
    printf("[wasm-mod]<INFO>: Model input set: %i\n", input);

    /*
     * Computing the output.
     */
    res = wasi_ephemeral_nn_compute(wasm_g_ctx);
    if (0 != res){
        printf("[wasm-mod]<ERROR>: %d - %s\n",res, wasi_nn_strerr(res));
        return -1;
    }
    printf("[wasm-mod]<INFO>: Computing...\n");

    int8_t output = 0;
    uint32_t size = 0;

    /*
     * Getting the output.
     * 
     * The output is a `uint8_t`(-Wpointer-sign) but we don't care for now.
     */ 
    res = wasi_ephemeral_nn_get_output(wasm_g_ctx, 0, &output, sizeof(output), &size);
    if (0 != res){
        printf("[wasm-mod]<ERROR>: %d - %s\n",res, wasi_nn_strerr(res));
        return -1;
    }
    printf("[wasm-mod]<INFO>: Model Output %i\n", output);

    return 0;
}
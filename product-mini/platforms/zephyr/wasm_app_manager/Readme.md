# WAMR application manager sample.
This sample was ported to Zephyr using the WAMR application manager. It is a simple application that demonstrates how to use the WAMR application manager to load and run WebAssembly applications.

## Building the Sample

### Environment Variables
This sample was adapted from the WAMR application manager sample. To build the sample, some environment variables must be set. The following environment variables are required:
- `ZEPHYR_BASE`: The path to the Zephyr base directory.
- `WASI_SDK_PATH`: The path to the WASI SDK.
- `WAMR_ROOT_DIR`: The path to the WAMR directory.
- `WAMR_APP_FRAMEWORK_DIR`: The path to the WAMR application framework directory.

### Flags
The following flags need to be set to build the sample:
- `-DWAMR_BUILD_APP_FRAMEWORK=1`: This flag is used to build the WAMR application framework code.

### Building
To build the sample, run the following command:
```shell
ZEPHYR_BASE=path/to/zephyr \
WASI_SDK_PATH=path/to/wasi-sdk \
WAMR_ROOT_DIR=path/to/wams-micro-runtime \
WAMR_APP_FRAMEWORK_DIR=path/to/wamr-app-framework \
west build . -b <target-board> -p always -- -DWAMR_BUILD_TARGET=<target-arch>-DWAMR_BUILD_APP_FRAMEWORK=1
```

## About the app manager
You can read the [app manager documentation](README.rst) for more information.
import os
import subprocess

# Path const definition
CWD = os.getcwd()
SRC_DIR = f"{CWD}/src"
WASM_APPS_DIR = f"{CWD}/wasm-apps"
HOME = "/home/user"

ZEPHYR_BASE = f"{HOME}/zephyrproject/zephyr" 
WAMR_ROOT_DIR = f"{HOME}/wasm-micro-runtime"
WASI_SDK_PATH = f"{HOME}/wasi-sdk-21.0"
WAMR_APP_FRAMEWORK_DIR = f"{HOME}/wamr-app-framework"

# Benchmark dict
BENCHMARKS = {
    "core": [
        "ackermann",
        "base64",
        "fib2",
        "gimli",
        "heapsort",
        "matrix",
        "memmove",
        "random",
        "sieve",
    ],
    "thread":[],
    "socket":[],
    "fileSystem":[],
} 

# Choisir un test case
# Build
# Flash
# relevé des données. (lecture de la liaison série)
# Répèter l'opération N fois
# Traiter les données (moyenne, etc )

# subprocess.run( [f"ZEPHYR_BASE={ZEPHYR_BASE}", f"WAMR_ROOT_DIR={WAMR_ROOT_DIR}", 
#          f"WASI_SDK_PATH={WASI_SDK_PATH}", f"WAMR_APP_FRAMEWORK_DIR={WAMR_APP_FRAMEWORK_DIR}",
#          "west", "build", ".", "-b", "nucleo_h743zi", "-p", "always"], check=True)


def build_wasm_benchmark(benchmark_name):
    print(f"Building {benchmark_name}.wasm")
    CLANG_PATH = f"{WASI_SDK_PATH}/bin/clang"
    WASI_SYSROOT = f"{WASI_SDK_PATH}/share/wasi-sysroot"
    COMMAND = [f"{CLANG_PATH}",
               f"--sysroot={WASI_SYSROOT}",
               "-nostdlib",
               "-O3",
               "-z",
               "stack-size=8192",
               "-Wl,--initial-memory=65536",
               "-Wl,--allow-undefined,--no-entry",
               "-Wl,--export=__heap_base,--export=__data_end",
               f"-Wl,--export=bench_{benchmark_name}",
               "-o",
               f"{WASM_APPS_DIR}/{benchmark_name}.wasm",
               f"{WASM_APPS_DIR}/{benchmark_name}.c"]
    try:
        subprocess.run(COMMAND, check=True)
    except  subprocess.CalledProcessError as e:
        print(f"{e}")

def build_native_benchmark(benchmark_name):
    print(f"Building {benchmark_name}.o")
    ZEPHYR_ARM_EABI = f"/home/user/zephyr-sdk-0.16.5-1/arm-zephyr-eabi"
    COMMAND = [f"{ZEPHYR_ARM_EABI}/bin/arm-zephyr-eabi-gcc",
               "-O3",
               "-mcpu=cortex-m33", "-mthumb", "-mabi=aapcs", "-mfp16-format=ieee", "-mtp=soft",
               "-c",
               "-o",
               f"{WASM_APPS_DIR}/{benchmark_name}.o",
               f"{WASM_APPS_DIR}/{benchmark_name}.c"]
    try:
        subprocess.run(COMMAND, check=True)
    except  subprocess.CalledProcessError as e:
        print(f"{e}")

def convert_wasm_to_c_header(benchmark_name):
    # Ensure the input file exists
    input_file = f"{WASM_APPS_DIR}/{benchmark_name}.wasm"
    output_file = f"{SRC_DIR}/{benchmark_name}.h" 
    
    if not os.path.isfile(input_file):
        print(f"Error: The file {input_file} does not exist.")
        return

    # Read the wasm file in binary mode
    with open(input_file, 'rb') as wasm_file:
        wasm_bytes = wasm_file.read()

    # Convert the bytes to a comma-separated string of hex values
    byte_array = ', '.join(f'0x{byte:02x}' for byte in wasm_bytes)

    # Create the output string
    output_content = f'unsigned char __aligned(4) wasm_{benchmark_name}[] = {{ {byte_array} }};'

    # Write the output string to the .h file
    with open(output_file, 'w') as header_file:
        header_file.write(output_content)
    
    print(f"Conversion complete. Output saved to {output_file}")

if __name__ == "__main__":
    for benchmark in BENCHMARKS["core"]:
        # build_wasm_benchmark(benchmark)
        # build_native_benchmark(benchmark)
        convert_wasm_to_c_header(benchmark)
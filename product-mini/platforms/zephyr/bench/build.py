import os
import subprocess
import argparse
import threading
import serial
import sys
from abc import ABC, abstractmethod

# Path const definition
CWD = os.getcwd()
APP_SRC_DIR = f"{CWD}/src"
WASM_APPS_DIR = f"{CWD}/wasm-apps"
HOME = "/home/user"

ZEPHYR_BASE = f"{HOME}/zephyrproject/zephyr" 
WAMR_ROOT_DIR = f"{HOME}/wasm-micro-runtime"
WASI_SDK_PATH = f"{HOME}/wasi-sdk-21.0"
WAMR_APP_FRAMEWORK_DIR = f"{HOME}/wamr-app-framework"

CLANG_PATH = f"{WASI_SDK_PATH}/bin/clang"
WASI_SYSROOT = f"{WASI_SDK_PATH}/share/wasi-sysroot"
LLVM_AR_PATH = f"{WASI_SDK_PATH}/bin/llvm-ar"
ZEPHYR_ARM_EABI = f"{HOME}/zephyr-sdk-0.16.5-1/arm-zephyr-eabi"

INC_DIR = f"{WASM_APPS_DIR}/inc"

SOURCE_DIRS = {
    "core": f"{WASM_APPS_DIR}/core/src",
    "socket": f"{WASM_APPS_DIR}/socket/src",
    "thread": f"{WASM_APPS_DIR}/thread/src",
    "fileSystem": f"{WASM_APPS_DIR}/fileSystem/src"
}

OUTPUT_DIRS = {
    "core": {
        "wasm": f"{WASM_APPS_DIR}/core/wasm",
        "native": f"{WASM_APPS_DIR}/core/native",
    },
    "socket": {
        "wasm": f"{WASM_APPS_DIR}/socket/wasm",
        "native": f"{WASM_APPS_DIR}/socket/native",
    },
    "thread": {
        "wasm": f"{WASM_APPS_DIR}/thread/wasm",
        "native": f"{WASM_APPS_DIR}/thread/native",
    },
    "fileSystem": {
        "wasm": f"{WASM_APPS_DIR}/fileSystem/wasm",
        "native": f"{WASM_APPS_DIR}/fileSystem/native",
    }
}

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
        "nestedloop",   
        "random",
        "sieve",
        "dummy"
    ],
    "thread":[],
    "socket":[
        "socket_create",
        "socket_connect",
        "socket_bind",
        "socket_sendto",
        "socket_recvfrom",
        ],
    "fileSystem":[],
} 


# Abstract class for benchmark suite
class BenchmarkSuite(ABC):
    def __init__(self, suite_name: str, source_dir: str, output_dirs: dict):
        self.suite_name = suite_name
        self.source_dir = source_dir
        self.output_dirs = output_dirs

    @abstractmethod
    def build_wasm(self, benchmark_name):
        pass

    @abstractmethod
    def build_native(self, benchmark_name):
        pass

    @abstractmethod
    def clean(self, benchmark_name):
        pass

    def convert_wasm_to_c_header(self, input_file, output_file, benchmark_name):
        # Ensure the input file exists
        
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

# Concrete class for core benchmark suite        
class CoreSuite(BenchmarkSuite):
    def __init__(self):
        super().__init__("core", SOURCE_DIRS["core"], OUTPUT_DIRS["core"])
        
    def build_wasm(self, benchmark_name):
        print(f"Building {benchmark_name}.wasm")
        COMMAND = [f"{CLANG_PATH}",
               f"--sysroot={WASI_SYSROOT}",
               "-nostdlib",
               "-O2",
               "-z",
               "stack-size=8192",
               "-Wl,--max-memory=65536",
               "-Wl,--allow-undefined,--no-entry",
               "-Wl,--export=__heap_base,--export=__data_end",
               f"-Wl,--export=bench_{benchmark_name}",
               "-o",
               f"{self.output_dirs['wasm']}/{benchmark_name}.wasm",
               f"{self.source_dir}/{benchmark_name}.c"]
        try:
            subprocess.run(COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")
            pass

    def build_native(self, benchmark_name):
        print(f"Building {benchmark_name}.o")
        COMMAND = [f"{ZEPHYR_ARM_EABI}/bin/arm-zephyr-eabi-gcc",
                "-O2",
                "-mcpu=cortex-m33+nodsp+nofp", "-mthumb", "-mabi=aapcs", "-mtp=soft",
                "-c",
                "-o",
                f"{self.output_dirs['native']}/{benchmark_name}.o",
                f"{self.source_dir}/{benchmark_name}.c"]
        try:
            subprocess.run(COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")
    
    def clean(self, benchmark_name):
        if os.path.exists(f"{self.output_dirs['wasm']}/{benchmark_name}.wasm"):
            os.remove(f"{self.output_dirs['wasm']}/{benchmark_name}.wasm")
        if os.path.exists(f"{self.output_dirs['native']}/{benchmark_name}.o"):
            os.remove(f"{self.output_dirs['native']}/{benchmark_name}.o")
        if os.path.exists(f"{APP_SRC_DIR}/{benchmark_name}.h"):
            os.remove(f"{APP_SRC_DIR}/{benchmark_name}.h")
        
# Concrete class for socket benchmark suite
class SocketSuite(BenchmarkSuite):
    def __init__(self):
        super().__init__("socket", SOURCE_DIRS["socket"], OUTPUT_DIRS["socket"])
        
    def build_static_lib(self):
        print(f"Building socket static lib")
        
        COMPILE_COMMAND = [f"{CLANG_PATH}",
                f"--sysroot={WASI_SYSROOT}",
                f"-I{INC_DIR}", # var ?
                "-c",
                f"{INC_DIR}/wasi_socket_ext.c",
                "-o",
                f"{INC_DIR}/wasi_socket_ext.o"]
        
        ARCHIVE_COMMAND = [f"{LLVM_AR_PATH}",
                           "rcs",
                           f"{INC_DIR}/libwasi_socket_ext.a",
                           f"{INC_DIR}/wasi_socket_ext.o"]
                           
        try:
            subprocess.run(COMPILE_COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")
        
        try:
            subprocess.run(ARCHIVE_COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}") 
        
    def build_wasm(self, benchmark_name):
        print(f"Building {benchmark_name}.wasm")
        COMMAND = [f"{CLANG_PATH}",
               f"--sysroot={WASI_SYSROOT}",
               f"-L{INC_DIR}/",
               "-lwasi_socket_ext",
               "-lc",
               "-O2",
               "-z",
               "stack-size=8192",
               "-Wl,--max-memory=65536",
               "-Wl,--allow-undefined,--no-entry",
               f"-Wl,--export=bench_{benchmark_name}",
               "-o",
               f"{self.output_dirs['wasm']}/{benchmark_name}.wasm",
               f"{self.source_dir}/{benchmark_name}.c"]
        try:
            subprocess.run(COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")

    def build_native(self, benchmark_name):
        print(f"Building {benchmark_name}.o")
        COMMAND = [f"{ZEPHYR_ARM_EABI}/bin/arm-zephyr-eabi-gcc",
                "-O2",
                # "-DARCH=armv8-m.main",
                # "-DBOARD=nucleo_h563zi",
                # "-DCONFIG_ARM=y",
                "-DCPU_CORTEX_M33=y",
                f"-I{ZEPHYR_BASE}/include",
                "-mcpu=cortex-m33", "-mthumb", "-mabi=aapcs", "-mtp=soft",
                "-c",
                "-o",
                f"{self.output_dirs['native']}/{benchmark_name}.o",
                f"{self.source_dir}/{benchmark_name}.c"]
        try:
            subprocess.run(COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")
            
    def clean(self, benchmark_name):
        if os.path.exists(f"{self.output_dirs['wasm']}/{benchmark_name}.wasm"):
            os.remove(f"{self.output_dirs['wasm']}/{benchmark_name}.wasm")
        if os.path.exists(f"{self.output_dirs['native']}/{benchmark_name}.o"):
            os.remove(f"{self.output_dirs['native']}/{benchmark_name}.o")
        if os.path.exists(f"{APP_SRC_DIR}/{benchmark_name}.h"):
            os.remove(f"{APP_SRC_DIR}/{benchmark_name}.h")
        if os.path.exists(f"{INC_DIR}/wasi_socket_ext.o"):
            os.remove(f"{INC_DIR}/wasi_socket_ext.o")
        if os.path.exists(f"{INC_DIR}/libwasi_socket_ext.a"):
            os.remove(f"{INC_DIR}/libwasi_socket_ext.a")

# Generic function to clean generated files
def clean_generated_files(suite: BenchmarkSuite) -> None:
    if isinstance(suite, CoreSuite) or isinstance(suite, SocketSuite):
        for benchmark in BENCHMARKS[suite.suite_name]:
            suite.clean(benchmark)
    else:
        print("Unsupported suite type.")

# Generic function to build benchmarks
def build_benchmarks(suite: BenchmarkSuite) -> None:
    if isinstance(suite, CoreSuite) or isinstance(suite, SocketSuite):
        for benchmark in BENCHMARKS[suite.suite_name]:
            suite.build_wasm(benchmark)
            suite.build_native(benchmark)
            suite.convert_wasm_to_c_header(
                f"{suite.output_dirs['wasm']}/{benchmark}.wasm",
                f"{APP_SRC_DIR}/{benchmark}.h",
                benchmark
            )
    else: 
        print("Unsupported suite type.")

# Function to parse command line arguments
def parse_arguments():
    parser = argparse.ArgumentParser(description='Build benchmarks.')
    parser.add_argument('-s', '--suite', choices=['core', 'socket', 'thread', 'filesystem'], required=True, help='Benchmark suite to build')
    parser.add_argument('--clean', action='store_true', help='Clean generated files for the selected suite')
    return parser.parse_args()  

if __name__ == "__main__":
    args = parse_arguments()
    selected_suite = args.suite
    
    if selected_suite == "core":
        core_suite = CoreSuite()
        if args.clean:
            clean_generated_files(core_suite)
            print("Cleaned core suite generated files.")
        else:
            print("Build core suite.")
            build_benchmarks(core_suite)
            print("Build complete.")
            
    elif selected_suite == "socket":
        socket_suite = SocketSuite()
        if args.clean:
            clean_generated_files(socket_suite)
            print("Cleaned core suite generated files.")
        else:
            print("Build core suite.")
            socket_suite.build_static_lib()
            build_benchmarks(socket_suite)
            print("Build complete.")
            print("[WARNING] Ensure that 'WAMR_BUILD_LIBC_WASI' is enabled in the 'CMakeLists.txt'.") 
              
    elif selected_suite == "thread":
        print("Unsupported suite name.")
        pass
    elif selected_suite == "filesystem":
        print("Unsupported suite name.")
        pass
    else:
        print("Invalid suite name.")
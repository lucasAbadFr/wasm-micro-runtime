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
        ],
    "fileSystem":[],
} 


# subprocess.run( [f"ZEPHYR_BASE={ZEPHYR_BASE}", f"WAMR_ROOT_DIR={WAMR_ROOT_DIR}", 
#          f"WASI_SDK_PATH={WASI_SDK_PATH}", f"WAMR_APP_FRAMEWORK_DIR={WAMR_APP_FRAMEWORK_DIR}",
#          "west", "build", ".", "-b", "nucleo_h743zi", "-p", "always"], check=True)


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
               "-Wl,--initial-memory=65536",
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
               "-Wl,--initial-memory=65536",
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
                "-mcpu=cortex-m33+nodsp+nofp", "-mthumb", "-mabi=aapcs", "-mtp=soft",
                "-c",
                "-o",
                f"{self.output_dirs['native']}/{benchmark_name}.o",
                f"{self.source_dir}/{benchmark_name}.c"]
        try:
            subprocess.run(COMMAND, check=True)
        except  subprocess.CalledProcessError as e:
            print(f"{e}")
        
class SerialReader(threading.Thread):
    def __init__(self, port, baud_rate=9600):
        super().__init__()
        self.serial_port = serial.Serial(port, baud_rate, timeout=1)
        self.running = True
        self.data = []  # Shared structure to store results

    def run(self):
        while self.running:
            if self.serial_port.in_waiting:
                line = self.serial_port.readline().decode('utf-8').strip()
                self.data += line
                # Process the line here and store in self.data
                # Example: self.data[benchmark_name] = result
                # print(line)  # For demonstration

    def stop(self):
        self.running = False
        self.serial_port.close()


def parse_arguments():
    parser = argparse.ArgumentParser(description='Build benchmarks.')
    parser.add_argument('-s', '--suite', choices=['core', 'socket', 'thread', 'filesystem'], required=True, help='Benchmark suite to build')
    return parser.parse_args()  

if __name__ == "__main__":
    args = parse_arguments()
    # serial_reader = SerialReader("/dev/ttyACM0", 115200)
    # serial_reader.start()
    selected_suite = args.suite
    benchmarks = BENCHMARKS[args.suite]
    
    if selected_suite == "core":
        print("Building core suite.")
        core_suite = CoreSuite()
        
        for benchmark in benchmarks:
            core_suite.build_wasm(benchmark)
            core_suite.build_native(benchmark)
            core_suite.convert_wasm_to_c_header(
                f"{core_suite.output_dirs['wasm']}/{benchmark}.wasm",
                f"{APP_SRC_DIR}/{benchmark}.h",
                benchmark
            )

        print("Build complete.")
    elif selected_suite == "socket":
        print("Building socket suite.")
        socket_suite = SocketSuite()
        socket_suite.build_static_lib()
    
        for benchmark in benchmarks:
            socket_suite.build_wasm(benchmark)
            socket_suite.build_native(benchmark)
            socket_suite.convert_wasm_to_c_header(
                f"{socket_suite.output_dirs['wasm']}/{benchmark}.wasm",
                f"{APP_SRC_DIR}/{benchmark}.h",
                benchmark
            )
        
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
        
    

        # try:
        #     # Place benchmark execution code here
        #     # The serial reader will collect data in the background
        #     west_build()
        #     west_flash()
        #     pass
        # finally:
        #     # Stop the serial reader thread after benchmarking
        #     # serial_reader.stop()
        #     # serial_reader.join()
    
    # Process or print the collected data
    # print(serial_reader.data)
    
    
# def west_build():
#     env = os.environ.copy()
    
#     # Get the path to the virtual environment's bin directory
#     venv_bin_path = os.path.join(os.path.dirname(sys.executable), 'bin')
    
#     # Prepend the virtual environment's bin directory to the PATH
#     env["PATH"] = venv_bin_path + os.pathsep + env["PATH"]
    
#     env["ZEPHYR_BASE"] = ZEPHYR_BASE
#     env["WAMR_ROOT_DIR"] = WAMR_ROOT_DIR
#     env["WASI_SDK_PATH"] = WASI_SDK_PATH
#     env["WAMR_APP_FRAMEWORK_DIR"] = WAMR_APP_FRAMEWORK_DIR
    
#     COMMAND = ["west",
#               "build",
#               ".",
#               "-b",
#               "nucleo_h743zi",
#               "-p",
#               "always"]
#     try: 
#         subprocess.run(COMMAND, env=env, check=True)
#     except  subprocess.CalledProcessError as e:
#         print(f"{e}")

# def west_flash():
#     try:
#         subprocess.run( [f"ZEPHYR_BASE={ZEPHYR_BASE}", "west", "flash"], check=True)
#     except  subprocess.CalledProcessError as e:
#         print(f"{e}")
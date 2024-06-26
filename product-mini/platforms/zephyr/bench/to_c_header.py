# Python script to convert wasm file to byte array in a .h file
import os
import sys

CWD = os.getcwd()

def convert_wasm_to_c_header(input_wasm, output_header):
    # Ensure the input file exists
    input_path = f"{CWD}/{input_wasm}"
    output_path = f"{CWD}/{output_header}" 
    
    if not os.path.isfile(input_path):
        print(f"Error: The file {input_wasm} does not exist.")
        return

    # Read the wasm file in binary mode
    with open(input_path, 'rb') as wasm_file:
        wasm_bytes = wasm_file.read()

    # Convert the bytes to a comma-separated string of hex values
    byte_array = ', '.join(f'0x{byte:02x}' for byte in wasm_bytes)

    # Create the output string
    output_content = f'unsigned char __aligned(4) wasm_test_file[] = {{ {byte_array} }};'

    # Write the output string to the .h file
    with open(output_path, 'w') as header_file:
        header_file.write(output_content)
    print(f"Conversion complete. Output saved to {output_header}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 to_c_header.py <input_wasm_file> <output_header_file>")
    else:
        input_wasm = sys.argv[1]
        output_header = sys.argv[2]
        convert_wasm_to_c_header(input_wasm, output_header)
#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <path_to_input_file> <path_to_replacement_file> <path_to_output_file>"
    exit 1
fi

# Assign arguments to variables
input_file="$1"
replacement_file="$2"
output_file="$3"

# Region parameters (in hex)
start_hex="03000000"
length_hex="01000000"

# Convert hex values to decimal
start_dec=$((16#$start_hex))
length_dec=$((16#$length_hex))

# Check if the size of the replacement file matches the target region length
replacement_size=$(stat -c%s "$replacement_file")
if [ "$replacement_size" -ne "$length_dec" ]; then
    echo "Error: Replacement file size ($replacement_size bytes) does not match the target region size ($length_dec bytes)."
    exit 1
fi

# Create a copy of the input file to the output file
cp "$input_file" "$output_file"

# Replace the content in the output file starting from the specified position with a larger block size
dd if="$replacement_file" of="$output_file" bs=4K seek=$((start_dec / 4096)) conv=notrunc status=none

echo "Region successfully replaced in the output file: $output_file"
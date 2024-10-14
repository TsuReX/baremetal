#!/bin/bash

# Helper function to convert hexadecimal to decimal
hex2dec() {
  echo "$1" | awk '{ printf "%d\n", strtonum($1) }'
}

# Helper function to pad data with zeros if needed
pad_with_zeros() {
  local data=$1
  local length=$2
  local padded=$(printf "%-${length}s" "$data" | tr ' ' '00')
  echo "$padded"
}

# Helper function to handle file and inline data sources
get_data_source() {
  local source=$1
  local start_addr=$2
  local length=$3
  local source_type

  # Check if it's a file or inline data
  if [[ "$source" =~ ^0x ]]; then
    # It's inline data
    source_type="inline"
    local inline_data=$(echo "$source" | sed 's/^0x//')
    local data="${inline_data:$start_addr:$length}"
    echo "$data"
  else
    # It's a file
    source_type="file"
    if [ ! -f "$source" ]; then
      echo "Error: Data source file $source does not exist."
      exit 1
    fi
    local file_size=$(stat -c%s "$source")
    if [ "$start_addr" -ge "$file_size" ]; then
      echo "Error: Start address $start_addr exceeds the size of $source."
      exit 1
    fi
    local data=$(xxd -p -l "$length" -s "$start_addr" "$source")
    echo "$data"
  fi
}

# Main script
input_file="$1"
substitution_file="$2"
output_file="$3"

if [[ ! -f "$input_file" ]]; then
  echo "Error: Input file $input_file does not exist."
  exit 1
fi

if [[ ! -f "$substitution_file" ]]; then
  echo "Error: Substitution map file $substitution_file does not exist."
  exit 1
fi

# Copy input file to output file
cp "$input_file" "$output_file"

# Iterate over substitution map
while IFS="|" read -r dest_addr_hex src_addr_hex length_hex source; do
  # Remove whitespace
  dest_addr_hex=$(echo "$dest_addr_hex" | xargs)
  src_addr_hex=$(echo "$src_addr_hex" | xargs)
  length_hex=$(echo "$length_hex" | xargs)
  source=$(echo "$source" | xargs)

  # Skip comments

  [[ "$dest_addr_hex" =~ ^# ]] && continue

  # Convert hex values to decimal
  dest_addr=$(hex2dec "$dest_addr_hex")
  src_addr=$(hex2dec "$src_addr_hex")
  length=$(hex2dec "$length_hex")

  # If length is zero, we need the whole source
  if [ "$length" -eq 0 ]; then
    if [[ "$source" =~ ^0x ]]; then
      # Inline data, get the full length
      length=$((${#source} / 2))

    else
      # It's a file, get the full file size
      length=$(stat -c%s "$source")

    fi
  fi

  # Check destination bounds
  dest_file_size=$(stat -c%s "$output_file")
  if [ "$dest_addr" -ge "$dest_file_size" ]; then
    echo "Error: Start address $dest_addr exceeds the size of $output_file."
    exit 1
  fi

  # Get the data from the source (file or inline)
  data=$(get_data_source "$source" "$src_addr" "$length")

  # Check if the length is less than requested, pad with zeros
  if [ "${#data}" -lt "$length" ]; then
    data=$(pad_with_zeros "$data" "$length")
  fi

  # Perform the substitution in the output file
  echo "Substituting data at destination address $dest_addr_hex from source $source"
  echo "$data" | xxd -r -p -seek "$dest_addr" - "$output_file"

done < "$substitution_file"

echo "Substitution completed successfully."

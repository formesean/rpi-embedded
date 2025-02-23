#!/bin/bash

# Check if a filename was provided
if [ -z "$1" ]; then
  echo "Usage: ./compile.sh <main_file.cpp>"
  exit 1
fi

# Extract the main file name without extension
MAIN_FILE=$1
OUTPUT_NAME=${MAIN_FILE%.cpp}

# Compile the file
g++ -o "$OUTPUT_NAME" "$MAIN_FILE" ../lib/AikaPi/AikaPi.cpp ../lib/MCP23S17/MCP23S17.cpp -lpigpio -lrt -pthread

# Check if compilation was successful
if [ $? -eq 0 ]; then
  echo "Compilation successful. Run ./$OUTPUT_NAME"
else
  echo "Compilation failed."
fi

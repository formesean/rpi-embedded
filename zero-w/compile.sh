#!/bin/bash

# Check if a filename was provided
if [ -z "$1" ]; then
  echo "Usage: ./compile.sh <main_file.cpp>"
  exit 1
fi

MAIN_FILE=$1
OUTPUT_NAME=${MAIN_FILE%.cpp}

# Source files
SOURCES=(
  "$MAIN_FILE"
  "../lib/AikaPi/AikaPi.cpp"
)

OBJECTS=()
COMPILED=0
TOTAL=${#SOURCES[@]}

start_time=$(date +%s)

# Progress bar
show_progress() {
  local width=40
  local percent=$((COMPILED * 100 / TOTAL))
  local filled=$((COMPILED * width / TOTAL))
  local empty=$((width - filled))

  bar=$(printf "%0.s#" $(seq 1 $filled))
  space=$(printf "%0.s." $(seq 1 $empty))

  echo -ne "\r[${bar}${space}] ${percent}% (${COMPILED}/${TOTAL})"
}

echo "Compiling sources..."

# Compile each source file to object file
for src in "${SOURCES[@]}"; do
  obj="${src##*/}"
  obj="${obj%.cpp}.o"
  g++ -c "$src" -o "$obj" -pthread
  if [ $? -ne 0 ]; then
    echo -e "\nCompilation failed at $src"
    exit 1
  fi
  OBJECTS+=("$obj")
  COMPILED=$((COMPILED + 1))
  show_progress
done

# Link object files
echo -ne "\nLinking...\n"
g++ -o "$OUTPUT_NAME" "${OBJECTS[@]}" -pthread
status=$?

end_time=$(date +%s)
elapsed=$((end_time - start_time))

# Cleanup object files
rm -f "${OBJECTS[@]}"

# Result
if [ $status -eq 0 ]; then
  echo "Build complete in ${elapsed}s. Run ./$OUTPUT_NAME"
else
  echo "Linking failed after ${elapsed}s."
fi

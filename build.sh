#!/usr/bin/env bash

set -e

COMPILER="./build/compiler"
RUNTIME_DIR="./runtime"
BUILD_DIR="./build"
INPUT="$1"


if [ -z "$INPUT" ]; then
	echo "Usage: ./build.sh file.txt"
	exit 1
fi

mkdir -p "$BUILD_DIR"

BASENAME=$(basename "$INPUT" .txt)

ASM_FILE="$BUILD_DIR/$BASENAME.s"
OBJ_FILE="$BUILD_DIR/$BASENAME.o"
OUT_FILE="$BUILD_DIR/$BASENAME"

echo "== COMPILING =="
$COMPILER "$INPUT" "$ASM_FILE"

echo "== ASSEMBLING =="
aarch64-linux-gnu-as "$ASM_FILE" -o "$OBJ_FILE"


echo "== PREPARING RUNTIME =="
RUNTIME_OBJS=""

for f in "$RUNTIME_DIR"/*.s; do
    obj="$BUILD_DIR/$(basename "$f" .s).o"
    aarch64-linux-gnu-as "$f" -o "$obj"
    RUNTIME_OBJS="$RUNTIME_OBJS $obj"
done

echo "== LINKING =="
aarch64-linux-gnu-ld "$OBJ_FILE" $RUNTIME_OBJS -o "$OUT_FILE"

echo "== DONE =="
echo "Executable: $OUT_FILE"

echo "== Running =="
"$OUT_FILE"

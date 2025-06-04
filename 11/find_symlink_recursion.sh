#!/bin/bash

BASE_FILENAME="a"
LINK_PREFIX="link_"
TEMP_DIR="symlink_test_$$"

mkdir "$TEMP_DIR"
if [ $? -ne 0 ]; then
    echo "Error creating temporary directory: $TEMP_DIR" >&2
    exit 1
fi
echo "Created temporary directory: $TEMP_DIR"

touch "$TEMP_DIR/$BASE_FILENAME"
if [ $? -ne 0 ]; then
    echo "Error creating base file: $TEMP_DIR/$BASE_FILENAME" >&2
    rm -rf "$TEMP_DIR"
    exit 1
fi

CURRENT_LINK="$TEMP_DIR/${LINK_PREFIX}0"
ln -s "$BASE_FILENAME" "$CURRENT_LINK"
if [ $? -ne 0 ]; then
    echo "Error creating initial symlink: $CURRENT_LINK -> $BASE_FILENAME" >&2
    rm -rf "$TEMP_DIR"
    exit 1
fi

depth=1

while true; do
    NEXT_LINK="$TEMP_DIR/${LINK_PREFIX}$depth"
    ln -s "${LINK_PREFIX}$((depth-1))" "$NEXT_LINK"
    if [ $? -ne 0 ]; then
        echo "Error creating symlink: $NEXT_LINK -> ${LINK_PREFIX}$((depth-1))" >&2
        break
    fi
    
    readlink -f "$NEXT_LINK" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        break
    fi

    CURRENT_LINK="$NEXT_LINK"
    depth=$((depth + 1))
done

echo "Cleaning up created files in $TEMP_DIR..."
rm -rf "$TEMP_DIR"
if [ $? -ne 0 ]; then
    echo "Error removing temporary directory: $TEMP_DIR" >&2
fi

echo "Symlink recursion depth: $depth"

exit 0 
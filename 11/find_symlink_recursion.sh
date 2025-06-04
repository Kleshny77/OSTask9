#!/bin/bash

# Define constants
BASE_FILENAME="a"
LINK_PREFIX="link_"
TEMP_DIR="symlink_test_$$" # Use $$ for a unique directory name

# Create a temporary directory
mkdir "$TEMP_DIR"
if [ $? -ne 0 ]; then
    echo "Error creating temporary directory: $TEMP_DIR" >&2
    exit 1
fi
echo "Created temporary directory: $TEMP_DIR"

# Create the base file inside the temporary directory
touch "$TEMP_DIR/$BASE_FILENAME"
if [ $? -ne 0 ]; then
    echo "Error creating base file: $TEMP_DIR/$BASE_FILENAME" >&2
    rm -rf "$TEMP_DIR"
    exit 1
fi

# Initial link points to the base file
CURRENT_LINK="$TEMP_DIR/${LINK_PREFIX}0"
ln -s "$BASE_FILENAME" "$CURRENT_LINK"
if [ $? -ne 0 ]; then
    echo "Error creating initial symlink: $CURRENT_LINK -> $BASE_FILENAME" >&2
    rm -rf "$TEMP_DIR"
    exit 1
fi

depth=1 # Start with depth 1 for the first link

# Loop to create subsequent symbolic links
while true; do
    NEXT_LINK="$TEMP_DIR/${LINK_PREFIX}$depth"
    # New link points to the previous link (relative path within temp dir)
    ln -s "${LINK_PREFIX}$((depth-1))" "$NEXT_LINK"
    if [ $? -ne 0 ]; then
        echo "Error creating symlink: $NEXT_LINK -> ${LINK_PREFIX}$((depth-1))" >&2
        break # Break if symlink creation fails
    fi
    
    # Attempt to read the target of the newest link (this operation hits recursion limit in bash)
    readlink -f "$NEXT_LINK" > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        # If readlink fails, we hit the recursion limit
        # We don't need to unlink NEXT_LINK here because it was never successfully read/followed
        break
    fi

    # Move to the next link
    CURRENT_LINK="$NEXT_LINK"
    depth=$((depth + 1))
done

# Clean up the temporary directory and its contents
echo "Cleaning up created files in $TEMP_DIR..."
rm -rf "$TEMP_DIR"
if [ $? -ne 0 ]; then
    echo "Error removing temporary directory: $TEMP_DIR" >&2
fi

echo "Symlink recursion depth: $depth"

exit 0 
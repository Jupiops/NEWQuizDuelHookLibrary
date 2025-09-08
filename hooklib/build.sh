#!/bin/bash

# Enable strict error handling
set -e

# Navigate to the module directory (adjust this path if necessary)
MODULE_DIR="$(dirname "$0")"
cd "$MODULE_DIR"

# Step 1: Execute ndk-build command
ndk-build -j4

# Print success message
echo "Build script executed successfully."
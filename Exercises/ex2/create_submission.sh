#!/bin/bash

# Script to create a ZIP file with the updated uThreads library files
# ZIP file: Operating_Systems_Exercise_2_Nir_Dahan_Liav_Sarfati.zip
# This script will overwrite the existing ZIP file each time it runs

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ZIP_NAME="Operating_Systems_Exercise_2_Nir_Dahan_Liav_Sarfati.zip"
ZIP_PATH="$SCRIPT_DIR/$ZIP_NAME"

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║            Creating Submission ZIP File                        ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Remove existing ZIP if it exists
if [ -f "$ZIP_PATH" ]; then
    echo "✓ Removing existing ZIP file: $ZIP_NAME"
    rm -f "$ZIP_PATH"
fi

# Create new ZIP with the updated files
echo "✓ Creating new ZIP file: $ZIP_NAME"
cd "$SCRIPT_DIR"

zip -j "$ZIP_PATH" \
    uthreads.c \
    uthread_queue.c \
    uthread_queue.h

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                    SUCCESS!                                    ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "📦 ZIP file created: $ZIP_PATH"
echo "📊 File size: $(ls -lh "$ZIP_PATH" | awk '{print $5}')"
echo "📋 Contents:"
unzip -l "$ZIP_PATH" | tail -n +4
echo ""

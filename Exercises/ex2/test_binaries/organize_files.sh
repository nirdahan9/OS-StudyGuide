#!/bin/bash

echo "🗂️ Organizing files in ex2 directory..."

# Create necessary directories if they don't exist
mkdir -p documentation
mkdir -p test_binaries
mkdir -p debug_scripts

# Move documentation files
echo "📄 Moving documentation files..."
mv -f *.md documentation/ 2>/dev/null || true
mv -f *.pdf documentation/ 2>/dev/null || true
mv -f *.txt documentation/ 2>/dev/null || true
mv -f *.docx documentation/ 2>/dev/null || true

# Move test binaries
echo "🧪 Moving test binaries..."
mv -f test_provided test_uthreads quick_test debug_test sleep_test signal_test simple_test test_block_debug 2>/dev/null || true
for file in test_provided test_uthreads quick_test debug_test sleep_test signal_test simple_test test_block_debug; do
    [ -f "$file" ] && mv "$file" test_binaries/
done

# Move debug and temporary scripts
echo "🔧 Moving debug scripts..."
mv -f *.sh test_binaries/ 2>/dev/null || true
mv -f *.c.bak test_binaries/ 2>/dev/null || true
mv -f signal_test.c sleep_test.c simple_manual_test.c test_block_debug.c 2>/dev/null || true
for file in signal_test.c sleep_test.c simple_manual_test.c test_block_debug.c; do
    [ -f "$file" ] && mv "$file" debug_scripts/
done

# Move ZIP file to documentation
echo "📦 Moving ZIP file..."
mv -f *zip documentation/ 2>/dev/null || true

# Clean up macOS files
echo "🧹 Removing macOS files..."
rm -f .DS_Store *.o 2>/dev/null || true

# Move run_all_tests.sh back to root for easy access
echo "⚡ Restoring run_all_tests.sh to root..."
[ -f test_binaries/run_all_tests.sh ] && cp test_binaries/run_all_tests.sh .

# gdb_script to debug_scripts
[ -f gdb_script.txt ] && mv gdb_script.txt debug_scripts/

# write_md.py to documentation
[ -f write_md.py ] && mv write_md.py documentation/

echo "✅ Organization complete!"
echo ""
echo "📁 Directory structure:"
tree -L 1 -I "my_tests" 2>/dev/null || ls -la | grep "^d"

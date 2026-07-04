#!/bin/bash

echo "=== Build ==="
make clean && make

echo "=== Test: --help ==="
./rename_tool -h

echo "=== Test: Normal rename ==="
git checkout HEAD -- test_dir
./rename_tool test_dir --match "hello.txt" --replace "world.txt"

echo "=== Test: Overwrite protection ==="
git checkout HEAD -- test_dir
echo "dummy" > test_dir/world.txt
./rename_tool test_dir --match "hello.txt" --replace "world.txt"

echo "=== Test: Wildcard matching ==="
git checkout HEAD -- test_dir
./rename_tool test_dir --match "*.txt" --replace ".bak"

echo "=== Test: Recursive directories ==="
git checkout HEAD -- test_dir
./rename_tool test_dir --match "file2.txt" --replace "doc2.txt"

echo "All tests completed"

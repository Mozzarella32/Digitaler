#!/bin/bash
# Script to test all ICE reduction cases with different g++ versions

echo "==================================="
echo "GCC ICE Test Script"
echo "==================================="
echo ""

# Function to test a file with a specific compiler
test_file() {
    local compiler=$1
    local file=$2
    local expected=$3  # "ICE" or "OK"
    
    echo -n "Testing $file with $compiler... "
    
    # Compile and capture output
    output=$($compiler -std=c++23 "$file" -o /tmp/test_out 2>&1)
    exit_code=$?
    
    # Check if ICE occurred
    if echo "$output" | grep -q "internal compiler error"; then
        if [ "$expected" == "ICE" ]; then
            echo "✅ ICE (as expected)"
        else
            echo "❌ Unexpected ICE"
        fi
    else
        if [ "$expected" == "OK" ]; then
            echo "✅ Compiles OK"
        else
            echo "❌ Expected ICE but compiled successfully"
        fi
    fi
    
    # Cleanup
    rm -f /tmp/test_out
}

# Test main files
echo "Testing main test cases:"
echo "-----------------------"

for compiler in g++-12 g++-13 g++-14; do
    if command -v $compiler &> /dev/null; then
        echo ""
        echo "Using $compiler:"
        test_file $compiler "ice_original.cpp" "OK"
        test_file $compiler "ice_minimal.cpp" "OK"
    else
        echo "$compiler not found, skipping..."
    fi
done

echo ""
echo "==================================="
echo "Testing ice_minimal.cpp specifically with g++-14 (should ICE):"
echo "==================================="
test_file g++-14 "ice_minimal.cpp" "ICE"

echo ""
echo "==================================="
echo "Quick test of all reductions with g++-14:"
echo "==================================="

for file in ice_reductions/ice_reduction*.cpp; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        echo -n "$filename: "
        if g++-14 -std=c++23 "$file" -o /tmp/test_out 2>&1 | grep -q "internal compiler error"; then
            echo "ICE ✗"
        else
            echo "OK ✓"
        fi
        rm -f /tmp/test_out
    fi
done

echo ""
echo "==================================="
echo "Test complete!"
echo "==================================="

# ICE Test Cases

This directory contains test cases for analyzing a g++-14 Internal Compiler Error.

## Main Test Files

- `ice_original.cpp` - The original code provided by the user
- `ice_minimal.cpp` - The minimal reproducer (smallest code that triggers ICE)

## Reductions Directory

The `ice_reductions/` directory contains 16 different reduction attempts to identify the minimal reproducer.

### Key Findings from Reductions

| File | Triggers ICE? | Description |
|------|--------------|-------------|
| reduction1.cpp | ❌ No | Remove outer parameter name |
| reduction2.cpp | ❌ No | Remove outer function entirely |
| reduction3.cpp | ❌ No | Make zero non-template |
| reduction4.cpp | ❌ No | Remove zero entirely |
| reduction5.cpp | ❌ No | Remove auto param from second lambda |
| reduction6.cpp | ❌ No | Remove auto param from Type lambda |
| reduction7.cpp | ❌ No | Empty body in Type lambda (no auto param) |
| reduction8.cpp | ❌ No | Remove auto param from second lambda |
| reduction9.cpp | ✅ Yes | Simplify Type lambda body (keep auto) |
| reduction10.cpp | ✅ Yes | Further simplify Type lambda body |
| reduction11.cpp | ✅ Yes | Change outer return type to void |
| reduction12.cpp | ✅ Yes | **MINIMAL - Remove return from second lambda** |
| reduction13.cpp | ❌ No | Remove using alias (inline Type usage) |
| reduction14.cpp | ❌ No | Remove template variable zero |
| reduction15.cpp | ❌ No | Concrete type for outer parameter |
| reduction16.cpp | ❌ No | Concrete type for Type lambda parameter |

## Testing Instructions

### Test with g++-14 (triggers ICE)
```bash
cd test_cases
g++-14 -std=c++23 ice_original.cpp
g++-14 -std=c++23 ice_minimal.cpp
```

### Test with g++-13 (should compile successfully)
```bash
g++-13 -std=c++23 ice_original.cpp
g++-13 -std=c++23 ice_minimal.cpp
```

### Test with g++-12 (should compile successfully)
```bash
g++-12 -std=c++23 ice_original.cpp
g++-12 -std=c++23 ice_minimal.cpp
```

## ICE Requirements

For the ICE to occur, ALL of the following are required:
1. Template variable with type parameter (e.g., `template <typename T> constexpr int zero`)
2. Template struct with non-type auto parameter (e.g., `template <auto> struct Type`)
3. Function with auto parameter (e.g., `void outer(auto)`)
4. Using alias for Type instantiated with a lambda
5. Lambda inside Type<> must have auto parameter
6. Second lambda that references the template variable with the using alias as template arg
7. Instantiation of the second lambda

Removing ANY of these elements prevents the ICE from occurring.

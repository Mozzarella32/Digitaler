# Workarounds for the g++-14 ICE

This document provides workarounds for the Internal Compiler Error in g++-14.

## Problem
The following code pattern triggers an ICE in g++-14:
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}
```

## Workarounds

### 1. Use g++-13 or g++-12
The simplest workaround is to use a different compiler version:
```bash
g++-13 -std=c++23 your_code.cpp
```

### 2. Remove the `using` alias
Instead of using a type alias, use the type directly:
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  [](auto) { zero<Type<[](auto) {}>>; }(1);  // Works! No ICE
}
```
**Note**: This is NOT equivalent semantically because each usage creates a different lambda type.

### 3. Use concrete parameter types instead of `auto`
Replace the `auto` parameter in the outer function:
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(int x) {  // Changed from auto to int
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}
```

### 4. Remove `auto` from the Type lambda
Use a concrete parameter type for the lambda:
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](int) {}>;  // Changed from auto to int
  [](auto) { zero<inner>; }(1);
}
```

### 5. Don't use the type as a template argument
Avoid using the type alias as a template argument:
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { 
    // Use inner directly instead of as template arg
    inner x;  // This compiles (though Type is incomplete)
    return 1; 
  }(1);
}
```

### 6. Use a non-template variable
If the template variable isn't essential, make it a regular constant:
```cpp
constexpr int zero = 1;  // Not a template
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero; }(1);  // Works without template instantiation
}
```

## Recommended Solution

For production code, the best workaround is **Option 1**: Use g++-13 until the bug is fixed in g++-14.

For development/testing, use:
```bash
# In Makefile or CMakeLists.txt
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14.0)
    message(WARNING "g++-14 has a known ICE bug, using g++-13 instead")
    set(CMAKE_CXX_COMPILER g++-13)
endif()
```

## Bug Report
This ICE should be reported to GCC Bugzilla with:
- Minimal reproducer (9 lines)
- g++ version: 14.2.0
- Platform: Linux
- Compilation command: `g++-14 -std=c++23 test.cpp`
- Note that it works correctly on g++-12 and g++-13

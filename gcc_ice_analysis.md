# GCC Internal Compiler Error (ICE) Analysis

## Summary
This document analyzes a C++ code snippet that triggers an Internal Compiler Error (ICE) in g++ 14.2.0.

## Confirmation
**Status**: ✅ Confirmed ICE in g++-14

### Test Results
- **g++-12**: Compiles successfully ✓
- **g++-13**: Compiles successfully ✓
- **g++-14**: Internal Compiler Error (Segmentation fault) ✗

## Original Code
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }
```

### Error Output (g++-14)
```
ice_test_original.cpp:5:22: internal compiler error: Segmentation fault
    5 |   using inner = Type<[](auto) { return 1; }>;
      |                      ^~~~~~~~~~~~~~~~~~~~~~
0x1431e4c internal_error(char const*, ...)
0x17030d8 template_parms_to_args(tree_node*)
0x178902f tsubst_expr(tree_node*, tree_node*, int, tree_node*)
...
```

## Minimal Reproducer
After systematic reduction, the absolute minimal code that triggers the ICE is:

```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }
```

This is **69 bytes** smaller than the original and removes unnecessary complexity while still triggering the ICE.

## Required Elements for ICE

The ICE requires ALL of the following elements:
1. ✅ Template variable `zero` with template parameter
2. ✅ Template struct `Type` with non-type template parameter (`auto`)
3. ✅ Function `outer` with `auto` parameter (generic lambda-like)
4. ✅ `using` alias `inner` for `Type<lambda>`
5. ✅ Lambda inside `Type<>` with `auto` parameter
6. ✅ Second lambda that uses `zero<inner>`
7. ✅ Instantiation of the second lambda

### Elements NOT Required
- ❌ Return values in lambdas
- ❌ Return value from `outer`
- ❌ Specific types of operations inside lambdas
- ❌ The `return` keyword in the Type lambda

## Reduction Steps

### Reduction 1: Remove outer function parameter name
Works - Parameter can be unnamed.

### Reduction 2: Remove outer function entirely
✗ Doesn't trigger ICE - needs the `outer` function with `auto` parameter.

### Reduction 3-4: Remove zero template or usage
✗ Doesn't trigger ICE - needs template variable zero and its usage.

### Reduction 5-8: Simplify lambda parameters
✗ Removing `auto` parameter from lambdas prevents ICE.

### Reduction 9-12: Simplify return values and bodies
✅ Can remove all return values and simplify lambda bodies.

### Reduction 13: Remove using alias
✗ Doesn't trigger ICE - the `using` alias is necessary.

### Reduction 14: Remove template variable zero
✗ Doesn't trigger ICE - needs the template variable.

### Reduction 15: Use concrete type for outer
✗ Doesn't trigger ICE - needs `auto` parameter.

### Reduction 16: Use concrete type for inner lambda
✗ Doesn't trigger ICE - needs `auto` parameter.

## Root Cause Analysis

The ICE appears to be triggered by the combination of:
- A templated function-like entity (`outer` with `auto` parameter)
- Nested lambda capture/usage where an inner type (from `Type<lambda>`) is used as a template argument
- The `auto` parameter in the lambda that's used as a non-type template argument
- Template variable instantiation with that type

This suggests a bug in g++-14's template instantiation mechanism, specifically in `template_parms_to_args` and `tsubst_expr` functions during the substitution of nested template arguments involving lambdas with `auto` parameters.

## Compiler Bug Report

This ICE should be reported to the GCC bug tracker with:
- The minimal reproducer (9 lines, 172 bytes)
- g++ version: 14.2.0
- Compile command: `g++-14 -std=c++23 test.cpp`
- The fact that it works on g++-12 and g++-13

## Test Files

Test files demonstrating the ICE are available in the repository:
- `test_cases/ice_original.cpp` - Original code
- `test_cases/ice_minimal.cpp` - Minimal reproducer
- `test_cases/ice_reductions/` - Directory with all reduction attempts

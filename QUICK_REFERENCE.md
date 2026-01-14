# Quick Reference: g++-14 ICE Issue

## ⚠️ Issue Summary
g++-14 has an Internal Compiler Error when compiling code with nested lambdas and template parameters.

## ✅ Confirmation
- **g++-12**: Works ✓
- **g++-13**: Works ✓  
- **g++-14**: ICE (Segmentation fault) ✗

## 📝 Minimal Reproducer (9 lines)
```cpp
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }
```

**Compile with:** `g++-14 -std=c++23 test.cpp`

## 🔧 Quick Fix
Use g++-13 instead:
```bash
g++-13 -std=c++23 your_code.cpp
```

## 📂 Files in This Repository

- **gcc_ice_analysis.md** - Detailed analysis of the ICE
- **test_cases/ice_original.cpp** - Original code from user
- **test_cases/ice_minimal.cpp** - Minimal reproducer
- **test_cases/WORKAROUNDS.md** - Alternative solutions
- **test_cases/test_ice.sh** - Automated test script
- **test_cases/ice_reductions/** - All 16 reduction attempts

## 🧪 Testing
```bash
cd test_cases
./test_ice.sh
```

## 📋 Key Requirements for ICE
All of these are needed for the ICE to occur:
1. Template variable with type parameter
2. Template struct with `auto` non-type parameter
3. Function with `auto` parameter
4. `using` alias for Type<lambda>
5. Lambda with `auto` parameter inside Type<>
6. Second lambda using template variable with alias as template arg

Removing ANY element prevents the ICE.

## 🐛 Report to GCC
This should be reported to GCC Bugzilla at https://gcc.gnu.org/bugzilla/

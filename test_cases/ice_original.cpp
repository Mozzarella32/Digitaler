// Original code that triggers ICE in g++-14
// Compiles successfully with g++-12 and g++-13
// Compile with: g++-14 -std=c++23 ice_original.cpp

template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

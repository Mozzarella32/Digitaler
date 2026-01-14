// Minimal reproducer for g++-14 ICE
// This is the smallest code that still triggers the ICE
// Compile with: g++-14 -std=c++23 ice_minimal.cpp

template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }

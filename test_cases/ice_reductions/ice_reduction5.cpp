// Reduction 5: Keep the key combination - remove lambda parameter
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  return []() { return zero<inner>; }();
}

int main() { outer(1); }

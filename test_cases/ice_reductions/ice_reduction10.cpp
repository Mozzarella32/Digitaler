// Reduction 10: Minimal version - remove return from Type lambda
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) { 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

// Reduction 1: Simplify the outer function parameter
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer() {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(); }

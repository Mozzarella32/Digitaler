// Reduction 15: Test with concrete type for outer
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(int) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }

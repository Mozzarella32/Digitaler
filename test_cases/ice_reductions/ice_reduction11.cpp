// Reduction 11: Even more minimal
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

// Reduction 16: Test with concrete type for inner lambda's auto param
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](int) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }

// Reduction 9: Simplify return values but keep auto parameter
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) {}>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

// Reduction 3: Simplify zero to be non-template
template <auto> struct Type;
constexpr int zero = 1;

int outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return zero; }(1);
}

int main() { outer(1); }

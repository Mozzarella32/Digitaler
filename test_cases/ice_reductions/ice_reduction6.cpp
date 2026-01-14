// Reduction 6: Simplify lambda body in Type
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[]() { return 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

// Reduction 7: Simplify lambda body in Type and second lambda
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[]() {}>;
  return [](auto) { return zero<inner>; }(1);
}

int main() { outer(1); }

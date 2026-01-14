// Reduction 2: Remove outer function entirely
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int main() {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return zero<inner>; }(1);
}

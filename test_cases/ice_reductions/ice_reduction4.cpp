// Reduction 4: Remove zero variable entirely
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[](auto) { return 1; }>;
  return [](auto) { return 1; }(1);
}

int main() { outer(1); }

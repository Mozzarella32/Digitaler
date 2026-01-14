// Reduction 14: Test if we need the template variable zero
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { inner{}; }(1);
}

int main() { outer(1); }

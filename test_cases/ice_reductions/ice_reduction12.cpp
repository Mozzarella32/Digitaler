// Reduction 12: Absolute minimal - replace zero usage with simpler expression
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  using inner = Type<[](auto) {}>;
  [](auto) { zero<inner>; }(1);
}

int main() { outer(1); }

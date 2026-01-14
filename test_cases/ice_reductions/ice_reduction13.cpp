// Reduction 13: Try without using alias
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

void outer(auto) {
  [](auto) { zero<Type<[](auto) {}>>; }(1);
}

int main() { outer(1); }

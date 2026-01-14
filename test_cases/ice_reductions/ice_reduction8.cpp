// Reduction 8: Change zero to use inner as template argument
template <typename T> constexpr int zero = 1;
template <auto> struct Type;

int outer(auto) {
  using inner = Type<[]() {}>;
  return []() { return zero<inner>; }();
}

int main() { outer(1); }

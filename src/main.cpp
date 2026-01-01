#include <iostream>

#include "inspect.hh"

struct A {};
struct B {};
struct C {};

int main() {
  std::cout << "Hello, World!" << std::endl;

  std::variant<A, B, C> my_variant;
  my_variant = C{};

  std::cout << "It was an: "
            << Inspect<char>(
                   my_variant, [](A value) { return 'A'; },
                   [](B value) { return 'B'; }, [](C value) { return 'C'; })
            << std::endl;

  std::optional<int> my_opt = 42;

  Inspect(
      my_opt, [](int value) { std::cout << "Value: " << value << std::endl; },
      []() { std::cout << "No Value" << std::endl; });

  my_opt = std::nullopt;
  std::cout << "Optional contains: "
            << Inspect<std::string>(
                   my_opt,
                   [](int value) { return "Value: " + std::to_string(value); },
                   []() { return "No Value"; })
            << std::endl;
  return 0;
}

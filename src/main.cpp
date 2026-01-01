#include "algebraic_datatype/algebraic_datatype.hh"
#include <iostream>

using namespace algebraic_datatype;
;

int main() {
  std::cout << "Hello, World!" << std::endl;

  std::variant<int, double, float> my_variant;
  my_variant = 42;

  Inspect(
      my_variant,
      [](int value) { std::cout << "Integer: " << value << std::endl; },
      // [](double value) { std::cout << "Double: " << value << std::endl; },
      [](float value) { std::cout << "Float: " << value << std::endl; });

  return 0;
}
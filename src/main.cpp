/**
 * @file main.cpp
 * @author Bartosz Ksel (bartoszmateusz.ksel@gmail.com)
 * @brief Reprezentacja działania funkcji Inspect dla typów algebraicznych,
 *        w tymL std::variant, std::optional oraz Result.
 *        `Result<T, E>` jest prostą implementacją wzorowaną na std::expected
 *        z C++23.
 *        Implementacja Inspect zapewnia w miarę czytelne komunikaty błędów
 *        w przypadku niepełnych handlerów (nieobsłużonych wariantów).
 *        Przystosowano to do standardu C++17, przez to ekstensywnie wykorzystywane
 *        są SFINAE zamiast konceptów. Dlatego użyto metody szablonów-pułapek,
 *        aby wymusić wyświetlanie komunikatów błędów tylko przy instancjalizacji
 *        funkcji Inspect, a nie wypluwać błędy ze środka implementacji.
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>

#include "inspect.hh"
#include "result.hh"

struct A {};
struct B {};
struct C {};

enum class ErrorCode { ERROR_ONE, ERROR_TWO };

void test_variant();
void test_optional();
void test_result();

int main() {
  std::cout << "Hello, World!" << std::endl;

  test_variant();
  test_optional();
  test_result();

  return 0;
}

void test_variant() {

  std::cout << "Testing Variant Inspect:" << std::endl;

  std::variant<A, B, C> my_variant;
  my_variant = C{};

  auto article = Inspect<std::string_view>(
      my_variant, [](A value) { return "an"; }, [](auto value) { return "a"; });

  std::cout << "It was " << article << ": "
            << Inspect<char>(
                   my_variant, [](A value) { return 'A'; },
                   [](B value) { return 'B'; }, [](C value) { return 'C'; })
            << std::endl;
}

void test_optional() {

  std::cout << "Testing Optional Inspect:" << std::endl;
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
}

void test_result() {
  std::cout << "Testing Result Inspect:" << std::endl;

  Result<int, ErrorCode> my_result = 100;
  std::cout << "Result contains: "
            << Inspect<std::string>(
                   my_result,
                   [](int value) { return "Value: " + std::to_string(value); },
                   [](ErrorCode err) {
                     return "Error: " + std::to_string(static_cast<int>(err));
                   })
            << std::endl;

  my_result = ErrorCode::ERROR_ONE;
  std::cout << "Result contains: ";
  Inspect(
      my_result,
      [](int value) { std::cout << "Value: " << value << std::endl; },
      [](ErrorCode err) {
        std::cout << "Error: " << static_cast<int>(err) << std::endl;
      });
}

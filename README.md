# C++ Algebraic Data Types

This repository contains a library-based solution for the lack of pattern matching in C++. It introduces an `Inspect` function, which can be used similarly to a `switch` statement, with the distinction that it is an expression, not a statement (`switch` cannot return a value). A custom implementation of `std::expected` compatible with C++17 has also been added. The `Inspect` function also uses clearer error logging (it explicitly states if a case handler definition was forgotten).

## Example

The `std::variant` is really useful class, but not really ergonomic to work with. It requires usage of `std::visit` with properly defined visitors. With this library, you could use it similarly to Rust's `match` or C's `switch`. A snippet utilizing `adt::Inspect` as pattern matcher is shown below:

```c++
void test_variant() {

  std::cout << "Testing Variant Inspect:" << std::endl;

  std::variant<A, B, C> my_variant;
  my_variant = C{};

  auto article = adt::Inspect<std::string_view>(
      my_variant, [](A value) { return "an"; }, [](auto value) { return "a"; });

  std::cout << "It was " << article << ": "
            << adt::Inspect<char>(
                   my_variant,
                   [](A value) { return 'A'; },
                   [](B value) { return 'B'; },
                   [](C value) { return 'C'; })
            << std::endl;
}
```

Another important class that might need to be analysed in similar fashion is `std::optional`. Here the task is simpler -- you only need to handle two cases. An example below:
```c++
void test_optional() {

  std::cout << "Testing Optional Inspect:" << std::endl;
  std::optional<int> my_opt = 42;

  adt::Inspect(
      my_opt, [](int value) { std::cout << "Value: " << value << std::endl; },
      []() { std::cout << "No Value" << std::endl; });

  my_opt = std::nullopt;
  std::cout << "Optional contains: "
            << adt::Inspect<std::string>(
                   my_opt,
                   [](int value) { return "Value: " + std::to_string(value); },
                   []() { return "No Value"; })
            << std::endl;
}
```

The C++17 standard library does not have more algebraic datatypes in standard. C++23 added `std::expected`, but it is relatively new feature that will not be used by many contemporary codebases as it will take few more years to adopt the newer standard. So this library provides conceptually similar solution inspired by Rust's `Result`, it is called `adt::Result`. It is slightly more verbose when it comes to specifying if a argument is an error or correct value than `std::expected`. A usage example was provided below:

```c++
void test_result() {
  std::cout << "Testing Result Inspect:" << std::endl;

  adt::Result<int, ErrorCode> my_result = adt::Ok(100);
  std::cout << "Result contains: "
            << adt::Inspect<std::string>(
                   my_result,
                   [](int value) { return "Value: " + std::to_string(value); },
                   [](ErrorCode err) {
                     return "Error: " + std::to_string(static_cast<int>(err));
                   })
            << std::endl;

  my_result = adt::Error(ErrorCode::ERROR_TWO);
  std::cout << "Result contains: ";
  adt::Inspect(
      my_result,
      [](int value) { std::cout << "Value: " << value << std::endl; },
      [](ErrorCode err) {
        std::cout << "Error: " << static_cast<int>(err) << std::endl;
      });

  // Test with same type for Value and Error, now handling Ok<T> and Error<E>
  // directly in order to avoid ambiguity
  adt::Result<int, int> another_result = adt::Ok(55);
  std::cout << "Another Result contains: "
            << adt::Inspect<std::string>(
                   another_result,
                   [](adt::Ok<int> value) {
                     return "Value: " + std::to_string(value.get());
                   },
                   [](adt::Error<int> err) {
                     return "Error: " + std::to_string(err.get());
                   })
            << std::endl;
}

```

## Building and Running the example

Two build methods are provided for this example.

### _Meson_ Configuration

You must have _Meson_ and _just_ (for the Justfile) installed. To run, execute:
```bash
just
```

That's it.

### _Makefile_ Configuration

This project is simple, so creating a _makefile_ was not much trouble. An _LLVM_ compiler configuration was chosen because it is slightly more portable than _GCC_. Therefore, you need _make_ and _clang++_ for the compilation itself.

**NOTE**: By modifying the `Makefile` and uncommenting `# CXX=g++`, you can change the compiler to GCC.

To run the program, execute the command:
```bash
make
```
That's it.
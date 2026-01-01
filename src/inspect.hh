/**
 * @file inspect.hh
 * @author Bartosz Ksel (bartoszmateusz.ksel@gmail.com)
 * @brief Provides the Inspect function for algebraic data types such as
 *        std::variant, std::optional, and Expected/Result types.
 * @version 0.1
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

namespace algebraic_type {

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace detail {
struct deduce_return_type {};

/**
 * @brief Removes const, volatile, and reference qualifiers from a type.
 */
template <typename T> struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;

} // namespace detail

namespace diagnostic {

// Helper, żeby static_assert nie odpalał się zawsze, a tylko przy
// instancjalizacji
template <typename T> struct always_false : std::false_type {};

// Twoja struktura błędu - użyjemy jej do wyświetlenia typu
template <typename T> struct MISSING_HANDLER_FOR_TYPE {
  static constexpr bool value = false;
};

template <typename Visitor, typename Variant> struct variant_validator;

template <typename Visitor, typename... Alts>
struct variant_validator<Visitor, std::variant<Alts...>> {

  static constexpr void validate() {
    // Fold expression (C++17) - perform check for each type in Alts...
    (check_type<Alts>(), ...);
  }

private:
  template <typename T> static constexpr void check_type() {
    constexpr bool is_handled = std::is_invocable_v<Visitor, T>;

    if constexpr (!is_handled) {
      static_assert(
          always_false<T>::value,
          "ALGEBRAIC ERROR: One of the variant types is missing a handler.");
      static_assert(MISSING_HANDLER_FOR_TYPE<T>::value,
                    "Missing handler -->: ");
    }
  }
};

template <typename T> struct MISSING_HANDLER_FOR_NONE {
  static constexpr bool value = false;
};

template <typename Visitor, typename Opt> struct optional_validator {

  static constexpr void validate() {
    using ValueType = typename detail::remove_cvref_t<Opt>::value_type;

    constexpr bool handles_value = std::is_invocable_v<Visitor, ValueType>;

    if constexpr (!handles_value) {
      static_assert(always_false<ValueType>::value,
                    "ALGEBRAIC ERROR: Inspect for optional does not handle the "
                    "value type!");
      static_assert(MISSING_HANDLER_FOR_TYPE<ValueType>::value,
                    "Missing handler for type ->");
    }

    constexpr bool handles_none = std::is_invocable_v<Visitor>;

    if constexpr (!handles_none) {
      static_assert(always_false<Opt>::value,
                    "ALGEBRAIC ERROR: Inspect for optional does not handle the "
                    "empty state (None)!");
      static_assert(MISSING_HANDLER_FOR_NONE<Opt>::value,
                    "You need to provide no-argument lambda: []() { ... }");
    }
  }
};

template <typename Visitor, typename Exp> struct expected_validator {
  static constexpr void validate() {

    using ValT =
        typename detail::remove_cvref_t<decltype(std::declval<Exp>().value())>;
    using ErrT =
        typename detail::remove_cvref_t<decltype(std::declval<Exp>().error())>;

    constexpr bool handles_value = std::is_invocable_v<Visitor, ValT>;
    if constexpr (!handles_value) {
      static_assert(always_false<ValT>::value,
                    "ALGEBRAIC ERROR: Inspect for Result does not handle the "
                    "success type!");
      static_assert(MISSING_HANDLER_FOR_TYPE<ValT>::value,
                    "Missing handler for value ->");
    }

    constexpr bool handles_error = std::is_invocable_v<Visitor, ErrT>;
    if constexpr (!handles_error) {
      static_assert(always_false<ErrT>::value,
                    "ALGEBRAIC ERROR: Inspect for Result does not handle the "
                    "error type!");

      static_assert(MISSING_HANDLER_FOR_TYPE<ErrT>::value,
                    "Missing handler for error ->");
    }
  }
};

} // namespace diagnostic

namespace traits {
// --- Detector of std::variant ---
template <typename T> struct is_variant_impl : std::false_type {};
template <typename... Args>
struct is_variant_impl<std::variant<Args...>> : std::true_type {};

template <typename T>
struct is_variant : is_variant_impl<detail::remove_cvref_t<T>> {};

// --- Detector of std::optional ---
template <typename T> struct is_optional_impl : std::false_type {};
template <typename T>
struct is_optional_impl<std::optional<T>> : std::true_type {};

template <typename T>
struct is_optional : is_optional_impl<detail::remove_cvref_t<T>> {};

// --- Detector of Expected/Result ---
// We check for: has_value(), value(), error(), as in C++23 std::expected
template <typename T, typename = void>
struct is_expected_impl : std::false_type {};

template <typename T>
struct is_expected_impl<T, std::void_t<decltype(std::declval<T>().has_value()),
                                       decltype(std::declval<T>().value()),
                                       decltype(std::declval<T>().error())>>
    : std::true_type {};

template <typename T>
struct is_expected
    : std::integral_constant<
          bool, is_expected_impl<detail::remove_cvref_t<T>>::value &&
                    !is_optional<T>::value> {};

// Helper do sprawdzenia wyczerpalności Varianta
template <typename Visitor, typename Variant>
struct is_exhaustive_variant : std::false_type {};

template <typename Visitor, typename... Alts>
struct is_exhaustive_variant<Visitor, std::variant<Alts...>> {
  // C++17 Fold Expression: (is_invocable && ... && is_invocable)
  static constexpr bool value = (std::is_invocable_v<Visitor, Alts> && ...);
};

// Helper do sprawdzenia wyczerpalności Optionala
template <typename Visitor, typename Opt> struct is_exhaustive_optional {
  using ValueType = typename detail::remove_cvref_t<Opt>::value_type;
  static constexpr bool value =
      std::is_invocable_v<Visitor, ValueType> && // Obsługa wartości
      std::is_invocable_v<Visitor>;              // Obsługa None
};

// Helper do sprawdzenia Result/Expected
template <typename Visitor, typename Exp> struct is_exhaustive_expected {
  // Używamy declval, żeby udawać obiekty
  using ValT = decltype(std::declval<Exp>().value());
  using ErrT = decltype(std::declval<Exp>().error());

  static constexpr bool value =
      std::is_invocable_v<Visitor, ValT> && std::is_invocable_v<Visitor, ErrT>;
};
} // namespace traits

/**
 * @brief Inspects a std::variant and applies the appropriate lambda based on
 * the active type.
 *
 * @param R The return type. If not specified, it is deduced.
 * @param Variant The variant type to inspect.
 * @param lambdas The lambdas corresponding to each type in the variant.
 * @return The result of the invoked lambda, either wrapped in R or deduced.
 *
 * @warning This function assumes that the number of lambdas provided matches
 *          all the types in the variant. If not, it will result in a
 *          compile-time error.
 *
 * @details This template implements additional compile-time checking to provide
 *          better error messages when the number of lambdas does not match the
 *          number of types in the variant. It uses static_assert to enforce
 * this constraint.
 *
 * @note This function might be used as an expression or a statement, depending
 *       on whether the return type R is specified or deduced. Here is an
 *       example:
 * ```cpp
 * void test_variant() {
 *     std::variant<A, B, C> my_variant;
 *     my_variant = C{};
 *
 *     auto article = Inspect<std::string_view>(
 *         my_variant,
 *         [](A value) { return "an"; },
 *         [](auto value) { return "a"; });
 *
 *     std::cout << "It was " << article << ": "
 *               << Inspect<char>(
 *                      my_variant,
 *                      [](A value) { return 'A'; },
 *                      [](B value) { return 'B'; },
 *                      [](C value) { return 'C'; })
 *               << std::endl;
 * }
 * ```
 */
template <typename R = detail::deduce_return_type,
          typename Variant, // Correct version
          typename... Lambdas,
          // Condition 1: It is a variant
          std::enable_if_t<traits::is_variant<Variant>::value, int> = 0,
          // Condition 2: Handlers are complete (IsExhaustive == true)
          std::enable_if_t<traits::is_exhaustive_variant<
                               overloaded<detail::remove_cvref_t<Lambdas>...>,
                               detail::remove_cvref_t<Variant>>::value,
                           int> = 0>
[[nodiscard]]
constexpr auto Inspect(Variant &&variant, Lambdas &&...lambdas) {
  // It is safe to proceed
  if constexpr (std::is_same_v<R, detail::deduce_return_type>) {
    return std::visit(overloaded{std::forward<Lambdas>(lambdas)...},
                      std::forward<Variant>(variant));
  } else {
    return R{std::visit(overloaded{std::forward<Lambdas>(lambdas)...},
                        std::forward<Variant>(variant))};
  }
}

// --- SFINAE failure version: Handlers are INCOMPLETE (IsExhaustive == false)
template <typename R = detail::deduce_return_type, typename Variant,
          typename... Lambdas,
          // Condition 1: It is a variant
          std::enable_if_t<traits::is_variant<Variant>::value, int> = 0,
          // Condition 2: Handlers are INCOMPLETE (IsExhaustive == false)
          std::enable_if_t<!traits::is_exhaustive_variant< // <--- Consider
                                                           // exclamation mark
                               overloaded<detail::remove_cvref_t<Lambdas>...>,
                               detail::remove_cvref_t<Variant>>::value,
                           int> = 0>
constexpr void Inspect(Variant &&, Lambdas &&...) {
  // It is used to display the error

  using VisitorType = overloaded<detail::remove_cvref_t<Lambdas>...>;
  using RawVariant = detail::remove_cvref_t<Variant>;

  diagnostic::variant_validator<VisitorType, RawVariant>::validate();
}

/**
 * @brief Inspects a std::optional and applies the appropriate lambda based on
 * whether it contains a value or not.
 *
 * @param R The return type. If not specified, it is deduced.
 * @param Opt The optional type to inspect.
 * @param lambdas The lambdas for the value and no-value cases.
 * @return The result of the invoked lambda, either wrapped in R or deduced.
 *
 * @warning This function assumes that at least one lambda is provided: one for
 * the value case and optionally one for the no-value case. If not, it will
 *          result in a compile-time error.
 *
 * @note This function might be used as an expression or a statement, depending
 *      on whether the return type R is specified or deduced. Here is an
 * example:
 * ```cpp
 * void test_optional() {
 *     std::optional<int> my_opt = 42;
 *     Inspect(
 *         my_opt,
 *         [](int value) { std::cout << "Value: " << value << std::endl; },
 *         []() { std::cout << "No Value" << std::endl; });
 *
 *     my_opt = std::nullopt;
 *     std::cout << "Optional contains: "
 *               << Inspect<std::string>(
 *                      my_opt,
 *                      [](int value) { return "Value: " +
 * std::to_string(value); },
 *                      []() { return "No Value"; })
 *               << std::endl;
 * }
 * ```
 */
template <typename R = detail::deduce_return_type, typename Opt,
          typename... Lambdas,
          std::enable_if_t<traits::is_optional<Opt>::value, int> = 0,
          // Checking exhaustiveness
          std::enable_if_t<traits::is_exhaustive_optional<
                               overloaded<detail::remove_cvref_t<Lambdas>...>,
                               detail::remove_cvref_t<Opt>>::value,
                           int> = 0>
[[nodiscard]]
constexpr auto Inspect(Opt &&opt, Lambdas &&...lambdas) noexcept {

  using VisitorType = overloaded<detail::remove_cvref_t<Lambdas>...>;

  using RawOpt = detail::remove_cvref_t<Opt>;
  diagnostic::optional_validator<VisitorType, RawOpt>::validate();

  auto visitor = VisitorType{std::forward<Lambdas>(lambdas)...};

  constexpr bool has_explicit_return_type =
      !std::is_same_v<R, detail::deduce_return_type>;

  if (opt) {
    if constexpr (has_explicit_return_type) {
      return R{visitor(*std::forward<Opt>(opt))};
    } else {
      return visitor(*std::forward<Opt>(opt));
    }
  } else {
    if constexpr (has_explicit_return_type) {
      return R{visitor()};
    } else {
      return visitor();
    }
  }
}

// --- SFINAE failure version: Handlers are INCOMPLETE (IsExhaustive == false)
template <typename R = detail::deduce_return_type, typename Opt,
          typename... Lambdas,
          std::enable_if_t<traits::is_optional<Opt>::value, int> = 0,
          // Checking LACK of exhaustiveness
          std::enable_if_t<!traits::is_exhaustive_optional< // <--- NEGACJA
                               overloaded<detail::remove_cvref_t<Lambdas>...>,
                               detail::remove_cvref_t<Opt>>::value,
                           int> = 0>
constexpr void Inspect(Opt &&, Lambdas &&...) {
  // Diagnostyka
  using VisitorType = overloaded<detail::remove_cvref_t<Lambdas>...>;
  using RawOpt = detail::remove_cvref_t<Opt>;
  diagnostic::optional_validator<VisitorType, RawOpt>::validate();
}

/**
 * @brief Inspects an Expected/Result type and applies the appropriate lambda
 * based on whether it contains a value or an error.
 *
 * @param R The return type. If not specified, it is deduced.
 * @param Exp The Expected/Result type to inspect.
 * @param lambdas The lambdas for the value and error cases.
 * @return The result of the invoked lambda, either wrapped in R or deduced.
 *
 * @warning This function assumes that at least one lambda is provided: one for
 * the value case and optionally ontemplate <
    typename R = detail::deduce_return_type, typename Opt, typename... Lambdas,
    std::enable_if_t<
        traits::is_optional<Opt>::value && (sizeof...(Lambdas) >= 1), int> = 0>
[[nodiscard]]e for the error case. If not, it will result
 * in a compile-time error.
 *
 * @note This function might be used as an expression or a statement, depending
 *      on whether the return type R is specified or deduced. Here is an
 * example:
 * ```cpp
 * void test_result() {
 *     Result<int, ErrorCode> my_result = 42;
 *
 *     Inspect(
 *         my_result,
 *         [](int value) { std::cout << "Value: " << value << std::endl; },
 *         [](ErrorCode err) { std::cout << "Error occurred: " <<
 * static_cast<int>(err) << std::endl; });
 *
 *     my_result = ErrorCode::ERROR_ONE;
 *     std::cout << "Result contains: "
 *               << Inspect<std::string>(
 *                      my_result,
 *                      [](int value) { return "Value: " +
 * std::to_string(value); },
 *                      [](ErrorCode err) { return "Error occurred: " +
 * std::to_string(static_cast<int>(err)); })
 *               << std::endl;
 * }
 * ```
 */
template <typename R = detail::deduce_return_type, typename Exp,
          typename... Lambdas,
          // 1. Is it expected(type)?
          std::enable_if_t<traits::is_expected<Exp>::value, int> = 0,
          // 2. Is Exhaustive? (Yes)
          std::enable_if_t<
              traits::is_exhaustive_expected<
                  overloaded<detail::remove_cvref_t<Lambdas>...>, Exp>::value,
              int> = 0>
[[nodiscard]]
constexpr auto Inspect(Exp &&exp, Lambdas &&...lambdas) noexcept {

  auto visitor = overloaded{std::forward<Lambdas>(lambdas)...};
  constexpr bool has_explicit_return_type =
      !std::is_same_v<R, detail::deduce_return_type>;

  if (exp.has_value()) {
    if constexpr (has_explicit_return_type) {
      return R{visitor(std::forward<Exp>(exp).value())};
    } else {
      return visitor(std::forward<Exp>(exp).value());
    }
  } else {
    if constexpr (has_explicit_return_type) {
      return R{visitor(std::forward<Exp>(exp).error())};
    } else {
      return visitor(std::forward<Exp>(exp).error());
    }
  }
}

// --- SFINAE failure version: Handlers are INCOMPLETE (IsExhaustive == false)
template <typename R = detail::deduce_return_type, typename Exp,
          typename... Lambdas,
          std::enable_if_t<traits::is_expected<Exp>::value, int> = 0,
          std::enable_if_t<
              !traits::is_exhaustive_expected<
                  overloaded<detail::remove_cvref_t<Lambdas>...>, Exp>::value,
              int> = 0>
constexpr void Inspect(Exp &&, Lambdas &&...) {
  using VisitorType = overloaded<detail::remove_cvref_t<Lambdas>...>;

  diagnostic::expected_validator<VisitorType, Exp>::validate();
}

} // namespace algebraic_type

#ifdef Inspect
#error                                                                         \
    "Someone has defined macro 'Inspect'! This is a reserved name for the ADT library."
#endif

/**
 * @brief Alias for algebraic_type::Inspect function.
 * @see algebraic_type::Inspect
 *
 * @note This alias is provided for convenience to avoid typing the full
 *       namespace each time. It has three overloads to handle std::variant,
 *       std::optional, and Expected/Result types.
 */
using algebraic_type::Inspect;
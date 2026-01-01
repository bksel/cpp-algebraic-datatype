#pragma once

#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

namespace algebraic_type {

namespace traits {

// --- Detektor dla std::variant ---
template <typename T> struct is_variant_impl : std::false_type {};

template <typename... Args>
struct is_variant_impl<std::variant<Args...>> : std::true_type {};

template <typename T>
struct is_variant : is_variant_impl<std::remove_cvref_t<T>> {};

template <typename T>
concept VariantType = is_variant<T>::value;

// --- Detektor dla std::optional ---
template <typename T> struct is_optional_impl : std::false_type {};

template <typename T>
struct is_optional_impl<std::optional<T>> : std::true_type {};

template <typename T>
struct is_optional : is_optional_impl<std::remove_cvref_t<T>> {};

template <typename T>
concept OptionalType = is_optional<T>::value;
} // namespace traits

namespace detail {

// --- Helpers ---
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct deduce_return_type {};

// --- Diagnostyka ---

// 1. Sprawdzanie wyczerpalności (czy lambda istnieje?)
template <typename Visitor, typename Variant>
struct is_exhaustive_for_variant : std::false_type {};

template <typename Visitor, typename... Alts>
struct is_exhaustive_for_variant<Visitor, std::variant<Alts...>> {
  static constexpr bool value = (std::invocable<Visitor, Alts> && ...);
};

template <typename Variant, typename... Lambdas>
concept MatchesAllVariants =
    is_exhaustive_for_variant<overloaded<std::remove_cvref_t<Lambdas>...>,
                              std::remove_cvref_t<Variant>>::value;

template <typename T> struct MISSING_HANDLER_FOR_TYPE {
  static constexpr bool value = false;
};

// 2. NOWOŚĆ: Sprawdzanie poprawności typu zwracanego
// Służy tylko do wyświetlenia ładnego błędu
template <typename SourceT, typename ActualRet, typename ExpectedRet>
struct INVALID_RETURN_TYPE {
  static constexpr bool value = false;
};

} // namespace detail

// --- Inspect ---

using traits::OptionalType;
using traits::VariantType;

template <typename R = detail::deduce_return_type, typename Variant,
          typename... Lambdas>
  requires VariantType<Variant> && (sizeof...(Lambdas) >= 1)
constexpr decltype(auto) Inspect(Variant &&variant, Lambdas &&...lambdas) {
  using namespace detail;

  using VariantType = std::remove_cvref_t<Variant>;
  using VisitorType = overloaded<std::remove_cvref_t<Lambdas>...>;

  // FAZA 1: Sprawdzenie wyczerpalności (Exhaustiveness)
  if constexpr (!MatchesAllVariants<VariantType, Lambdas...>) {
    auto diagnose_missing = []<typename... Alts>(std::variant<Alts...> *) {
      (
          [&]() {
            if constexpr (!std::invocable<VisitorType, Alts>) {
              static_assert(MISSING_HANDLER_FOR_TYPE<Alts>::value,
                            "ALGEBRAIC ERROR: Nie obsłużyłeś wariantu! (Typ w "
                            "argumencie szablonu)");
            }
          }(),
          ...);
    };
    diagnose_missing(static_cast<VariantType *>(nullptr));
  } else {
    // FAZA 2: Sprawdzenie typów zwracanych (Return Type Safety)
    // Wykonujemy tylko, jeśli user podał konkretny typ R (nie
    // deduce_return_type)
    if constexpr (!std::is_same_v<R, deduce_return_type>) {

      auto diagnose_return_type = []<typename... Alts>(
                                      std::variant<Alts...> *) {
        (
            [&]() {
              // Jaki typ zwraca visitor dla danego typu wariantu?
              using ActualRet = std::invoke_result_t<VisitorType, Alts>;

              // Czy ten typ konwertuje się do R?
              // Uwaga: void do void jest OK. int do void jest OK. void do int
              // NIE jest OK.
              constexpr bool is_valid =
                  std::is_convertible_v<ActualRet, R> || (std::is_void_v<R>);

              if constexpr (!is_valid) {
                static_assert(INVALID_RETURN_TYPE<Alts, ActualRet, R>::value,
                              "ALGEBRAIC ERROR: Lambda zwraca typ niezgodny z "
                              "zadeklarowanym w Inspect<T>!");
              }
            }(),
            ...);
      };
      diagnose_return_type(static_cast<VariantType *>(nullptr));
    }

    // FAZA 3: Wykonanie
    if constexpr (std::is_same_v<R, deduce_return_type>) {
      return std::visit(overloaded{std::forward<Lambdas>(lambdas)...},
                        std::forward<Variant>(variant));
    } else {
      return std::visit<R>(overloaded{std::forward<Lambdas>(lambdas)...},
                           std::forward<Variant>(variant));
    }
  }
}

// --- NOWA Sekcja dla std::optional ---

// Inspect dla Optionala
// Wymagamy DOKŁADNIE dwóch obsługiwanych ścieżek:
// 1. Dla wartości T
// 2. Dla pustego stanu (void)
template <typename R = detail::deduce_return_type, typename Opt,
          typename... Lambdas>
  requires OptionalType<Opt> && (sizeof...(Lambdas) >= 1)
auto Inspect(Opt &&opt, Lambdas &&...lambdas) -> decltype(auto) {
  using namespace detail;

  constexpr bool has_explicit_return_type =
      !std::is_same_v<R, detail::deduce_return_type>;

//   using RawOpt = std::remove_cvref_t<Opt>;
//   using ValueType = typename RawOpt::value_type; // Teraz to zadziała!

  // Tworzymy wizytora z dostarczonych lambd
  auto visitor = overloaded{std::forward<Lambdas>(lambdas)...};

  if (opt.has_value()) {
    // Ścieżka "Some": wywołujemy wizytora z wartością w środku
    if constexpr (has_explicit_return_type) {
      // Jeśli użytkownik wymusił typ R (np. std::string), rzutujemy wynik
      // lambdy na R. Dzięki temu "const char*" zamieni się w "std::string".
      return R{visitor(*std::forward<Opt>(opt))};
    } else {
      return visitor(*std::forward<Opt>(opt));
    }
  } else {
    // Ścieżka "None": wywołujemy wizytora bez argumentów
    // Jeśli użytkownik nie dostarczył lambdy [](){}, kod się nie skompiluje!
    if constexpr (has_explicit_return_type) {
      // To samo tutaj. "const char*" -> std::string
      return R{visitor()};
    } else {
      return visitor();
    }
  }
}

} // namespace algebraic_type

#ifdef Inspect
#error                                                                         \
    "Ktoś zdefiniował makro 'Inspect'! To nazwa zastrzeżona dla biblioteki ADT."
#endif

using algebraic_type::Inspect;
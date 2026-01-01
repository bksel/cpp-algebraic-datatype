#pragma once

#include <concepts>
#include <type_traits>
#include <variant>

namespace algebraic_datatype {

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// Krok 1: Trait, który rozbiera Variant na czynniki pierwsze
// Domyślnie fałsz (dla typów niebędących wariantami)
template <typename Visitor, typename Variant>
struct is_exhaustive_for_variant : std::false_type {};

// Specjalizacja dla std::variant<Alts...>
template <typename Visitor, typename... Alts>
struct is_exhaustive_for_variant<Visitor, std::variant<Alts...>> {
  // Tu jest serce algorytmu: Fold Expression (&& ...)
  // Sprawdzamy, czy Visitor może być wywołany dla każdego typu z Alts
  static constexpr bool value = (std::invocable<Visitor, Alts> && ...);
};

// Krok 2: Koncept C++20
template <typename Variant, typename... Lambdas>
concept MatchesAllVariants = is_exhaustive_for_variant<
    overloaded<std::remove_cvref_t<Lambdas>...>, // Symulujemy typ wizytora
    std::remove_cvref_t<Variant> // Oczyszczamy typ wariantu (usuwamy & i const)
    >::value;

// Krok 3: Inspect z poprawnym Requiresem
template <typename Variant, typename... Lambdas>
  requires MatchesAllVariants<Variant, Lambdas...>
decltype(auto) Inspect(Variant &&variant, Lambdas &&...lambdas) {
  return std::visit(overloaded{std::forward<Lambdas>(lambdas)...},
                    std::forward<Variant>(variant));
}

} // namespace algebraic_datatype
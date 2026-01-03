#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

#include <cstdlib>

/**
 * @brief A simple Result type representing either a value of type T or an
 * error of type E.
 *
 * @note This is an implementation aiming to mimic the behavior of
 * Expected/Result type std::expected introduced in C++23.
 *
 * @tparam T The type of the value.
 * @tparam E The type of the error.
 */
template <typename T, typename E> class Result {
  std::variant<T, E> _data;

public:
  constexpr Result(T val) : _data(std::move(val)) {}
  constexpr Result(E err) : _data(std::move(err)) {}

  [[nodiscard]]
  constexpr bool has_value() const noexcept {
    return std::holds_alternative<T>(_data);
  }
  [[nodiscard]]
  constexpr bool has_error() const noexcept {
    return std::holds_alternative<E>(_data);
  }

  [[nodiscard]]
  constexpr T &value() & {
    ensure_value();
    return std::get<T>(_data);
  }

  [[nodiscard]]
  constexpr const T &value() const & {
    ensure_value();
    return std::get<T>(_data);
  }

  [[nodiscard]]
  constexpr T &&value() && {
    ensure_value();
    return std::get<T>(std::move(_data));
  }

  [[nodiscard]]
  constexpr E &error() & {
    ensure_error();
    return std::get<E>(_data);
  }

  [[nodiscard]]
  constexpr const E &error() const & {
    ensure_error();
    return std::get<E>(_data);
  }

private:
  constexpr void ensure_value() const {
    if (!has_value()) {
      assert(false && "Result: Attempt to access value on error state!");

      std::abort();
    }
  }

  constexpr void ensure_error() const {
    if (!has_error()) {
      assert(false && "Result: Attempt to access error on success state!");
      std::abort();
    }
  }
};
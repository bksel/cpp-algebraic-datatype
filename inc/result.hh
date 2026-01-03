#include <cassert>
#include <type_traits>
#include <utility>
#include <variant>

#include <cstdlib>

namespace adt {

enum class Inspectability { Inspectable, NonInspectable };

static constexpr Inspectability NonInspectable = Inspectability::NonInspectable;

template <typename E> class Error {
  E error;

public:
  Error(E err) : error(std::move(err)) {}

  [[nodiscard]] constexpr E &get() & { return error; }
  [[nodiscard]] constexpr const E &get() const & { return error; }
  [[nodiscard]] constexpr E &&get() && { return std::move(error); }
};

template <typename T> class Ok {
  T value;

public:
  Ok(T val) : value(std::move(val)) {}

  [[nodiscard]] constexpr T &get() & { return value; }
  [[nodiscard]] constexpr const T &get() const & { return value; }
  [[nodiscard]] constexpr T &&get() && { return std::move(value); }
};

/**
 * @brief A simple Result type representing either a value of type T or an
 * error of type E.
 *
 * @note This is an implementation aiming to mimic the behavior of
 * Expected/Result type std::expected introduced in C++23.
 * 
 * @warning When T and E are the same type, the user must handle Ok<T> and
 *          Error<E> explicitly to avoid ambiguity. Otherwise, accessing value()
 *          or error() will provide the underlying T or E directly.
 *
 * @tparam T The type of the value.
 * @tparam E The type of the error.
 */
template <typename T, typename E> class Result {
  static_assert(!std::is_void_v<T>, "Value type T cannot be void.");
  static_assert(!std::is_void_v<E>, "Error type E cannot be void.");
  static_assert(!std::is_reference_v<T>, "Value type T cannot be a reference.");
  static_assert(!std::is_reference_v<E>, "Error type E cannot be a reference.");
  std::variant<Ok<T>, Error<E>> _data;

public:
  constexpr Result(Ok<T> val) : _data(std::move(val)) {}
  constexpr Result(Error<E> err) : _data(std::move(err)) {}

  [[nodiscard]]
  constexpr bool has_value() const noexcept {
    return std::holds_alternative<Ok<T>>(_data);
  }
  [[nodiscard]]
  constexpr bool has_error() const noexcept {
    return std::holds_alternative<Error<E>>(_data);
  }

  [[nodiscard]] constexpr decltype(auto) value() & {
    ensure_value();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Ok<T>>(_data); // Zwraca Ok<T>&
    } else {
      return std::get<Ok<T>>(_data).get(); // Zwraca T&
    }
  }

  [[nodiscard]] constexpr decltype(auto) value() const & {
    ensure_value();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Ok<T>>(_data); // Zwraca const Ok<T>&
    } else {
      return std::get<Ok<T>>(_data).get(); // Zwraca const T&
    }
  }

  [[nodiscard]] constexpr decltype(auto) value() && {
    ensure_value();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Ok<T>>(std::move(_data)); // Zwraca Ok<T>&&
    } else {
      return std::get<Ok<T>>(std::move(_data)).get(); // Zwraca T&&
    }
  }

  // --- ERROR (Analogicznie) ---

  [[nodiscard]] constexpr decltype(auto) error() & {
    ensure_error();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Error<E>>(_data);
    } else {
      return std::get<Error<E>>(_data).get();
    }
  }

  [[nodiscard]] constexpr decltype(auto) error() const & {
    ensure_error();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Error<E>>(_data);
    } else {
      return std::get<Error<E>>(_data).get();
    }
  }

  [[nodiscard]] constexpr decltype(auto) error() && {
    ensure_error();
    if constexpr (std::is_same_v<T, E>) {
      return std::get<Error<E>>(std::move(_data));
    } else {
      return std::get<Error<E>>(std::move(_data)).get();
    }
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

} // namespace adt
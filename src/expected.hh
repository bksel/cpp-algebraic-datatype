#pragma once

#include <variant>
#include <stdexcept>

// Prosty wrapper, który nadaje semantykę sukcesu/błędu
template<typename T, typename E>
class Result {
    std::variant<T, E> _data;

public:
    // Konstruktory (niejawne dla wygody)
    Result(T val) : _data(std::move(val)) {}
    Result(E err) : _data(std::move(err)) {}

    bool has_value() const { return std::holds_alternative<T>(_data); }

    // API wymagane przez nasz koncept ExpectedType:
    
    // Dla l-value
    T& value() { 
        if (!has_value()) throw std::logic_error("Accessing value of error result");
        return std::get<T>(_data); 
    }
    const T& value() const { 
        if (!has_value()) throw std::logic_error("Accessing value of error result");
        return std::get<T>(_data); 
    }

    // Dla r-value (move semantics) - ważne dla wydajności!
    T&& value() && {
        if (!has_value()) throw std::logic_error("Accessing value of error result");
        return std::get<T>(std::move(_data));
    }

    E& error() { 
        return std::get<E>(_data); 
    }
    const E& error() const { 
        return std::get<E>(_data); 
    }
};
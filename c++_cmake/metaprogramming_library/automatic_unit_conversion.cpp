#include <iostream>
#include <ostream>
#include <type_traits>
#include "functions.h"

namespace metaprogramming_library {

// Template programming is about letting the compiler deal with the types.
// Template metaprogramming is about letting the compiler calculate values.

// These are strong types for weights.
// The class automatically converts to the desired unit when it gets passed to a function.
// For example if a function expects a weight in grams,
// and the code passes a weight in kilograms,
// then the compiler automatically converts the passed unit to the expected unit.

// The factors to convert the given weight type to grams.
struct grams     { static constexpr float ms_factor = 1.0f; };
struct kilograms { static constexpr float ms_factor = 1000.0f; };

// This concept can be used to assure that a type is a weight type.
template<typename T>
concept weight_unit = std::is_same_v<T, grams> or std::is_same_v<T, kilograms>;

template <weight_unit U>
class Weight {
    float m_value {};
public:
    constexpr Weight() noexcept = default;
    // Constructor taking a float, so the weight is equal to the float passed.
    constexpr explicit Weight(decltype(m_value) v) noexcept : m_value(v) {}
    template <weight_unit UU>
    // This constructor is a template, the compiler generates multiple constructors,
    // 1. Create a kilogram from a gram.
    // 2. Create a kilogram from a kilogram.
    // 3. Create a gram from a kilogram.
    // 4. Create a gram from a gram.
    constexpr Weight(const Weight<UU>& s) noexcept {
        if constexpr (std::is_same_v<U, UU>)
            m_value = s.value();
        else
            m_value = s.value() * UU::ms_factor / U::ms_factor;
    }
    constexpr decltype(m_value) value() const noexcept { return m_value; }
    explicit constexpr operator decltype(m_value)() const noexcept { return m_value; }  // Static cast
    constexpr void value(const decltype(m_value) v) noexcept { m_value = v; }
    auto operator<=> (const Weight<U>&) const noexcept = default;

    // Allow for basic arithmetic operations:
    constexpr Weight<U>& operator+=(const Weight<U>& s) noexcept { m_value += s.value(); return *this; }
    constexpr Weight<U>& operator-=(const Weight<U>& s) noexcept { m_value -= s.value(); return *this; }
    constexpr Weight<U>& operator*=(const Weight<U>& s) noexcept { m_value *= s.value(); return *this; }
    constexpr Weight<U>& operator*=(const decltype(m_value) v) noexcept { m_value *= v; return *this; }
    constexpr Weight<U>& operator/=(const Weight<U>& s) noexcept { m_value /= s.value(); return *this; }
    constexpr Weight<U>& operator/=(const decltype(m_value) v) noexcept { m_value /= v; return *this; }
};

template<weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator+(Weight<UL> l, const Weight<UR> r) noexcept { return l += r; }
template<weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator-(Weight<UL> l, const Weight<UR> r) noexcept { return l -= r; }
template<weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator*(Weight<UL> l, const Weight<UR> r) noexcept { return l *= r; }
template<weight_unit U>
constexpr Weight<U> operator*(Weight<U> l, const float r) noexcept { return l *= r; }
template<weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator/(Weight<UL> l, const Weight<UR> r) noexcept { return l /= r; }
template<weight_unit U>
constexpr Weight<U> operator/(Weight<U> l, const float r) noexcept { return l /= r; }

template <weight_unit U>
inline std::ostream& operator<<(std::ostream& os, const Weight<U>& s) noexcept {
    os << s.value() << " ";
    if constexpr (std::is_same_v<U, grams>)
        os << "g";
    else if constexpr (std::is_same_v<U, kilograms>)
        os << "kg";
    else
        os << "<unknown weight unit>";
    return os;
}

// Some quality tests.
static_assert(std::is_constructible_v<Weight<grams>, float>);
static_assert(std::is_nothrow_constructible_v<Weight<grams>, float>);

static_assert(std::is_default_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_default_constructible_v<Weight<grams>>);

static_assert(std::is_copy_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_copy_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_copy_constructible_v<Weight<grams>>);

static_assert(std::is_move_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_move_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_move_constructible_v<Weight<grams>>);

void automatic_unit_conversion()
{
    const auto weight_100_kg = Weight<kilograms>(100);
    const auto weight_10_g = Weight<grams>(10);

    const Weight<grams> weight_g = weight_100_kg;
    std::cout << weight_100_kg << " equals " << weight_g << std::endl;

    const Weight<kilograms> weight_kg = weight_10_g;
    std::cout << weight_10_g << " equals " << weight_kg << std::endl;
}

}

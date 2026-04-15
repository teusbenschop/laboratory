/*
Copyright (©) 2021-2026 Teus Benschop.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "templates.h"

#include <cassert>
#include <iostream>
#include <type_traits>

namespace templates {

// Template programming is about letting the compiler deal with the types.
// Template metaprogramming is about letting the compiler calculate values.

namespace default_type {

template <typename T = int>
constexpr auto sum(T a, T b) -> T
{
    return a + b;
}

// Pass the type:
static_assert(sum<int>(1, 1) == 2);
// Omit the type: it takes int.
static_assert(sum(1, 1) == 2);

}


namespace automatic_unit_conversion {
// These are strong types for weights.
// The class automatically converts to the desired unit when it gets passed to a function.
// For example if a function expects a weight in grams,
// and the code passes a weight in kilograms,
// then the compiler automatically converts the passed unit to the expected unit.

// The factors to convert the given weight type to grams.
struct grams
{
    static constexpr float ms_factor = 1.0f;
};

struct kilograms
{
    static constexpr float ms_factor = 1000.0f;
};

// This concept can be used to assure that a type is a weight type.
template <typename T>
concept weight_unit = std::is_same_v<T, grams> or std::is_same_v<T, kilograms>;

template <weight_unit U>
class Weight
{
    float m_value{};

public:
    constexpr Weight() noexcept = default;
    // Constructor taking a float, so the weight is equal to the float passed.
    constexpr explicit Weight(const decltype(m_value) v) noexcept : m_value(v)
    {
    }

    // This constructor is a template, the compiler generates multiple constructors:
    // 1. Create a kilogram from a gram.
    // 2. Create a kilogram from a kilogram.
    // 3. Create a gram from a kilogram.
    // 4. Create a gram from a gram.
    template <weight_unit UU>
    constexpr Weight(const Weight<UU>& s) noexcept
    {
        if constexpr (std::is_same_v<U, UU>)
            m_value = s.value();
        else
            m_value = s.value() * UU::ms_factor / U::ms_factor;
    }

    [[nodiscard]] constexpr decltype(m_value) value() const noexcept { return m_value; }
    explicit constexpr operator decltype(m_value)() const noexcept { return m_value; } // Support static cast.
    constexpr void value(const decltype(m_value) v) noexcept { m_value = v; }
    auto operator<=>(const Weight<U>&) const noexcept = default;

    // Allow for basic arithmetic operations:
    constexpr Weight<U>& operator+=(const Weight<U>& s) noexcept
    {
        m_value += s.value();
        return *this;
    }

    constexpr Weight<U>& operator-=(const Weight<U>& s) noexcept
    {
        m_value -= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const Weight<U>& s) noexcept
    {
        m_value *= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const decltype(m_value) v) noexcept
    {
        m_value *= v;
        return *this;
    }

    constexpr Weight<U>& operator/=(const Weight<U>& s) noexcept
    {
        m_value /= s.value();
        return *this;
    }

    constexpr Weight<U>& operator/=(const decltype(m_value) v) noexcept
    {
        m_value /= v;
        return *this;
    }
};

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator+(Weight<UL> l, const Weight<UR> r) noexcept { return l += r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator-(Weight<UL> l, const Weight<UR> r) noexcept { return l -= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator*(Weight<UL> l, const Weight<UR> r) noexcept { return l *= r; }

template <weight_unit U>
constexpr Weight<U> operator*(Weight<U> l, const float r) noexcept { return l *= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator/(Weight<UL> l, const Weight<UR> r) noexcept { return l /= r; }

template <weight_unit U>
constexpr Weight<U> operator/(Weight<U> l, const float r) noexcept { return l /= r; }

template <weight_unit U>
inline std::ostream& operator<<(std::ostream& os, const Weight<U>& s) noexcept
{
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

constexpr auto weight_100_kg = Weight<kilograms>(100);
constexpr auto weight_10_g = Weight<grams>(10);

constexpr Weight<grams> weight_g = weight_100_kg;
static_assert(weight_g.value() == 100000);

constexpr Weight<kilograms> weight_kg = weight_10_g;
static_assert(weight_kg.value() == 0.01f);

}


namespace simple_template {

struct Point
{
    int x, y;
    Point operator+(const Point& s) const noexcept
    {
        return Point{x + s.x, y + s.y};
    }
};

// A general template.
template <typename T>
T add (T a, T b) noexcept
{
    return a + b;
}
template <typename T>
int signum (T a) noexcept
{
    if (a < 0) return -1;
    if (a > 0) return 1;
    return 0;
}

// A specialized template.
template <> // This could be left out.
Point add (const Point a, const Point b) noexcept
{
    return Point{a.x + b.x, a.y + b.y};
}
template <>
int signum (const Point a) noexcept
{
    if (signum(a.x) < 0 and signum(a.y) < 0) return -1;
    if (signum(a.x) > 0 and signum(a.y) > 0) return 1;
    return 0;
}

void demo()
{
    add(1,1);
    add(1.1,2.2);
    Point p {1,1 };
    add(p,p);
    signum(Point{1,-2});
}
}

namespace class_template {

template <typename T>
struct Cls
{
    T a;
    Cls(T a) : a(a) {}
};
void demo()
{
}
}


namespace class_with_template_methods {
template <typename T>
struct Cls
{
    T a;
    Cls(T a) : a(a) {}
    std::string get()
    {
        return std::to_string(a);
    };
};
void demo()
{
    Cls<float> cls1 (1.1);
    assert(cls1.get() == "1.100000");
}
}

void demo()
{
    simple_template::demo();
    class_template::demo();
    class_with_template_methods::demo();
}
}


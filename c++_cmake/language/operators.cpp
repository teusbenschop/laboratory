#include <cassert>
#include <iostream>

#include "functions.h"

namespace language {

// A struct with overloaded operators.

struct Container {
  constexpr Container(const int value) : value(value) { }
  int value;

  // Overload the "+" operator.
  constexpr Container operator+(const Container &other)
  {
    return Container(value + other.value);
  }

  // Overload the "()" operator.
  constexpr int operator()() { return value; }

  // Overload the += and the -= and the %= operators.
  constexpr Container& operator+=(const Container& c) noexcept {
    value += c.value; return *this;
  }
  constexpr Container& operator-=(const Container& c) noexcept {
    value -= c.value; return *this;
  }
  constexpr Container& operator%=(const Container& c) noexcept {
    value = value % c.value; return *this;
  }
};

// Overload the "<<" operator.
std::ostream& operator<<(std::ostream& os, const Container& c) noexcept
{
  os << c.value;
  return os;
}

// Overload the "==" operator.
constexpr inline bool operator==(const Container& l, const Container& r) noexcept {
  return l.value == r.value;
}

static_assert(Container(10) + Container(20) == 30);
static_assert(Container(15)() == 15);
static_assert(Container(10) != Container(20));


void operator_overloading()
{
  std::cout << Container(10) << std::endl;
  Container container(10);
  container += Container(5);
  assert(container == 15);
  container -= Container(1);
  assert(container == 14);
  container %= Container(5);
  assert(container == 4);
}

// Demo of the spaceship ( <=> ) operator in C++20.
struct Version {
  unsigned short major {};
  unsigned short minor {};

  // Setting the spaceship operator to default causes the compiler to generate
  // all comparison operators, like < <= == >= > != .
  // The compiler considers all fields, in this case major and minor.
  // If the first field is smaller than or greater than, the comparison is complete.
  // If the first fields are the same,
  // then it considers the second field, and so on,
  // till it completes the comparison.
  constexpr auto operator<=>(const Version&) const noexcept = default;
};

static_assert(Version(1,1) != Version(1,2));
static_assert(Version(1,1) <  Version(1,2));
static_assert(Version(1,1) <= Version(1,2));
static_assert(Version(1,2) >  Version(1,1));
static_assert(Version(1,2) >= Version(1,1));
static_assert(Version(1,1) == Version(1,1));

}
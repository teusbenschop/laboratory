#include <concepts>
#include <iostream>
#include <string>

#include "functions.h"

namespace concepts_library {

// An unconstrained template function.
template <typename T>
// The constexpr makes the static_assert possible below.
constexpr auto basic_sum (T one, T two) {
  return one + two;
}

// Call the template with numbers: OK.
static_assert(basic_sum(1, 2) == 3);

// Call the template with strings:
// Oops, it works, but not as intended, it concatenates instead of summing.
// It would help if the passed types could be constrained.
static_assert(basic_sum(std::string("a"), std::string("b")) == "ab");

// Call the template with chars:
// OK, it fails:
// invalid operands of types 'const char*' and 'const char*' to binary 'operator+'
// static_assert(basic_sum("a", "b") == "?");


// Three classes, two related, one unrelated, to demonstrate constraints.
struct Base {
  static constexpr int value {10};
};
struct Derived : Base {};
struct Unrelated {
  static constexpr int value {20};
};

template<typename Type>
// This template has a constraint to make sure the correct type is passed.
// The template function requires that the parameter is derived from the Base struct.
requires std::derived_from<Type, Base>
constexpr int get_value(Type object)
{
  return object.value;
}

static_assert(get_value(Base()) == 10); // This compiles.
static_assert(get_value(Derived()) == 10); // This compiles.
// static_assert(get_value(Unrelated() == 10);
// The above fails to compile due to the constraint:
// error: no matching function for call to 'get_value(template_with_constraint::Unrelated&)'


// Define a basic concept.
template <typename T>
concept floating_point = std::is_floating_point_v<T>;

// Define a concept consisting of another concept and a type trait.
template <typename T>
concept number = floating_point<T> || std::is_integral_v<T>;

// Define a concept that requires the type to have the given class members.
template <typename T>
concept range = requires(T& t) {
  std::begin(t);
  std::end(t);
};

// Constraining a type with a concept.
template <typename T>
requires number<T>
constexpr auto add_both_template(T v, T n) {
  return v + n;
}
static_assert(add_both_template(10, 11) == 21);

// Using concepts ("number") with abbreviated function templates.
constexpr number auto add_both_abbreviated(number auto v, number auto n) {
  return v + n;
}
static_assert(add_both_abbreviated(10, 11) == 21);


// A variadic template function takes a variable number of arguments.
template<typename T, typename... Args>
void variadic_template_print(T t, Args ...args){
  // The "if constexpr" is evaluated at compile time.
  // Here, if no arguments are passed, it no longer does recursion.
  if constexpr (!sizeof ...(args)) {
    std::cout << t << std::endl;
  }
  else {
    std::cout << t << ", ";
    variadic_template_print(args...);
  }
}

// The above as an abbreviated variadic template function.
void variadic_abbreviated_print(auto t, auto ...args) {
  if constexpr (!sizeof ...(args)) {
    std::cout << t << std::endl;
  }
  else {
    std::cout << t << ", ";
    variadic_abbreviated_print(args...);
  }
}

void demo_constraints_concepts()
{
    variadic_template_print   ("Hello", 1, 3.14, 5L);
    variadic_abbreviated_print("Hello", 1, 3.14, 5L);
}

}
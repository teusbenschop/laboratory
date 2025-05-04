#include "main.h"
#include <cstdlib>
#include <iostream>
#include <array>
#include <cassert>
#include <format>
#include <vector>
#ifdef __STDCPP_FLOAT128_T__
#include <stdfloat>
#endif
#include <limits>
#include <optional>
#include <expected>
//#include <flat_map>
//#include <flat_set>
//#include <generator>
#include <mdspan>
#include <print>
//#include <spanstream>
//#include <stacktrace>
#include <stdatomic.h>
#include <iomanip>


// Hands-on study of the new features in C++23.
// https://en.cppreference.com/w/cpp/23


int main()
{
  expected_example();
  //constexpr_function_with_non_literal_parameter_or_return_type();
  //static_constexpr_variables_in_constexpr();
  //non_literals_labels_gotos_in_constexpr();
  //if_consteval();
  //text_encoding_changes();
  //declaration_order_members_mandated();
  //narrowing_conversions_to_bool();
  //making_brackets_more_optional_for_lambdas();
  //alias_declarations_in_init_statements();
  //labels_at_end_of_compound_statements();
  //class_template_argument_deduction_from_inherited_constructors();
  //extended_lifetime_temporaries_range_based_for_loop_initializer();
  //simpler_implicit_move();
  //whitespace_trimming_before_line_splicing();
  //literal_suffix_z();
  //preprocessor_directives();
  //extended_floating_point_types();
  //attributes_on_lambdas();
  //attribute_assume();
  //auto_x_decay_copy();
  //static_operators_and_lambdas();
  //multidimensional_subscript_operator();
  //explicit_object_parameter();
  //language_feature_testing();
  return EXIT_SUCCESS;
}


void language_feature_testing()
{
  const auto has = [] (const bool has, const char* name) {
    if (has)
      std::cout << "Has";
    else
      std::cout << "Lacks";
    std::cout << " attribute " << name << std::endl;
  };
  const auto supports = [] (const bool supports, const char* name) {
    if (supports)
      std::cout << "Supports";
    else
      std::cout << "Does not support";
    std::cout << " " << name << std::endl;
  };
  has (__has_cpp_attribute(assume), "assume");
  has (__has_cpp_attribute(likely), "likely");
  supports (__cpp_auto_cast, "auto cast");
  supports (__cpp_consteval, "consteval");
#if __cpp_contracts
  std::cout << "Supports contracts" << std::endl;
#else
  std::cout << "Does not support contracts" << std::endl;
#endif
}


struct ExplicitObjectParameter
{
  // OK.
  // Same as void foo(int i) const &;
  void f1(this ExplicitObjectParameter const& self, int i);
  
  // Error: already declared.
  // void f1(int i) const &;
  
  // Also OK for templates.
  // For member function templates,
  // explicit object parameter allows deduction of type and value category,
  // this language feature is called “deducing this”.
  template<typename Self>
  void f2(this Self&& self);
  
  // Pass object by value: makes a copy of “*this”.
  void f3(this ExplicitObjectParameter self, int i);
  
  // Error: “const” not allowed here
  // void p(this ExplicitObjectParameter) const;

  // Error: “static” not allowed here
  // static void q(this ExplicitObjectParameter);

  // Error: an explicit object parameter can only be the first parameter
  // void r(int, this ExplicitObjectParameter);
  
  // Inside the body of an explicit object member function,
  // the "this" pointer cannot be used.
  // All member access must be done through the first parameter,
  // like in static member functions.
  void f4(this ExplicitObjectParameter object)
  {
    // invalid use of 'this' in a function with an explicit object parameter
    // auto x = this;

    // There's no implicit "this": use of undeclared identifier 'bar'
    // bar();

    object.f3(1);
  }
};


void explicit_object_parameter()
{
  // A parameter declaration with the specifier "this"
  // declares an explicit object parameter.
  
  // An explicit object parameter cannot be a function parameter pack,
  // and it can only appear as the first parameter of the parameter list
  // in the following declarations:
  // 1. A declaration of a member function or member function template.
  // 2. An explicit instantiation or explicit specialization of a templated member function.
  // 3. A lambda declaration.
  
  // A member function with an explicit object parameter has the following restrictions:
  // 1. The function is not static.
  // 2. The function is not virtual.
  // 3. The declarator of the function does not contain cv and ref.
  
  // Error: non-member functions cannot have an explicit object parameter
  // void func(this ExplicitObjectParameter& self);
  
  // A pointer to an explicit object member function is an ordinary pointer to function,
  // not a pointer to member.
  
  struct Y
  {
    int f(int, int) const& {return 1;};
    int g(this Y const&, int, int) {return 1;};
  };
  
  Y y{};
  
  auto pf = &Y::f;
  
  // error: pointers to member functions are not callable
  // called object type 'int (Y::*)(int, int) const &' is not a function or function pointer
  //pf(y, 1, 2);
  
  (y.*pf)(1, 2);            // ok
  std::invoke(pf, y, 1, 2); // ok
  
  auto pg = &Y::g;
  pg(y, 3, 4);              // ok
  
  // error: “pg” is not a pointer to member function
  // right hand operand to .* has non-pointer-to-member type 'int (*)(const Y &, int, int)'
  //(y.*pg)(3, 4);
  
  std::invoke(pg, y, 3, 4); // ok
}


template<typename T, std::size_t Z, std::size_t Y, std::size_t X>
struct Array3d
{
  std::array<T, X * Y * Z> m{};
  constexpr T& operator[](std::size_t z, std::size_t y, std::size_t x)
  {
    assert(x < X and y < Y and z < Z);
    return m[z * Y * X + y * X + x];
  }
};


void multidimensional_subscript_operator()
{
  // The operator[] can take any number of subscripts.
  // For example, an operator[] of a 3D array class declared as
  // T& operator[](std::size_t x, std::size_t y, std::size_t z)
  // can directly access the elements.
  Array3d<int, 4, 3, 2> array3d;
  array3d[3, 2, 1] = 42;
  std::cout << "array3d[3, 2, 1] = " << array3d[3, 2, 1] << std::endl;
}


void static_operators_and_lambdas()
{
  struct S
  {
    // Static operators: Can call them without the object instance.
    static int operator()(int a, int b) { return a + b; }
    static int operator[](int a, int b) { return a - b; }
  };

  S object;
  
  // This creates objects (and perhaps the optimizer removes them again).
  std::print("({})[{}]\n", S{}(1, 0), S{}[3, 1]); // (1)[2]
  std::print("({})[{}]\n", S()(2, 1), S()[5, 1]); // (3)[4]

  // This does not create objects, just calls the static method.
  std::print("({})[{}]\n", object(1, 0), object[3, 1]); // (1)[2]
  std::print("({})[{}]\n", object(2, 1), object[5, 1]); // (3)[4]
  
  // Lambda's can be made static too.
  int x {0};
  static auto lambda1 = [x] { return x;};
  auto lambda2 = [&x] {x+=28; return x;};
  // But see https://godbolt.org/z/3qeqnEsh8 that static lambdas generate much more code.
  // Consider that a static lambda does the capture (of variable x) once.
  // And that may give unexpected output.
  // The rule from R1 of this paper is basically:
  // A static lambda shall have no lambda-capture.
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html#lambdas
  std::cout << lambda1() << " " << lambda2() << std::endl;
  std::cout << lambda1() << " " << lambda2() << std::endl;
}


void auto_x_decay_copy()
{
  // A decay-copy is a copy of a variables which has lost some properties.
  // How does auto(x) help?
  // It is an easy way to make a copy of a variable.
  // It clearly communicates that it makes a copy of a variable.

  const auto pop_front = [] (auto& x) {
    std::erase(x, auto(x.front())); // <- Make copy through auto(x)
  };

  std::vector<int> v {1, 2, 3};
  std::cout << v.size() << std::endl;
  pop_front (v);
  std::cout << v.size() << std::endl;
  pop_front (v);
  std::cout << v.size() << std::endl;
}


void attribute_assume()
{
  // Specifies that the given expression is assumed to always evaluate to true
  // to allow compiler optimizations based on the information given.
  // Since assumptions cause runtime-undefined behavior if they do not hold,
  // they should be used sparingly.
  // https://en.cppreference.com/w/cpp/language/attributes/assume

  auto f = [] (auto x) {
    // Compiler may assume x is positive.
    [[assume(x > 0)]];
    std::cout << x << std::endl;
  };

  f(2);
  f(1);
  // Likely crashes because assumption fails.
  f(0);
  f(-1);
}


void attributes_on_lambdas()
{
  const auto f = [] [[nodiscard]] (const auto msg) {
    // -------------^^^^^^^^^^^^^ <-- attribute
    throw std::runtime_error(msg);
  };

  try {
    f("msg");
  } catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
  }
}


void extended_floating_point_types()
{
#ifdef __STDCPP_FLOAT128_T__
  std::float64_t f64 = 0.0;
#else
  std::cout << "No support for std::float64_t" << std::endl;
#endif
}


void preprocessor_directives()
{
#define d
#ifdef a
  std::cout << "defined a" << std::endl;
#elifdef d
  std::cout << "defined d" << std::endl;
#endif
  
#ifdef a
  std::cout << "defined a" << std::endl;
#elifndef a
  std::cout << "not defined a" << std::endl;
#endif
}


void literal_suffix_z()
{
  // Avoid this warning:
  // comparison of integers of different signs: 'int' and 'size_type' (aka 'unsigned long') [-Wsign-compare]
  std::vector<int> v{2, 4, 6, 8};
  for (auto i = 0uz; i < v.size(); ++i) {
    std::cout << i << " : " << v.at(i) << std::endl;
  }
}


void whitespace_trimming_before_line_splicing()
{
#pragma clang diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
  constexpr int i = 1
  // \
  + 42
  ;
#pragma clang diagnostic pop
  assert(i == 1);
  std::cout << i << std::endl;
}


void simpler_implicit_move()
{
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2266r3.html
}


void extended_lifetime_temporaries_range_based_for_loop_initializer()
{
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2718r0.html
}


void class_template_argument_deduction_from_inherited_constructors()
{
  // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2582r1.pdf
}


void labels_at_end_of_compound_statements()
{
  const auto f = [](int x) {
    if (x)
      goto end;
    x = 42;
  end:
    std::cout << x << std::endl;
  };
  
  f(0);
  f(10);
}


void alias_declarations_in_init_statements()
{
  std::vector<int> v {1, 2, 3};
  for (using T = int; T& e : v)
    std::cout << e << std::endl;
}


void making_brackets_more_optional_for_lambdas()
{
  std::string s1 = "s1";
  const auto with_parenthesis = [s1 = std::move(s1)] () mutable {
    std::cout << s1 << std::endl;
  };
  
  std::string s2 = "s2";
  const auto without_parenthesis = [s2 = std::move(s2)] mutable {
    std::cout << s2 << std::endl;
  };
}


void narrowing_conversions_to_bool()
{
  constexpr int i {1};
  if constexpr (static_cast<bool> (i)) { }
  if constexpr(i != 0) {}
  if constexpr(i) {}
  static_assert(static_cast<bool>(i));
  static_assert(i);
  static_assert(i % 4 != 0);
  static_assert(i % 4);
}


void declaration_order_members_mandated()
{
}


void text_encoding_changes()
{
  // Character sets and encodings.
#define S(x) # x
  const char* s1 = S(Köppe);       // "Köppe"
  const char* s2 = S(K\u00f6ppe);  // "Köppe"
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;
  
  // Consistent character literal encoding.
#if 'A' == '\x41'
  std::cout << R"('A' == '\x41')" << std::endl;
#endif
  if ('A' == 0x41)
    std::cout << R"('A' == '0x41')" << std::endl;
  
  // Named universal character escapes.
  // https://www.unicode.org/Public/14.0.0/ucd/NamesList.txt
  std::cout << "\N{CAT FACE}" << std::endl;
  std::cout << "\N{COW FACE}" << std::endl;
  std::cout << "\N{NATIONAL PARK}" << std::endl;

  // Delimited escape sequences.
  std::cout << "\o{111}" << " " << "\x{A0}" << " " << "\u{CAFE}" << std::endl;
  
  // Support for UTF-8 as a portable file source encoding.
  // The .h and .cpp files, and so on, are supported by all compilers.
}


constexpr bool is_constant_evaluated() noexcept
{
  if consteval { return true; } else { return false; }
}


constexpr bool is_runtime_evaluated() noexcept
{
  if not consteval { return true; } else { return false; }
}


consteval std::uint64_t ipow_ct(std::uint64_t base, std::uint8_t exp)
{
  if (!base) return base;
  std::uint64_t res{1};
  while (exp)
  {
    if (exp & 1) res *= base;
    exp /= 2;
    base *= base;
  }
  return res;
}


constexpr std::uint64_t ipow(std::uint64_t base, std::uint8_t exp)
{
  if consteval { // use a compile-time friendly algorithm
    return ipow_ct(base, exp);
  }
  else { // use runtime evaluation
    std::cout << "runtime" << std::endl;
    return std::pow(base, exp);
  }
}


void if_consteval()
{
  static_assert(ipow(0, 10) == 0);
  static_assert(ipow(2, 10) == 1024);
  int base {10};
  std::cout << ipow(base, 3) << std::endl;
}


template<typename T> constexpr bool f() {
  if (std::is_constant_evaluated()) {
    // ...
    return true;
  } else {
    T t;
    int n = [&]() {
      thread_local int n = 1;
      return n;
    }();
    return n + 1;
    // ...
    return true;
  }
}


void non_literals_labels_gotos_in_constexpr()
{
  std::string s {};
  static_assert(f<std::string>());
}


constexpr char xdigit(int n) {
  static constexpr char digits[] = "0123456789abcdef";
  return digits[n];
}


void static_constexpr_variables_in_constexpr()
{
  std::cout << xdigit(1) << std::endl;
}


constexpr void cfwnlport(std::optional<int>& o) {
  o.reset();
}

void constexpr_function_with_non_literal_parameter_or_return_type()
{
  std::optional<int> i {1};
  cfwnlport (i);
}


enum class parse_error
{
  invalid_input,
  overflow
};

auto parse_number(std::string_view& str) -> std::expected<double, parse_error>
{
  const char* begin = str.data();
  char* end;
  float number = std::strtof(begin, &end);
  
  if (begin == end)
    return std::unexpected(parse_error::invalid_input);
  if (std::isinf(number))
    return std::unexpected(parse_error::overflow);
  
  str.remove_prefix(end - begin);
  return number;
}

void expected_example()
{
  const auto process = [](std::string_view str) -> void
  {
    std::cout << "input: " << std::quoted(str) << std::endl;
    const auto num = parse_number(str);
    std::cout << "has value: " << num.has_value() << std::endl;
    std::cout << "dereferenced value: " << *num << std::endl;
    // If num did not have a value, dereferencing num
    // would cause an undefined behavior, and
    try {
      // num.value() would throw std::bad_expected_access.
      std::cout << ".value(): " << num.value() << std::endl;
    } catch (const std::exception& exception) {
      std::cout << "exception: " << exception.what() << std::endl;
    }
    std::cout << "value or default: " << num.value_or(123) << std::endl;
    if (num.error() == parse_error::invalid_input)
      std::cout << "has invalid input" << std::endl;
    if (num.error() == parse_error::overflow)
      std::cout << "overflow error" << std::endl;
  };
  
  for (auto src : {"42.2", "42abc", "meow", "inf"})
    process(src);
}

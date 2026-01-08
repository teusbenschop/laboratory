


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


void move_only_function()
{
  std::packaged_task<double()> packaged_task([](){ return 3.14159; });
  
  std::future<double> future = packaged_task.get_future();
  
  auto lambda = [task = std::move(packaged_task)]() mutable { task(); };
  
  // std::function<void()> function = std::move(lambda); // Error
  //std::move_only_function<void()> function = std::move(lambda); // OK does not yet compile on macOS 15.4.1
  
  //function();
  
  std::cout << future.get();
}


void bind_back()
{
  const auto multiply_add = [](int a, int b, int c) { return a * b + c; };
  const auto multiply_plus_seven = std::bind_back(multiply_add, 7);
  assert(multiply_plus_seven(4, 10) == 47); //: multiply_add(4, 10, 7) == 4 * 10 + 7 = 47.
  
#if __cpp_lib_bind_back >= 202306L
  auto mul_plus_seven_cpp26 = std::bind_back<multiply_add>(7);
  assert(mul_plus_seven_cpp26(4, 10) == 47);
#endif
}


void byte_swap()
{
  const auto dump = [] (auto value){
    std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(value) * 2) << value << " : ";
    for (std::size_t i{}; i != sizeof(value); i++, value >>= 8)
      std::cout << std::setw(2) << static_cast<unsigned>(decltype(value)(0xFF) & value) << ' ';
    std::cout << std::dec << std::endl;
  };
  
  static_assert(std::byteswap('a') == 'a');
  
  std::cout << "byteswap for U16:\n";
  constexpr auto x = std::uint16_t(0xCAFE);
  dump(x);
  dump(std::byteswap(x));
  
  std::cout << "\nbyteswap for U32:\n";
  constexpr auto y = std::uint32_t(0xDEADBEEFu);
  dump(y);
  dump(std::byteswap(y));
  
  std::cout << "\nbyteswap for U64:\n";
  constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
  dump(z);
  dump(std::byteswap(z));
}


struct TypeTeller
{
  void operator()(this auto&& self)
  {
    using self_type = decltype(self);
    using unref_self_type = std::remove_reference_t<self_type>;
    if constexpr (std::is_lvalue_reference_v<self_type>)
    {
      if constexpr (std::is_const_v<unref_self_type>)
        std::cout << "const lvalue" << std::endl;
      else
        std::cout << "mutable lvalue" << std::endl;
    }
    if constexpr (std::is_rvalue_reference_v<self_type>) {
      if constexpr (std::is_const_v<unref_self_type>)
        std::cout << "const rvalue" << std::endl;
      else
        std::cout << "mutable rvalue" << std::endl;
    }
  }
};


struct FarStates
{
  std::unique_ptr<TypeTeller> pointer;
  std::optional<TypeTeller> optional;
  std::vector<TypeTeller> container;
  
  auto&& from_opt(this auto&& self)
  {
    return std::forward_like<decltype(self)>(self.optional.value());
    // It is OK to use std::forward<decltype(self)>(self).opt.value(),
    // because std::optional provides suitable accessors.
  }
  
  auto&& operator[](this auto&& self, std::size_t i)
  {
    return std::forward_like<decltype(self)>(self.container.at(i));
    // It is not so good to use std::forward<decltype(self)>(self)[i], because
    // containers do not provide rvalue subscript access, although they could.
  }
  
  auto&& from_ptr(this auto&& self)
  {
    if (!self.pointer)
      throw std::bad_optional_access{};
    return std::forward_like<decltype(self)>(*self.pointer);
    // It is not good to use *std::forward<decltype(self)>(self).ptr, because
    // std::unique_ptr<TypeTeller> always dereferences to a non-const lvalue.
  }
};


void forward_like()
{
  FarStates my_state
  {
    .pointer{std::make_unique<TypeTeller>()},
    .optional{std::in_place, TypeTeller{}},
    .container{std::vector<TypeTeller>(1)},
  };
  
  my_state.from_ptr()();
  my_state.from_opt()();
  my_state[0]();
  
  std::cout << std::endl;
  
  std::as_const(my_state).from_ptr()();
  std::as_const(my_state).from_opt()();
  std::as_const(my_state)[0]();
  
  std::cout << std::endl;
  
  std::move(my_state).from_ptr()();
  std::move(my_state).from_opt()();
  std::move(my_state)[0]();
  
  std::cout << std::endl;
  
  std::move(std::as_const(my_state)).from_ptr()();
  std::move(std::as_const(my_state)).from_opt()();
  std::move(std::as_const(my_state))[0]();
}


void invoke_r()
{
  struct Foo
  {
    Foo(const int num) : m_num(num) {}
    void print_add(const int i) const {
      std::cout << m_num + i << std::endl;
    }
    int m_num;
  };
  
  const auto print_num = [](const int i)
  {
    std::cout << i << std::endl;
  };
  
  struct PrintNum
  {
    void operator()(const int i) const
    {
      std::cout << i << std::endl;
    }
  };
  
  std::cout << "invoke a free function: ";
  std::invoke(print_num, -9);
  
  std::cout << "invoke a lambda: ";
  std::invoke([&print_num](){ print_num(42); });
  
  std::cout << "invoke a member function: ";
  const Foo foo(123);
  std::invoke(&Foo::print_add, foo, 1);
  
  std::cout << "invoke (i.e., access) data member m_num: "
  << std::invoke(&Foo::m_num, foo) << std::endl;
  
  std::cout << "invoke a function object: ";
  std::invoke(PrintNum(), 18);
  
  const auto add = [](int x, int y){ return x + y; };
  
  std::cout << "invoke a lambda converting result to float: ";
  const auto ret = std::invoke_r<float>(add, 11, 22);
  static_assert(std::is_same<decltype(ret), const float>());
  std::cout << std::fixed << ret << std::endl;

  std::cout << "invoke print_num: ";
  std::invoke_r<void>(print_num, 44);
}


void to_underlying()
{

  enum class E1 : char { e };
  static_assert(std::is_same_v<char, decltype(std::to_underlying(E1::e))>);
  
  enum struct E2 : long { e };
  static_assert(std::is_same_v<long, decltype(std::to_underlying(E2::e))>);
  
  enum E3 : unsigned { e };
  static_assert(std::is_same_v<unsigned, decltype(std::to_underlying(e))>);

  enum class ColorMask : std::uint32_t
  {
    red = 0xFF,
    green = (red << 8),
    blue = (green << 8),
    alpha = (blue << 8)
  };
  static_assert(std::is_same_v<std::uint32_t, decltype(std::to_underlying(ColorMask::red))>);

  std::cout << std::hex << std::uppercase << std::setfill('0')
  << std::setw(8) << std::to_underlying(ColorMask::red) << std::endl
  << std::setw(8) << std::to_underlying(ColorMask::green) << std::endl
  << std::setw(8) << std::to_underlying(ColorMask::blue) << std::endl
  << std::setw(8) << std::to_underlying(ColorMask::alpha) << std::endl;
  
//  std::underlying_type_t<ColorMask> x = ColorMask::alpha; // Error: no known conversion
  std::underlying_type_t<ColorMask> y = std::to_underlying(ColorMask::alpha); // OK
  std::cout << y << std::endl;
}


void unreachable()
{
  struct Color { std::uint8_t r, g, b, a; };
  
  // Assume that only restricted set of texture caps is supported.
  const auto generate_texture = [](std::vector<Color>& tex, std::size_t xy) -> void
  {
    switch (xy)
    {
      case 128: [[fallthrough]];
      case 256: [[fallthrough]];
      case 512:
        tex.clear();
        tex.resize(xy * xy, Color{0, 0, 0, 0});
        break;
      default:
        std::unreachable();
    }
  };
  
  std::vector<Color> texture;
  generate_texture(texture, 128); // OK.
  assert(texture.size() == 128 * 128);
  generate_texture(texture, 32); // Results in undefined behavior.
}


// Monadic operations (transform, or_else, and and_then)
// for std::optional (P0798R8) and std::expected (P2505R5)
void monadic_operations_on_optional()
{
  // and_then
  // returns the result of the given function on the contained value if it exists,
  // or an empty optional otherwise

  // transform
  // returns an optional containing the transformed contained value if it exists,
  // or an empty optional otherwise

  // or_else
  // returns the optional itself if it contains a value,
  // or the result of the given function otherwise

  using namespace std::literals;

  const std::vector<std::optional<std::string>> v
  {
    "1234", "15 foo", "bar", "42", "5000000000", " 5", std::nullopt, "-43"
  };
  
  for (auto&& x : v | std::views::transform([](auto&& o) {
    
    const auto to_int = [](std::string_view sv) -> std::optional<int>
    {
      int r{};
      auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), r)};
      if (ec == std::errc())
        return r;
      else
        return std::nullopt;
    };

    // Print the content of input optional<string>
    std::cout << std::left << std::setw(13)
    << std::quoted(o.value_or("nullopt")) << " -> ";
    
    return o
    // if optional is nullopt convert it to optional with "" string
      .or_else([]{ return std::optional{""s}; })
    // flatmap from strings to ints (making empty optionals where it fails)
      .and_then(to_int)
    // map int to int + 1
      .transform([](int n) { return n + 1; })
    // convert back to strings
      .transform([](int n) { return std::to_string(n); })
    // replace all empty optionals that were left by
    // and_then and ignored by transforms with "NaN"
      .value_or("NaN");
    }
    ))
    std::cout << x << std::endl;
}


void monadic_operations_on_expected()
{
  const std::vector<std::optional<std::string>> inputs {
    "1234",
    "15 foo",
    "bar",
    "42",
    "5000000000",
    " 5",
    std::nullopt,
    "-43"
  };
  
  // Instance std::expected<T, E> provides a way to store either of two values.
  // An instance of std::expected always holds a value:
  // either the expected value of type T, or the unexpected value of type E.
  // This vocabulary type requires the header <expected>.
  // With std::expected you can implement functions that either return a value or an error.
  
  const auto get_int = [](const auto input) -> std::expected<int, std::string> {
    try {
      return std::stoi (input.value_or(""));
    } catch (const std::exception& exception) {
      return std::unexpected(exception.what());
    }
  };
  
  // Template std::expected supports monadic operations for convenient function composition:
  // expected.and_then, expected.transform, expected.or_else, and expected.transform_error.
  // and_then
  // Returns the result of the given function call if it exists or an empty std::expected.
  // transform
  // Returns a std::expected containing its transformed value or an empty std::expected.
  // or_else
  // Returns the std::expected if it contains a value.
  // Otherwise it returns the result of the given function.
  // transform_error
  // returns the std::expected if it contains an expected value.
  // Otherwise, it returns a std::expected that contains a transformed unexpected value.
  
  for (const auto& input : inputs) {
    std::cout << std::left << std::setw(12) << std::quoted(input.value_or("nullopt")) << " -> ";
    const auto result = get_int(input)
      .transform( [](int n) { return n + 1; })
    ;
    if (result)
      std::cout << result.value();
    else
      std::cout << result.error();
    std::cout << std::endl;
  }
}


void tuple()
{
  const auto get_student = [](int id) -> std::tuple<double, char, std::string>
  {
    switch (id)
    {
      case 0: return {3.8, 'A', "Lisa Simpson"};
      case 1: return {2.9, 'C', "Milhouse Van Houten"};
      case 2: return {1.7, 'D', "Ralph Wiggum"};
      case 3: return {0.6, 'F', "Bart Simpson"};
    }
    throw std::invalid_argument("id");
  };
  
  {
    const auto student = get_student(0);
    std::cout
    << "ID: 0, "
    << "GPA: "   << std::get<0>(student) << ", "
    << "grade: " << std::get<1>(student) << ", "
    << "name: "  << std::get<2>(student)
    << std::endl;
  }
  
  {
    const auto student = get_student(1);
    std::cout
    << "ID: 1, "
    << "GPA: "   << std::get<double>(student) << ", "
    << "grade: " << std::get<char>(student)   << ", "
    << "name: "  << std::get<std::string>(student)
    << std::endl;
  }

  {
    double gpa;
    char grade;
    std::string name;
    std::tie(gpa, grade, name) = get_student(2);
    std::cout 
    << "ID: 2, "
    << "GPA: "   << gpa   << ", "
    << "grade: " << grade << ", "
    << "name: "  << name
    << std::endl;
  }
  
  // C++17 structured binding:
  {
    const auto [gpa, grade, name] = get_student(3);
    std::cout 
    << "ID: 3, "
    << "GPA: "   << gpa << ", "
    << "grade: " << grade << ", "
    << "name: "  << name
    << std::endl;
  }
}


void common_reference()
{
  static_assert(std::same_as<int&, std::common_reference_t<
                std::add_lvalue_reference_t<int>,
                std::add_lvalue_reference_t<int>&,
                std::add_lvalue_reference_t<int>&&
                >>);
}


/*
template<typename T>
struct Tree
{
  T value;
  Tree *left{}, *right{};
  
  std::generator<const T&> traverse_inorder() const
  {
    if (left)
      co_yield std::ranges::elements_of(left->traverse_inorder());
    
    co_yield value;
    
    if (right)
      co_yield std::ranges::elements_of(right->traverse_inorder());
  }
};
 */

void coroutine_generator_for_ranges()
{
  /*
  Tree<char> tree[]
  {
    {'D', tree + 1, tree + 2},
    //                            │
    //            ┌───────────────┴────────────────┐
    //            │                                │
    {'B', tree + 3, tree + 4},       {'F', tree + 5, tree + 6},
    //            │                                │
    //  ┌─────────┴─────────────┐      ┌───────────┴─────────────┐
    //  │                       │      │                         │
    {'A'},                  {'C'}, {'E'},                    {'G'}
  };
  
  for (char x : tree->traverse_inorder())
    std::cout << x << ' ';
  std::cout << '\n';
   */
}


void stacktrace_library()
{
  // Could not be compiled on macOS 15.4.1.
}


void views_adjacent()
{
  [[maybe_unused]] constexpr std::array v{1, 2, 3, 4, 5, 6};
  std::cout << "v = [1 2 3 4 5 6]" << std::endl;
  
//  for (int i{}; std::tuple t : v | std::views::adjacent<3>)
//  {
//    auto [t0, t1, t2] = t;
//    std::cout << std::format("e = {:<{}}[{} {} {}]", "", 2 * i++, t0, t1, t2) << std::endl;
//  }
  
  // Output:
  //  v = [1 2 3 4 5 6]
  //  e = [1 2 3]
  //  e =   [2 3 4]
  //  e =     [3 4 5]
  //  e =       [4 5 6]
}


void views_as_const()
{
  int x[]{1, 2, 3, 4, 5};
  
  auto v1 = x | std::views::drop(2);
  assert(v1.back() == 5);
  v1[0]++; // OK, can modify non-const element
  
  //auto v2 = x | std::views::drop(2) | std::views::as_const;
  //assert(v2.back() == 5);
  // v2[0]++; // Compile-time error, cannot modify read-only element
}


void views_as_rvalue()
{
  std::vector<std::string> words = { "The", "\N{FOX FACE}", "failed", "to", "..." };
  
  std::vector<std::string> new_words;
  
  // Move strings from words into new_words.
  std::ranges::copy (words | std::views::as_rvalue, std::back_inserter(new_words));
  
  std::cout << "Old words: ";
  for (auto&& word : words)
    std::cout << std::quoted(word) << ' ';
  std::cout << std::endl;
  
  std::cout << "New words: ";
  for (auto&& word : new_words)
    std::cout << std::quoted(word) << ' ';
  std::cout << std::endl;
}




void views_cartesian_product()
{
  [[maybe_unused]] const auto x = std::array{'A', 'B'};
  const auto y = std::vector{1, 2, 3};
  const auto z = std::list<std::string>{"α", "β", "γ", "δ"};

  [[maybe_unused]] const auto print = [](std::tuple<char const&, int const&, std::string const&> t, int pos)
  {
    const auto& [a, b, c] = t;
    std::cout << '(' << a << ' ' << b << ' ' << c << ')' << (pos % 4 ? " " : "\n");
  };

  // Does not yet compile on macOS 15.5
//  for (int i{1}; auto const& tuple : std::views::cartesian_product(x, y, z))
//    print(tuple, i++);

  // Output:
  //  (A 1 α) (A 1 β) (A 1 γ) (A 1 δ)
  //  (A 2 α) (A 2 β) (A 2 γ) (A 2 δ)
  //  (A 3 α) (A 3 β) (A 3 γ) (A 3 δ)
  //  (B 1 α) (B 1 β) (B 1 γ) (B 1 δ)
  //  (B 2 α) (B 2 β) (B 2 γ) (B 2 δ)
  //  (B 3 α) (B 3 β) (B 3 γ) (B 3 δ)
}


void views_chunk_by()
{
  const auto print_chunks = [](auto view, std::string_view separator = ", ")
  {
    for (auto const subrange : view)
    {
      std::cout << '[';
      for (std::string_view prefix; auto const& elem : subrange)
        std::cout << prefix << elem, prefix = separator;
      std::cout << "] ";
    }
    std::cout << std::endl;
  };

  std::initializer_list v1 = {1, 2, 3, 1, 2, 3, 3, 3, 1, 2, 3};
  auto fn1 = std::ranges::less{};
  auto view1 = v1 | std::views::chunk_by(fn1);
  print_chunks(view1);
  
  std::initializer_list v2 = {1, 2, 3, 4, 4, 0, 2, 3, 3, 3, 2, 1};
  auto fn2 = std::ranges::not_equal_to{};
  auto view2 = v2 | std::views::chunk_by(fn2);
  print_chunks(view2);
  
  std::string_view v3 = "__cpp_lib_ranges_chunk_by";
  auto fn3 = [](auto x, auto y) { return not(x == '_' or y == '_'); };
  auto view3 = v3 | std::views::chunk_by(fn3);
  print_chunks(view3, "");
  
  std::string_view v4 = "\u007a\u00df\u6c34\u{1f34c}"; // "zß水🍌"
  auto fn4 = [](auto, auto ß) { return 128 == ((128 + 64) & ß); };
  auto view4 = v4 | std::views::chunk_by(fn4);
  print_chunks(view4, "");
}




void views_chunk()
{
  [[maybe_unused]] auto print_subrange = [](std::ranges::viewable_range auto&& r)
  {
    std::cout << '[';
    for (int pos{}; auto elem : r)
      std::cout << (pos++ ? " " : "") << elem;
    std::cout << "] ";
  };

  [[maybe_unused]] const auto v = {1, 2, 3, 4, 5, 6};
  
  // Does not yet compile on macOS 15.4.
//  for (const unsigned width : std::views::iota(1U, 2U + v.size()))
//  {
//    auto const chunks = v | std::views::chunk(width);
//    std::cout << "chunk(" << width << "): ";
//    std::ranges::for_each(chunks, print_subrange);
//    std::cout << std::endl;
//  }
  
  // Output:
  //  chunk(1): [1] [2] [3] [4] [5] [6]
  //  chunk(2): [1 2] [3 4] [5 6]
  //  chunk(3): [1 2 3] [4 5 6]
  //  chunk(4): [1 2 3 4] [5 6]
  //  chunk(5): [1 2 3 4 5] [6]
  //  chunk(6): [1 2 3 4 5 6]
  //  chunk(7): [1 2 3 4 5 6]
}


void views_enumerate()
{
//  std::vector days {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//  for(const auto & [index, value] : std::views::enumerate(days)) {
//    std::cout << index << " " << value << std::endl;
//  }
}


void views_join_width()
{
  using namespace std::literals;
  
  const std::vector v {"This"sv, "is"sv, "a"sv, "test."sv};
//  auto joined = v | std::views::join_with(' ');
  
//  for (auto c : joined)
//    std::cout << c;
  std::cout << std::endl;
  
  // Output: This is a test.
}


void views_repeat()
{
  using namespace std::literals;
  
  // Bounded overload.
  for (auto s : std::views::repeat("C++"sv, 3))
    std::cout << s << ' ';
  std::cout << std::endl;
  
  // Ubounded overload.
  for (auto s : std::views::repeat("I know that you know that"sv) | std::views::take(3))
    std::cout << s << ' ';
  std::cout << "..." << std::endl;
}


void views_slide()
{
  [[maybe_unused]] const auto print_subrange = [](std::ranges::viewable_range auto&& r)
  {
    std::cout << '[';
    for (char space[]{0,0}; auto elem : r)
      std::cout << space << elem, *space = ' ';
    std::cout << "] ";
  };
  
  const auto v = {1, 2, 3, 4, 5, 6};
  
  std::cout << "All sliding windows of width:" << std::endl;
  for (const unsigned width : std::views::iota(1U, 1U + v.size()))
  {
//    const auto windows = v | std::views::slide(width);
    std::cout << "W = " << width << ": ";
//    std::ranges::for_each(windows, print_subrange);
    std::cout << std::endl;
  }
  
  // Output:
  //  All sliding windows of width W:
  //  W = 1: [1] [2] [3] [4] [5] [6]
  //  W = 2: [1 2] [2 3] [3 4] [4 5] [5 6]
  //  W = 3: [1 2 3] [2 3 4] [3 4 5] [4 5 6]
  //  W = 4: [1 2 3 4] [2 3 4 5] [3 4 5 6]
  //  W = 5: [1 2 3 4 5] [2 3 4 5 6]
  //  W = 6: [1 2 3 4 5 6]
}


void views_stride()
{
  using namespace std::literals;
  
  const auto print = [](std::ranges::viewable_range auto&& v, std::string_view separator = " ")
  {
    for (auto const& x : v)
      std::cout << x << separator;
    std::cout << std::endl;
  };

  print(std::views::iota(1, 13));

//  print(std::views::iota(1, 13) | std::views::stride(3));
//  print(std::views::iota(1, 13) | std::views::stride(3) | std::views::reverse);
//  print(std::views::iota(1, 13) | std::views::reverse | std::views::stride(3));
//  
//  print("0x0!133713337*x//42/A$@"sv | std::views::stride(0B11) |
//        std::views::transform([](char O) -> char { return 0100 | O; }),
//        "");
  // Output:
  // 1 2 3 4 5 6 7 8 9 10 11 12
  // 1 4 7 10
  // 10 7 4 1
  // 12 9 6 3
  // password
}


void views_zip()
{
  const auto print = [] (auto const rem, auto const& range)
  {
    for (std::cout << rem; auto const& elem : range)
      std::cout << elem << ' ';
    std::cout << std::endl;
  };
  
  auto x = std::vector{1, 2, 3, 4};
  auto y = std::list<std::string>{"α", "β", "γ", "δ", "ε"};
  auto z = std::array{'A', 'B', 'C', 'D', 'E', 'F'};
  
  print("Source views:", "");
  print("x: ", x);
  print("y: ", y);
  print("z: ", z);
  
  std::cout << std::endl;

  print("zip(x,y,z):", "");
  
  for (std::tuple<int&, std::string&, char&> elem : std::views::zip(x, y, z))
  {
    std::cout << std::get<0>(elem) << ' '
    << std::get<1>(elem) << ' '
    << std::get<2>(elem) << '\n';
    
    std::get<char&>(elem) += ('a' - 'A'); // modifies the element of z
  }
  
  std::cout << std::endl;

  print("After modification, z: ", z);
}


void views_zip_transform()
{
  const auto print = [](auto const rem, auto const& r)
  {
    std::cout << rem << '{';
    for (char o[]{0,' ',0}; auto const& e : r)
      std::cout << o << e, *o = ',';
    std::cout << "}" << std::endl;
  };
  
  auto v1 = std::vector<float>{1, 2, 3};
  auto v2 = std::list<short>{1, 2, 3, 4};
  auto v3 = std::to_array({1, 2, 3, 4, 5});
  
  [[maybe_unused]] const auto add = [](auto a, auto b, auto c) { return a + b + c; };
  
  // auto sum = std::views::zip_transform(add, v1, v2, v3);
  
  print("v1:  ", v1);
  print("v2:  ", v2);
  print("v3:  ", v3);
  //print("sum: ", sum);
  
// Output:
// v1:  {1, 2, 3}
// v2:  {1, 2, 3, 4}
// v3:  {1, 2, 3, 4, 5}
// sum: {3, 6, 9}
}


void range_adaptor_closure()
{
  // Define Slice as a range adaptor closure.
  struct Slice : std::ranges::range_adaptor_closure<Slice>
  {
    std::size_t start {0};
    std::size_t end {std::string_view::npos};
    
    constexpr std::string_view operator()(std::string_view sv) const
    {
      return sv.substr(start, end - start);
    }
  };
  
  constexpr std::string_view str = "01234567";
  
  constexpr const auto start {1};
  constexpr const auto width {5};
  constexpr Slice slice {.start = start, .end = start + width};
  
  // Use slicer as a normal function object.
  constexpr auto sv1 = slice(str);
  static_assert(sv1 == "12345");
  
  // Use slicer as a range adaptor closure object.
  constexpr auto sv2 = str | slice;
  static_assert(sv2 == "12345");
  
  // Range adaptor closures can be composed.
  constexpr auto slice_and_drop = slice | std::views::drop(2);
  static_assert((str | slice_and_drop) == "345");
}


void ranges_to()
{
  {
    std::println("{}", std::views::iota(1, 5));

    auto vec = std::views::iota(1, 5)
    | std::views::transform([](int v){ return v * 2; })
    | std::ranges::to<std::vector>();
    
    static_assert(std::same_as<decltype(vec), std::vector<int>>);
    std::println("{}", vec);
    
    auto list = vec | std::views::take(3) | std::ranges::to<std::list<double>>();

    static_assert(std::same_as<decltype(list), std::list<double>>);
    std::println("{}", list);
  }
  
  // Direct init
  {
    char array[]{'a', 'b', '\0', 'c'};
    
    // Argument type is convertible to result value type:
    auto str_to = std::ranges::to<std::string>(array);
    std::println("{}", str_to);
    
    // Equivalent to
    std::string str(array);
    
    // Result type is not an input range:
    auto re_to = std::ranges::to<std::regex>(array);

    // Equivalent to
    std::regex re(array);
  }
}


void range_starts_with()
{
  using namespace std::literals;
  
  constexpr auto ascii_upper = [](char8_t c)
  {
    return u8'a' <= c && c <= u8'z' ? static_cast<char8_t>(c + u8'A' - u8'a') : c;
  };
  
  [[maybe_unused]] constexpr auto cmp_ignore_case = [=](char8_t x, char8_t y)
  {
    return ascii_upper(x) == ascii_upper(y);
  };
  
//  static_assert(std::ranges::starts_with("const_cast", "const"sv));
//  static_assert(std::ranges::starts_with("constexpr", "const"sv));
//  static_assert(!std::ranges::starts_with("volatile", "const"sv));
//  
//  std::cout << std::boolalpha
//  << std::ranges::starts_with(u8"Constantinopolis", u8"constant"sv,
//                              {}, ascii_upper, ascii_upper) << ' '
//  << std::ranges::starts_with(u8"Istanbul", u8"constant"sv,
//                              {}, ascii_upper, ascii_upper) << ' '
//  << std::ranges::starts_with(u8"Metropolis", u8"metro"sv,
//                              cmp_ignore_case) << ' '
//  << std::ranges::starts_with(u8"Acropolis", u8"metro"sv,
//                              cmp_ignore_case) << '\n';
  
  [[maybe_unused]] constexpr static auto v = { 1, 3, 5, 7, 9 };
  [[maybe_unused]] constexpr auto odd = [](int x) { return x % 2; };
//  static_assert(std::ranges::starts_with(v, std::views::iota(1)
//                                         | std::views::filter(odd)
//                                         | std::views::take(3)));
}


void range_ends_with()
{
//  static_assert (not std::ranges::ends_with("for", "cast"));
//  static_assert (std::ranges::ends_with("dynamic_cast", "cast"));
//  static_assert (std::ranges::ends_with("as_const", "cast"));
//  static_assert (std::ranges::ends_with("bit_cast", "cast"));
//  static_assert (not std::ranges::ends_with("to_underlying", "cast"));
//  static_assert (std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{3, 4}));
//  static_assert (not std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{4, 5}));
}


void range_contains()
{
  [[maybe_unused]] constexpr auto haystack = std::array{3, 1, 4, 1, 5};
  [[maybe_unused]] constexpr auto needle = std::array{1, 4, 1};
  [[maybe_unused]] constexpr auto bodkin = std::array{2, 5, 2};
  
//  static_assert (std::ranges::contains(haystack, 4));
//  static_assert (not std::ranges::contains(haystack, 6));
//  static_assert (std::ranges::contains_subrange(haystack, needle));
//  static_assert (not ranges::contains_subrange(haystack, bodkin));
  
  [[maybe_unused]] constexpr std::array<std::complex<double>, 3> nums{{{1, 2}, {3, 4}, {5, 6}}};
//  static_assert(std::ranges::contains(nums, {3, 4}));
//  static_assert(std::ranges::contains(nums, std::complex<double>{3, 4}));
}


void range_find_last()
{
  [[maybe_unused]] constexpr static auto v = {1, 2, 3, 1, 2, 3, 1, 2};
  
  {
//    constexpr auto i1 = std::ranges::find_last(v.begin(), v.end(), 3);
//    constexpr auto i2 = std::ranges::find_last(v, 3);
//    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
//    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
//    constexpr auto i1 = std::ranges::find_last(v.begin(), v.end(), -3);
//    constexpr auto i2 = std::ranges::find_last(v, -3);
//    static_assert(i1.begin() == v.end());
//    static_assert(i2.begin() == v.end());
  }
  
  [[maybe_unused]] auto abs = [](int x) { return x < 0 ? -x : x; };
  
  {
    [[maybe_unused]] auto pred = [](int x) { return x == 3; };
//    constexpr auto i1 = std::ranges::find_last_if(v.begin(), v.end(), pred, abs);
//    constexpr auto i2 = std::ranges::find_last_if(v, pred, abs);
//    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
//    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
    [[maybe_unused]] auto pred = [](int x) { return x == -3; };
//    constexpr auto i1 = std::ranges::find_last_if(v.begin(), v.end(), pred, abs);
//    constexpr auto i2 = std::ranges::find_last_if(v, pred, abs);
//    static_assert(i1.begin() == v.end());
//    static_assert(i2.begin() == v.end());
  }
  
  {
    [[maybe_unused]] auto pred = [](int x) { return x == 1 or x == 2; };
//    constexpr auto i1 = std::ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
//    constexpr auto i2 = std::ranges::find_last_if_not(v, pred, abs);
//    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
//    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
    [[maybe_unused]] auto pred = [](int x) { return x == 1 or x == 2 or x == 3; };
//    constexpr auto i1 = std::ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
//    constexpr auto i2 = std::ranges::find_last_if_not(v, pred, abs);
//    static_assert(i1.begin() == v.end());
//    static_assert(i2.begin() == v.end());
  }
  
  using P = std::pair<std::string_view, int>;
  std::forward_list<P> list
  {
    {"one", 1}, {"two", 2}, {"three", 3},
    {"one", 4}, {"two", 5}, {"three", 6},
  };
  [[maybe_unused]] auto cmp_one = [](const std::string_view &s) { return s == "one"; };
  
  // find latest element that satisfy the comparator, and projecting pair::first
//  const auto subrange = std::ranges::find_last_if(list, cmp_one, &P::first);
  
  std::cout << "The found element and the tail after it are:\n";
//  for (P const& e : subrange)
//    std::cout << '{' << std::quoted(e.first) << ", " << e.second << "} ";
  std::cout << '\n';
  
//  const auto i3 = ranges::find_last(list, P{"three", 3});
//  assert(i3.begin()->first == "three" && i3.begin()->second == 3);
  
  // Output:
  //  The found element and the tail after it are:
  //  {"one", 4} {"two", 5} {"three", 6}
}


template <typename Proj = std::identity>
void println(auto comment, std::ranges::input_range auto&& range, Proj proj = {})
{
  for (std::cout << comment; auto const &element : range)
    std::cout << proj(element) << ' ';
  std::cout << '\n';
}

void range_iota()
{
  std::list<int> list(8);
  
  // Fill the list with ascending values: 0, 1, 2, ..., 7
  //std::ranges::iota(list, 0);
  std::println("{}", list);
  
  // A vector of iterators (see the comment to Example)
  std::vector<std::list<int>::iterator> vec(list.size());
  
  // Fill with iterators to consecutive list's elements
//  std::ranges::iota(vec.begin(), vec.end(), list.begin());
  
//  std::ranges::shuffle(vec, std::mt19937 {std::random_device {}()});
//  println("List viewed via vector: ", vec, [](auto it) { return *it; });
}



template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
  for (const auto& s : v)
  {
    if constexpr (std::is_same_v<T, std::string>)
      os << (s.empty() ? "." : s) << ' ';
    else
      os << s << ' ';
  }
  return os;
}

void range_shift()
{
  std::cout << std::left;
  
  std::vector<int> b{1, 2, 3, 4, 5, 6, 7};
  std::vector<std::string> c{"α", "β", "γ", "δ", "ε", "ζ", "η"};
  
  std::cout << "vector<int> \tvector<string>\n";
  std::cout << b << "  " << c << '\n';
  
//  std::ranges::shift_left(b, 3);
//  std::ranges::shift_left(c, 3);
  std::cout << b << "  " << c << '\n';
  
//  std::ranges::shift_right(b, 2);
//  std::ranges::shift_right(c, 2);
  std::cout << b << "  " << c << '\n';
  
  // has no effect: n >= last - first
//  std::ranges::shift_left(b, 8);
//  std::ranges::shift_left(c, 8);
  std::cout << b << "  " << c << '\n';
  
  //  std::ranges::shift_left(a, -3); // UB
}


void range_fold()
{
  const std::vector v{1, 2, 3, 4, 5, 6, 7, 8};
  
  const int sum = std::ranges::fold_left(v.cbegin(), v.cend(), 0, std::plus<int>());
  std::cout << "sum: " << sum << std::endl;
  
  const int mul = std::ranges::fold_left(v, 1, std::multiplies<int>());
  std::cout << "mul: " << mul << std::endl;
  
  // Get the product of the std::pair::second of all pairs in the vector:
  std::vector<std::pair<char, float>> data {{'A', 2.f}, {'B', 3.f}, {'C', 3.5f}};
  const float sec = std::ranges::fold_left
  (data | std::ranges::views::values, 2.0f, std::multiplies<>());
  std::cout << "sec: " << sec << std::endl;
  
  // Use a program defined function object (lambda-expression):
  std::string str = std::ranges::fold_left
  (v, "A", [](std::string s, int x) { return s + ':' + std::to_string(x); });
  std::cout << "str: " << str << std::endl;
  
  using CD = std::complex<double>;
  std::vector<CD> nums{{1, 1}, {2, 0}, {3, 0}};
  auto res = std::ranges::fold_left(nums, CD{7, 0}, std::multiplies{});
  std::cout << "res: " << res << std::endl;
}


// A non-owning multidimensional array reference.
// std::mdspan is a multidimensional array view
// that maps a multidimensional index to an element of the array.
// The mapping and element access policies are configurable,
// and the underlying array need not be contiguous or even exist in memory at all.
void mdspan()
{
  std::vector v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  
  // View data as contiguous memory representing 2 rows of 6 ints each.
  auto ms2 = std::mdspan(v.data(), 2, 6);
  
  // View the same data as a 3D array 2 x 3 x 2.
  auto ms3 = std::mdspan(v.data(), 2, 3, 2);

  // Write data using 2D view.
  for (std::size_t i = 0; i != ms2.extent(0); i++)
    for (std::size_t j = 0; j != ms2.extent(1); j++)
      ms2[i, j] = i * 1000 + j;
  
  // Read back using 3D view.
  for (std::size_t i = 0; i != ms3.extent(0); i++) {
    std::println("slice @ i = {}", i);
    for (std::size_t j = 0; j != ms3.extent(1); j++) {
      for (std::size_t k = 0; k != ms3.extent(2); k++)
        std::print("{} ", ms3[i, j, k]);
      std::println("");
    }
  }
}


// The flat set is a container adaptor
// that gives the functionality of an associative container
// that stores a sorted set of unique objects of type Key.
// Sorting is done using the key comparison function Compare.
void flat_set()
{
  // Does not yet compile on macOS June 2025.
}


// The flat map is a container adaptor
// that gives the functionality of an associative container
// that contains key-value pairs with unique keys.
// Keys are sorted by using the comparison function Compare.
void flat_map()
// Does not yet compile on macOS June 2025.
{
}


void type_traits()
{
  {
    static_assert(std::is_scoped_enum_v<int> == false);
    
    class A {};
    static_assert(std::is_scoped_enum_v<A> == false);
    
    enum B { self_test = std::is_scoped_enum_v<B> };
    static_assert(std::is_scoped_enum_v<B> == false);
    static_assert(!self_test);
    
    enum struct C { };
    static_assert(std::is_scoped_enum_v<C> == true);
    
    enum class D : int { };
    static_assert(std::is_scoped_enum_v<D> == true);
    
    enum class E;
    static_assert(std::is_scoped_enum_v<E> == true);
  }
  
  
  // The following types are collectively called implicit-lifetime types:
  // * scalar types:
  //     * arithmetic types
  //     * enumeration types
  //     * pointer types
  //     * pointer-to-member types
  //     * std::nullptr_t
  // * implicit-lifetime class types
  //     * is an aggregate whose destructor is not user-provided
  //     * has at least one trivial eligible constructor and a trivial,
  //       non-deleted destructor
  // * array types
  // * cv-qualified versions of these types.
  
//  static_assert(std::is_implicit_lifetime_v<int>); // arithmetic type is a scalar type
//  static_assert(std::is_implicit_lifetime_v<const int>); // cv-qualified a scalar type
  
  enum E { e };
//  static_assert(std::is_implicit_lifetime_v<E>); // enumeration type is a scalar type
//  static_assert(std::is_implicit_lifetime_v<int*>); // pointer type is a scalar type
//  static_assert(std::is_implicit_lifetime_v<std::nullptr_t>); // scalar type
  
  struct S { int x, y; };
  // S is an implicit-lifetime class: an aggregate without user-provided destructor
//  static_assert(std::is_implicit_lifetime_v<S>);
  
//  static_assert(std::is_implicit_lifetime_v<int S::*>); // pointer-to-member
  
  struct X { ~X() = delete; };
  // X is not implicit-lifetime class due to deleted destructor
//  static_assert(!std::is_implicit_lifetime_v<X>);
  
//  static_assert(std::is_implicit_lifetime_v<int[8]>); // array type
//  static_assert(std::is_implicit_lifetime_v<volatile int[8]>); // cv-qualified array type

//  static_assert(std::reference_constructs_from_temporary_v<int&&, int> == true);
//  static_assert(std::reference_constructs_from_temporary_v<const int&, int> == true);
//  static_assert(std::reference_constructs_from_temporary_v<int&&, int&&> == false);
//  static_assert(std::reference_constructs_from_temporary_v<const int&, int&&> == false);
//  static_assert(std::reference_constructs_from_temporary_v<int&&, long&&> == true);
//  static_assert(std::reference_constructs_from_temporary_v<int&&, long> == true);
}


void printing()
{
  std::println("{2} {1}{0}!", 23, "C++", "Hello");  // overload (1)
  
  const auto tmp{std::filesystem::temp_directory_path() / "test.txt"};
  std::println("{}", tmp.string());
  if (std::FILE* stream{std::fopen(tmp.c_str(), "w")})
  {
    std::print(stream, "File: {}", tmp.string()); // overload (2)
    std::fclose(stream);
  }
}

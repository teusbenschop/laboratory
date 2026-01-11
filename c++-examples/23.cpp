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

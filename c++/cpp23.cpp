/* Todo



void views_zip()
{
  return;

  const auto print = [] (auto const remark, auto const& range) {
    std::cout << remark;
    for (auto const& element : range)
      std::cout << element << ' ';
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

  for (std::tuple<int&, std::string&, char&> element : std::views::zip(x, y, z))
  {
    std::cout
    << std::get<0>(element) << ' '
    << std::get<1>(element) << ' '
    << std::get<2>(element) << std::endl;

    std::get<char&>(element) += ('a' - 'A'); // modifies the element of z
  }

  std::cout << std::endl;

  print("After modification, z: ", z);
}


void views_zip_transform()
{
  return;

  const auto print = [](auto const rem, auto const& r) {
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


// https://en.cppreference.com/w/cpp/ranges/range_adaptor_closure.html
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

  constexpr std::string_view str = "0123456789";

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


// Convert a range to a container like a vector, list, and so on.
void ranges_to()
{
  return;

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
    std::println("{}, size {}", str_to, str_to.size());

    // Equivalent to
    std::string str(array);

    // Result type is not an input range:
    auto re_to = std::ranges::to<std::regex>(array);

    // Equivalent to
    std::regex re(array);
  }
}


void ranges_find_last()
{
  return;

  constexpr static auto v = {1, 2, 3, 1, 2, 3, 1, 2};

  {
    constexpr auto i1 = std::ranges::find_last(v.begin(), v.end(), 3);
    constexpr auto i2 = std::ranges::find_last(v, 3);
    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
    constexpr auto i1 = std::ranges::find_last(v.begin(), v.end(), -3);
    constexpr auto i2 = std::ranges::find_last(v, -3);
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
  }

  constexpr const auto abs = [](int x) { return x < 0 ? -x : x; };

  {
    constexpr const auto pred = [](int x) { return x == 3; };
    constexpr auto i1 = std::ranges::find_last_if(v.begin(), v.end(), pred, abs);
    constexpr auto i2 = std::ranges::find_last_if(v, pred, abs);
    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
    constexpr const auto pred = [](int x) { return x == -3; };
    constexpr auto i1 = std::ranges::find_last_if(v.begin(), v.end(), pred, abs);
    constexpr auto i2 = std::ranges::find_last_if(v, pred, abs);
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
  }

  {
    constexpr const auto pred = [](int x) { return x == 1 or x == 2; };
    constexpr auto i1 = std::ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
    constexpr auto i2 = std::ranges::find_last_if_not(v, pred, abs);
    static_assert(std::ranges::distance(v.begin(), i1.begin()) == 5);
    static_assert(std::ranges::distance(v.begin(), i2.begin()) == 5);
  }
  {
    auto pred = [](int x) { return x == 1 or x == 2 or x == 3; };
    constexpr auto i1 = std::ranges::find_last_if_not(v.begin(), v.end(), pred, abs);
    constexpr auto i2 = std::ranges::find_last_if_not(v, pred, abs);
    static_assert(i1.begin() == v.end());
    static_assert(i2.begin() == v.end());
  }

  {
    using P = std::pair<std::string_view, int>;
    const std::forward_list<P> list {
      {"one", 1}, {"two", 2}, {"three", 3},
      {"one", 4}, {"two", 5}, {"three", 6},
    };
    [[maybe_unused]] auto cmp_one = [](const std::string_view &s) { return s == "one"; };

    // Find last elements that satisfy the comparator, and projecting pair::first
    const auto subrange = std::ranges::find_last_if(list, cmp_one, &P::first);

    std::cout << "The found element and the tail after it are:" << std::endl;
    for (P const& e : subrange)
      std::cout << '{' << std::quoted(e.first) << ", " << e.second << "} ";
    std::cout << std::endl;;
    // Output:
    //  The found element and the tail after it are:
    //  {"one", 4} {"two", 5} {"three", 6}

    const auto i3 = std::ranges::find_last(list, P{"three", 3});
    assert(i3.begin()->first == "three" && i3.begin()->second == 3);
  }
}


namespace std_identity {

struct Pair
{
  int n;
  std::string s;
  friend std::ostream& operator<<(std::ostream& os, const Pair& p) {
    return os << '{' << p.n << ", " << p.s << '}';
  }
};

// A range printer that can print projected (modified) elements of a range.
template<std::ranges::input_range R, typename Projection = std::identity> //<- Notice the default projection.
void demoprint(std::string_view const rem, R&& range, Projection projection = {})
{
  std::cout << rem << '{';
  const auto func = [O = 0](const auto& o) mutable {
    std::cout << (O++ ? ", " : "") << o;
  };
  std::ranges::for_each(range, func, projection);
  std::cout << "}" << std::endl;
}

void demo_identity()
{
  return;
  const auto v = {Pair {1, "one"}, {2, "two"}, {3, "three"} };
  demoprint("Print using std::identity as a projection: ", v);
  demoprint("Project the Pair::n: ", v, &Pair::n);
  demoprint("Project the Pair::s: ", v, &Pair::s);
  demoprint("Print using custom closure as a projection: ", v, [](Pair const& p) {
    return std::to_string(p.n) + ':' + p.s;
  });
}

}

namespace range_shift {

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

void demo_range_shift()
{
  return;

  std::cout << std::left;

  std::vector<int>         b{ 1,   2,   3,   4,   5,   6,   7};
  std::vector<std::string> c{"α", "β", "γ", "δ", "ε", "ζ", "η"};

  std::cout << "vector<int> \tvector<string>" << std::endl;
  std::cout << b << "  " << c << std::endl;

  //  std::ranges::shift_left(b, 3);
  //  std::ranges::shift_left(c, 3);
  std::cout << b << "  " << c << std::endl;

  //  std::ranges::shift_right(b, 2);
  //  std::ranges::shift_right(c, 2);
  std::cout << b << "  " << c << std::endl;

  // has no effect: n >= last - first
  //  std::ranges::shift_left(b, 8);
  //  std::ranges::shift_left(c, 8);
  std::cout << b << "  " << c << std::endl;

  //  std::ranges::shift_left(a, -3); // UB
}

}


void demo_range_fold()
{
  return;

  const std::vector v{1, 2, 3, 4, 5, 6, 7, 8};

  const int sum = std::ranges::fold_left(v.cbegin(), v.cend(), 0, std::plus<int>());
  std::cout << "sum: " << sum << std::endl;

  const int mul = std::ranges::fold_left(v, 1, std::multiplies<int>());
  std::cout << "product: " << mul << std::endl;

  // Get the product of the std::pair::second of all pairs in the vector:
  std::vector<std::pair<char, float>> data {{'A', 2.f}, {'B', 3.f}, {'C', 3.5f}};
  const float sec = std::ranges::fold_left
  (data | std::ranges::views::values, 2.0f, std::multiplies<>());
  std::cout << "product of second: " << sec << std::endl;

  // Use a program defined function object (lambda-expression):
  const auto program = [](std::string s, int x) {
    return s + ':' + std::to_string(x);
  };
  std::string str = std::ranges::fold_left(v, "A", program);
  std::cout << "string: " << str << std::endl;

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
void demo_multi_dimensional_span()
{
  return;

  std::vector v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  // View data as contiguous memory representing 2 rows of 6 integers each.
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
{
  // Does not yet compile on macOS June 2025.
}


void demo_print()
{
  return;
  std::println("{2} {1}{0}!", 23, "C++", "Hello");  // overload (1)

  const auto tmp{std::filesystem::temp_directory_path() / "test.txt"};
  std::println("{}", tmp.string());
  if (std::FILE* stream{std::fopen(tmp.c_str(), "w")})
  {
    std::print(stream, "File: {}", tmp.string()); // overload (2)
    std::fclose(stream);
  }
}

*/

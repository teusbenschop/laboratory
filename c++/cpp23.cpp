/* Todo



namespace std_identity {


void demo_identity()
{
  return;
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

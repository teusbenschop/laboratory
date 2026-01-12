

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

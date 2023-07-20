#include <version>
#include <iostream>
#include <functional>
#include <chrono>
#include <array>
#include <set>
#include <algorithm>
#include <ranges>
#include <list>
#include <numeric>
#include <sstream>
#include <memory>
#include <cstdint>
#include <type_traits>
#include <cmath>
#include <concepts>
#include <unordered_map>
#include <optional>

[[maybe_unused]] static void test_lambda_capture ()
{
  // Capture by value.
  {
    auto v = 7;
    // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
    // Mark it mutable because it mutates the captured v.
    auto lambda = [v]() mutable {
      std::cout << v << std::endl;
      ++v;
    };
    lambda();
    lambda();
    std::cout << v << std::endl;
  }
  
  // Capture by reference.
  {
    auto v = 7;
    auto lambda = [&v]() {
      std::cout << v << std::endl;
      ++v;
    };
    lambda();
    lambda();
    std::cout << v << std::endl;
  }
}

[[maybe_unused]] static void assign_two_lambdas_to_same_function_object ()
{
  // Create an unassigned std::function object.
  auto func = std::function<void(int)>{};
  
  // Assign a lambda without capture to the std::function object
  func = [](int v) { std::cout << v << std::endl; };
  func(12); // Prints 12.
  
  // Assign a lambda with capture to the same std::function object.
  auto forty_two = 42;
  func = [forty_two](int v) { std::cout << (v + forty_two) << std::endl; };
  func(12); // Prints 54.
}

// Implementing a button class with std::function.
class button1 {
public:
  button1(std::function<void(void)> click) : m_handler{click} {}
  auto on_click() const { m_handler(); }
private:
  std::function<void(void)> m_handler{};
};

static auto create_buttons() {
  auto beep = button1([counter = 0]() mutable {
    std::cout << "Beep:" << counter << std::endl;
    ++counter;
  });
  auto bop = button1([] {
    std::cout << "Bop:" << std::endl;
  });
  auto silent = button1([] {
  });
  
  return std::vector<button1>{beep, bop, silent};
}

[[maybe_unused]] static void std_function_with_button_class ()
{
  const auto& buttons = create_buttons();
  for (const auto& b : buttons) {
    b.on_click();
  }
  buttons.front().on_click(); // counter has been incremented
  // Output "Beep: 0! Bop. Beep: 1!"
}


// Requires C++20
[[maybe_unused]] static void stateles_lambdas() {
  auto x = [] {}; // A lambda without captures
  auto y = x;     // Assignable
  decltype(y) z;  // Default-constructible
  static_assert(std::is_same_v<decltype(x), decltype(y)>); // passes
  static_assert(std::is_same_v<decltype(x), decltype(z)>); // passes
}

[[maybe_unused]] static void lambda_auto_typename() {
  // Using auto
  auto x = [](auto v) { return v + 1; };
  
  // Using typename
  auto y = []<typename T>(T v) { return v + 1; };
  
  {
    // Using char
    auto v = 'a';
    std::cout << v + 1 << " " << x(v) << std::endl;
    std::cout << v + 1 << " " << y(v) << std::endl;
  }
  
  {
    // Using int
    std::cout << x(41) << std::endl;
    std::cout << y(41) << std::endl;
  }
  {
    // Using
    auto v = std::uint64_t{41};
    std::cout << v + 1 << " " << x(v) << std::endl;
    std::cout << v + 1 << " " << y(v) << std::endl;
  }
}

class scoped_timer {
  
public:
  using clock_type = std::chrono::steady_clock;
  scoped_timer(const char* func) : function_{func}, m_start{clock_type::now()} {}
  scoped_timer(const scoped_timer&) = delete;
  scoped_timer(scoped_timer&&) = delete;
  auto operator=(const scoped_timer&) -> scoped_timer& = delete;
  auto operator=(scoped_timer &&) -> scoped_timer& = delete;
  ~scoped_timer() {
    using namespace std::chrono;
    auto stop = clock_type::now();
    auto duration = (stop - m_start);
    auto ms = duration_cast<nanoseconds>(duration).count();
    std::cout << ms << " nanoseconds " << function_ << '\n';
  }
private:
  const char* function_ = {};
  const clock_type::time_point m_start = {};
};

[[maybe_unused]] static void benchmarking_function()
{
  [[maybe_unused]] auto scoped_timer ("test");
  for (unsigned int i = 0; i < 10000; i++) {
    std::cout << ".";
  }
  std::cout << std::endl;
}

// This example demonstrates cache thrashing.
// Changing the way we access elements in
// a matrix has a huge effect on the
// performance.

// The L1 Data cache size, replace this number with the
// L1 Data cache size on your machine
constexpr auto k_l1_cache_capacity = 32768u;
constexpr auto k_size = k_l1_cache_capacity / sizeof(int);
using matrix_type = std::array<std::array<int, k_size>, k_size>;

[[maybe_unused]] static auto cache_thrashing_fast(matrix_type& matrix) {
  auto counter = 0;
  for (auto i = 0u; i < k_size; ++i) {
    for (auto j = 0u; j < k_size; ++j) {
      matrix[i][j] = counter++;
    }
  }
}

[[maybe_unused]] static auto cache_thrashing_slow(matrix_type& matrix) {
  auto counter = 0;
  for (auto i = 0u; i < k_size; ++i) {
    for (auto j = 0u; j < k_size; ++j) {
      matrix[j][i] = counter++; // Slow due to cache thrashing
    }
  }
}

// Use static storage, since this matrix
// might not fit on the stack.
matrix_type m;

[[maybe_unused]] static void test_contains()
{
  auto bag = std::multiset<std::string>{};
  std::string word {"word"};
  bag.insert(word);
  std::cout << bag.contains(word) << std::endl;
}

//
// This example demonstrates that traversing a contiguous array
// of small objects is faster than traversing an array of big objects.
//

struct small_object {
  std::array<char, 4> data_{};
  int score_{};
  small_object() : score_{std::rand()} {}
};

struct big_object {
  std::array<char, 256> data_{};
  int score_{};
  big_object() : score_{std::rand()} {}
};

template <class T>
auto sum_scores(const std::vector<T>& objects) {
  scoped_timer t{"sum_scores"};
  auto sum = 0ul;
  for (const auto& obj : objects) {
    sum += static_cast<size_t>(obj.score_);
  }
  return sum;
}

[[maybe_unused]] static void sum_scores_compare_processing_time ()
{
  std::cout << "sizeof(small_object): " << sizeof(small_object) << " bytes" << std::endl;
  std::cout << "sizeof(big_object): " << sizeof(big_object) << " bytes" << std::endl;
  
  auto num_objects = 1'000'000ul;
  auto small_objects = std::vector<small_object>(num_objects);
  auto big_objects = std::vector<big_object>(num_objects);
  
  std::cout << "+++ sum_scores using small_object +++" << std::endl;
  auto small_sum = 0ul;
  small_sum += sum_scores(small_objects);
  small_sum += sum_scores(small_objects);
  small_sum += sum_scores(small_objects);
  small_sum += sum_scores(small_objects);
  
  std::cout << "+++ sum_scores using big_object +++" << std::endl;
  auto big_sum = 0ul;
  big_sum += sum_scores(big_objects);
  big_sum += sum_scores(big_objects);
  big_sum += sum_scores(big_objects);
  big_sum += sum_scores(big_objects);
  
  std::cout << "small sum: " << small_sum << std::endl;
  std::cout << "big sum: " << big_sum << std::endl;
  std::cout << "total sum: " << small_sum + big_sum << std::endl;
}

[[maybe_unused]] static void sorting ()
{
  {
    auto values = std::vector{6, 3, 2, 7, 4, 1, 5};
    std::sort(values.begin(), values.end());
    std::cout << (std::ranges::is_sorted(values) ? "sorted" : "unsorted") << std::endl;
  }
  {
    auto v = std::vector{6, 3, 2, 7, 4, 1, 5};
    std::ranges::sort(v);
    std::cout << (std::ranges::is_sorted(v) ? "sorted" : "unsorted") << std::endl;
  }
}

[[maybe_unused]] static void print_ranges_for_each (auto&& r) {
  std::ranges::for_each(r, [](auto&& i) {
    std::cout << i << ' ';
  });
  std::cout << std::endl;
}

[[maybe_unused]] static void algorithm_transform()
{
  auto input = std::vector<int>{1, 2, 3, 4};
  auto output = std::vector<int>(input.size());
  auto lambda = [](auto&& i) { return i * i; };
  std::ranges::transform(input, output.begin(), lambda);
  print_ranges_for_each(output);
}

[[maybe_unused]] static void algorithm_fill()
{
  std::vector<int> v(5);
  std::ranges::fill(v, -123);
  print_ranges_for_each(v);
}

[[maybe_unused]] static void algorithm_generate()
{
  std::vector<int> v(4);
  std::ranges::generate(v, std::rand);
  print_ranges_for_each(v);
}

[[maybe_unused]] static void algorithm_iota()
{
  auto v = std::vector<int>(6);
  std::iota(v.begin(), v.end(), 0);
  print_ranges_for_each(v);
  // Output: 0 1 2 3 4 5
}

[[maybe_unused]] static void algorithm_find()
{
  auto col = std::list{2, 4, 3, 2, 3, 1};
  auto it = std::ranges::find(col, 2);
  if (it != col.end()) {
    std::cout << *it << '\n';
  }
}

[[maybe_unused]] static void algorithm_binary_search()
{
  auto v = std::vector<int>{2, 2, 3, 3, 3, 4, 5};
  // A binary search works if the container is sorted.
  std::cout << std::boolalpha << std::ranges::is_sorted(v) << std::endl;
  bool found = std::ranges::binary_search(v, 3);
  std::cout << std::boolalpha << found << std::endl; // Output: true
}

[[maybe_unused]] static void algorithm_lower_upper_bound()
{
  auto v = std::vector<int>{2, 2, 3, 3, 3, 4, 5};
  {
    auto it = std::ranges::lower_bound(v, 3);
    if (it != v.end()) {
      auto index = std::distance(v.begin(), it);
      std::cout << "Position: " << index << '\n';
    }
  }
  {
    auto it = std::ranges::upper_bound(v, 3);
    if (it != v.end()) {
      auto index = std::distance(v.begin(), it);
      std::cout << "Position: " << index << '\n';
    }
    // Output: Position: 5
  }
}

[[maybe_unused]] static void algorithm_all_any_none_of()
{
  const auto v = std::vector<int>{3, 2, 2, 1, 0, 2, 1};
  print_ranges_for_each(v);

  const auto is_negative = [](int i) { return i < 0; };
  
  std::cout << "Contains only natural numbers: " << std::boolalpha << std::ranges::none_of(v, is_negative) << std::endl;

  std::cout << "Contains only negative numbers: " << std::boolalpha << std::ranges::all_of(v, is_negative) << std::endl;

  std::cout << "Contains at least one negative number: " << std::boolalpha << std::ranges::any_of(v, is_negative) << std::endl;
}

[[maybe_unused]] static void algorithm_count()
{
  auto v = std::list{3, 3, 2, 1, 3, 1, 3};
  auto n = std::ranges::count(v, 3);
  std::cout << n << std::endl; // Output: 4
}

static auto some_func() { return 200; }

[[maybe_unused]] static void algorithm_min_max_clamp()
{
  const auto y_max = 100;
  const auto y_min = 10;
  std::cout << std::min(some_func(), y_max) << std::endl;
  std::cout << std::max(some_func(), y_min) << std::endl;
  std::cout << std::clamp(some_func(), y_min, y_max) << std::endl;
}

[[maybe_unused]] static void algorithm_minmax()
{
  const auto v = std::vector<int>{4, 2, 1, 7, 3, 1, 5};
  const auto [min, max] = std::ranges::minmax(v);
  std::cout << min << " " << max << std::endl; // Output: 1 7
}

[[maybe_unused]] static void algorithm_projections()
{
  auto names = std::vector<std::string>{"Ralph",  "Lisa", "Homer", "Maggie", "Apu",  "Bart"};
  std::ranges::sort(names, std::less<>{}, &std::string::size);
  // names is now "Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"
  // Find names with length 3
  auto iterator = std::ranges::find(names, 3, &std::string::size);
  std::cout << "Find first name with length 3: " << *iterator << std::endl;
}

[[maybe_unused]] static void algorithm_lambda_projections()
{
  struct Player {
    std::string name{};
    int level{};
    float health{1.0f};
  };
  auto players = std::vector<Player>{
    {"Aki", 1, 9.f},
    {"Nao", 2, 7.f},
    {"Rei", 2, 3.f}};
  const auto level_and_health = [](const Player& p) {
    return std::tie(p.level, p.health);
  };
  // Order players by level, then health
  std::ranges::sort(players, std::less<>{}, level_and_health);
  std::ranges::for_each(players, [](auto&& player) {
    std::cout << player.name << " " << player.level << " " << player.health << std::endl;
  });
}

template <typename Iterator>
auto contains_duplicates_n2(Iterator begin, Iterator end) {
  for (auto it = begin; it != end; ++it)
    if (std::find(std::next(it), end, *it) != end)
      return true;
  return false;
}

template <typename Iterator>
auto contains_duplicates_allocating(Iterator first, Iterator last) {
  // As (*begin) returns a reference, we have to get the base type using std::decay_t
  using ValueType = std::decay_t<decltype(*first)>;
  auto copy = std::vector<ValueType>(first, last);
  std::sort(copy.begin(), copy.end());
  return std::adjacent_find(copy.begin(), copy.end()) != copy.end();
}


[[maybe_unused]] static void algorithm_contains_duplicates()
{
  auto vals = std::vector{1,4,2,5,3,6,4,7,5,8,6,9,0};
  {
    scoped_timer ("a");
    auto a = contains_duplicates_n2(vals.begin(), vals.end());
    std::cout << "Contains duplicates: " << std::boolalpha << a << std::endl;
  }
  {
    scoped_timer ("b");
    auto b = contains_duplicates_allocating(vals.begin(), vals.end());
    std::cout << "Contains duplicates: " << std::boolalpha << b << std::endl;
  }
}

struct Grid {
  Grid(int w, int h)
  : m_width{w}, m_height{h}
  {
    m_data.resize(static_cast<size_t>(w * h));
  }
  auto get_row_v1(int y); // Returns iterator pairs
  auto get_row_v2(int y); // Returns range using subrange
  auto get_row_v3(int y); // Returns range using counted
  std::vector<int> m_data{};
  int m_width{};
  int m_height{};
};

auto Grid::get_row_v1(int y) {
  auto left = m_data.begin() + m_width * y;
  auto right = left + m_width;
  return std::make_pair(left, right);
}

auto Grid::get_row_v2(int y) {
  auto first = m_data.begin() + m_width * y;
  auto sentinel = first + m_width;
  return std::ranges::subrange{first, sentinel};
}

auto Grid::get_row_v3(int y) {
  auto first = m_data.begin() + m_width * y;
  return std::views::counted(first, m_width);
}

[[maybe_unused]] static void algorithm_grid()
{
  {
    auto grid = Grid{10, 10};
    auto y = 3;
    auto row = grid.get_row_v1(y);
    std::generate(row.first, row.second, [](){return 5;});
    auto num_fives = std::count(row.first, row.second, 5);
    std::cout << "number of fives: " << num_fives << std::endl;
  }
  {
    auto grid = Grid{10, 10};
    auto y = 3;
    auto row = grid.get_row_v2(y);
    std::ranges::generate(row, std::rand);
    auto num_fives = std::ranges::count(row, 5);
    std::cout << "number of fives: " << num_fives << std::endl;
  }
  {
    auto grid = Grid{10, 10};
    auto y = 3;
    auto row = grid.get_row_v3(y);
    std::ranges::generate(row, std::rand);
    auto num_fives = std::ranges::count(row, 5);
    std::cout << "number of fives: " << num_fives << std::endl;
  }
}

struct student {
  int year{};
  int score{};
  std::string name{};
};

[[maybe_unused]] static auto get_max_score_copy(const std::vector<student>& students, int year)
{
  auto by_year = [=](const auto& s) { return s.year == year; };
  // The student list needs to be copied in order to filter on the year.
  auto v = std::vector<student>{};
  std::ranges::copy_if(students, std::back_inserter(v), by_year);
  auto it = std::ranges::max_element(v, std::less{}, &student::score);
  return it != v.end() ? it->score : 0;
}

[[maybe_unused]] static auto get_max_score_for_loop(const std::vector<student>& students, int year) {
  auto max_score = 0;
  for (const auto& student : students) {
    if (student.year == year) {
      max_score = std::max(max_score, student.score);
    }
  }
  return max_score;
}

[[maybe_unused]] static auto max_value(auto&& range) {
  const auto it = std::ranges::max_element(range);
  return it != range.end() ? *it : 0;
}

[[maybe_unused]] static auto get_max_score(const std::vector<student>& students, int year)
{
  const auto by_year = [=](auto&& s) { return s.year == year; };
  return max_value(students | std::views::filter(by_year) | std::views::transform(&student::score));
}

[[maybe_unused]] static auto get_max_score_explicit_views(const std::vector<student>& s, int year) {
  auto by_year = [=](const auto& s) { return s.year == year; };
  auto v1 = std::ranges::ref_view{s}; // Wrap container in a view.
  auto v2 = std::ranges::filter_view{v1, by_year};
  auto v3 = std::ranges::transform_view{v2, &student::score};
  auto it = std::ranges::max_element(v3);
  return it != v3.end() ? *it : 0;
}

[[maybe_unused]] static void algorithm_composability()
{
  auto students = std::vector<student>{
    {3, 120, "A"},
    {2, 140, "B"},
    {3, 190, "C"},
    {2, 110, "D"},
    {2, 120, "E"},
    {3, 130, "F"},
  };
  {
    scoped_timer ("copy");
    auto score = get_max_score_copy(students, 2); // score is 140
    std::cout << score << std::endl;
  }
  {
    scoped_timer ("for loop");
    auto score = get_max_score_for_loop(students, 2); // score is 140
    std::cout << score << std::endl;
  }
  {
    scoped_timer ("composed");
    auto score = get_max_score(students, 2);
    std::cout << score << std::endl;
  }
  {
    scoped_timer ("explicit");
    auto score = get_max_score_explicit_views(students, 2);
    std::cout << score << std::endl;
  }
}

[[maybe_unused]] static void algorithm_views_transform()
{
  auto numbers = std::vector {1, 2, 3, 4, 5, 6, 7, 8};
  auto square = [](auto v) { return v * v; };
  // Create a view, but do not yet evaluate this view.
  auto squared_view = std::views::transform(numbers, square);
  // Iterate over the squared view, which invokes evaluation and so invokes the lambda.
  for (auto s : squared_view) std::cout << s << " ";
  std::cout << std::endl;
}

[[maybe_unused]] static void algorithm_views_filter()
{
  auto v = std::vector{4, 5, 6, 7, 6, 5, 4};
  auto odd_view = std::views::filter(v, [](auto i) { return (i % 2) == 1; });
  for (auto odd_number : odd_view) std::cout << odd_number << " ";
  // Output: 5 7 5
  std::cout << std::endl;
}

[[maybe_unused]] static void algorithm_views_join()
{
  auto list_of_lists =
  std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}, {5}, {4, 3, 2, 1}};
  auto flattened_view = std::views::join(list_of_lists);
  for (auto v : flattened_view) std::cout << v << " ";
  // Output: 1 2 3 4 5 5 4 3 2 1
  std::cout << std::endl;
  
  auto max_value = *std::ranges::max_element(flattened_view);
  // max_value is 5
  std::cout << max_value << std::endl;
}

// Materialize the range r into a std::vector
// See also https://timur.audio/how-to-make-a-container-from-a-c20-range
[[maybe_unused]] static auto to_vector(auto&& r) {
  std::vector<std::ranges::range_value_t<decltype(r)>> v;
  if constexpr (std::ranges::sized_range<decltype(r)>) {
    v.reserve(std::ranges::size(r));
  }
  std::ranges::copy(r, std::back_inserter(v));
  return v;
}

[[maybe_unused]] static void algorithm_views_materialize()
{
  auto ints = std::list{2, 3, 4, 2, 1};
  auto r = ints | std::views::transform([](auto i) { return std::to_string(i); });
  auto strings = to_vector(r);
  print_ranges_for_each (strings);
}

template<typename T>
class type_deduction;

[[maybe_unused]] static void algorithm_views_take()
{
  auto vec = std::vector<int>{4, 2, 7, 1, 2, 6, 1, 5};
  auto first_half = vec | std::views::take(vec.size());
  std::ranges::sort(first_half);
  // vec is now 1, 2, 4, 7, 2, 6, 1, 5
  print_ranges_for_each (first_half);
  //type_deduction<decltype(first_half)> compiler_error;
}

[[maybe_unused]] static void algorithm_views_split_join()
{
  auto csv = std::string{"10,11,12"};
//  auto digits = csv | std::ranges::views::split(',') // [ [1, 0], [1, 1], [1, 2] ]
//  | std::views::join;          // [ 1, 0, 1, 1, 1, 2 ]
  
//  for (auto i : digits) {
//    std::cout << i;
//  }
  // Prints 101112
}

[[maybe_unused]] static void algorithm_views_sampling()
{
  auto vec = std::vector{1, 2, 3, 4, 5, 4, 3, 2, 1};
//  auto v = vec | std::views::drop_while([](auto i) { return i < 5; }) |
//  std::views::take(3);
//  // Prints 5 4 3
//  for (const auto& i : v) {
//    std::cout << i << " ";
//  }
  std::cout << std::endl;
}

[[maybe_unused]] static void algorithm_views_stream()
{
  // auto ifs = std::ifstream("numbers.txt");
  // Using an istringstream instead of ifstream here
//  auto ifs = std::istringstream{"1.4142 1.618 2.71828 3.14159 6.283"};
//  for (auto f : std::ranges::istream_view<float>(ifs)) {
//    std::cout << f << '\n';
//  }
  // ifs.close();
}

struct User {
  User(const std::string& name) : name_(name) { }
  std::string name_;
};

[[maybe_unused]] static void memory_placement_new()
{
  {
    auto* user = new User {"John"};
    delete user;
  }
  {
    // Allocate memory.
    auto* memory = std::malloc(sizeof(User));
    // Construct new object in existing memory.
    auto* user = ::new (memory) User("john");
    std::cout << user->name_ << std::endl;
    // Call destructor.
    user->~User();
    // Free memory on heap.
    std::free(memory);
  }
  {
    auto* memory = std::malloc(sizeof(User));
    auto* user_ptr = reinterpret_cast<User*>(memory);
    std::uninitialized_fill_n(user_ptr, 1, User{"john"});
    std::construct_at (user_ptr, User{"john"}); // C++20.
    std::cout << user_ptr->name_ << std::endl;
    std::destroy_at(user_ptr);
    std::free(memory);
  }
}

template <typename T>
auto pow_n_traditional(const T& v, int n) {
  auto product = T{1};
  for (int i = 0; i < n; ++i) {
    product *= v;
  }
  return product;
}

auto pow_n_abbreviated (const auto&v, int n) {
  auto product = decltype(v){1};
  for (int i = 0; i < n; i++) {
    product *= v;
  }
  return product;
}

auto pow_n_remove_cfref (const auto&v, int n) {
  typename std::remove_cvref<decltype(v)>::type product {1};
  for (int i = 0; i < n; i++) {
    product *= v;
  }
  return product;
}

[[maybe_unused]] static void template_pow()
{
  auto x = pow_n_traditional<float>(2.0f, 3); // x is a float
  std::cout << x << std::endl;
  //type_deduction<decltype(x)> compiler_error;
  auto y = pow_n_traditional<int>(3, 3); // y is an int
  std::cout << y << std::endl;
  auto z = pow_n_abbreviated(3.0, 3);
  std::cout << z << std::endl;
  {
    auto pow_n_func = []<class T>(const T& v, int n) {
      auto product = T{1};
      for (int i = 0; i < n; ++i) {
        product *= v;
      }
      return product;
    };
    auto a = pow_n_func(3, 3); // x is an int
    std::cout << a << std::endl;

  }
}

template <typename T>
auto sign_func(const T& v) -> int {
  if (std::is_unsigned_v<T>) {
    return 1;
  }
  return v < 0 ? -1 : 1;
}

[[maybe_unused]] static void demo_type_traits()
{
  const auto same_type = std::is_same_v<uint8_t, unsigned char>;
  static_assert(same_type);

  const auto is_float_or_double = std::is_floating_point_v<decltype(3.f)>;
  static_assert(is_float_or_double);

  class Planet {};
  class Mars : public Planet {};
  class Sun {};
  static_assert(std::is_base_of_v<Planet, Mars>);
  static_assert(!std::is_base_of_v<Planet, Sun>);

  {
    auto unsigned_value = uint32_t{42};
    auto sign = sign_func(unsigned_value);
    std::cout << sign << std::endl; // 1
  }
  {
    auto signed_value = int32_t{-42};
    auto sign = sign_func(signed_value);
    std::cout << sign << std::endl; // -1
  }
}

[[maybe_unused]] static consteval auto consteval_sum (int x, int y)
{
  return x + y;
}

[[maybe_unused]] static void demo_consteval()
{
  [[maybe_unused]] constexpr auto sum = consteval_sum(1, 2);
  [[maybe_unused]] auto x = 1;
  // auto sum2 = consteval_sum(x, 2); // Fails to compile.
}

struct Bear {
  auto roar() const { std::cout << "roar\n"; }
};
struct Duck {
  auto quack() const { std::cout << "quack\n"; }
};

// The function speak can't be used with Bear and Duck class
template <typename Animal>
auto speak(const Animal& a) {
  if (std::is_same_v<Animal, Bear>) {
    a.roar();
  } else if (std::is_same_v<Animal, Duck>) {
    a.quack();
  }
}

template <typename Animal>
void constexpr_if_speak(const Animal& a) {
  if constexpr (std::is_same_v<Animal, Bear>) {
    a.roar();
  } else if constexpr (std::is_same_v<Animal, Duck>) {
    a.quack();
  }
}

template <typename T>
auto generic_mod(const T& v, const T& n) -> T {
  assert(n != 0);
  if (std::is_floating_point_v<T>) {
    return std::fmod(v, n);
  } else {
    return v % n; // if T is a floating point, this line wont compile
  }
}

template <typename T>
auto constexpr_if_generic_mod(const T& v, const T& n) -> T {
  assert(n != 0);
  if constexpr (std::is_floating_point_v<T>) {
    return std::fmod(v, n);
  } else {        // If T is a floating point,
    return v % n; // this code is eradicated
  }
}

[[maybe_unused]] static void demo_if_constexpr()
{
  {
    Bear bear;
    Duck duck;
    constexpr_if_speak(bear);
    constexpr_if_speak(duck);
    // Won't compile because constexpr if is not used.
    // speak(bear);
    // speak(duck);
  }
  {
    auto const value = 7;
    auto const modulus = 5;
    auto const result = generic_mod(value, modulus);
    auto const constexpr_if_result = constexpr_if_generic_mod(value, modulus);
    std::cout << result << " " << constexpr_if_result << std::endl;
  }
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    // Compilation error % operation for float value
    // auto const result = generic_mod(value, modulus);
    auto const constexpr_if_result = constexpr_if_generic_mod(value, modulus);
    std::cout << constexpr_if_result << std::endl;
  }
}

template <typename T>
class Point2D {
public:
  Point2D(T x, T y) : x_{x}, y_{y} {}
  auto x() { return x_; }
  auto y() { return y_; }
private:
  T x_{};
  T y_{};
};

template <typename T>
auto dist(T point1, T point2) {
  auto a = point1.x() - point2.x();
  auto b = point1.y() - point2.y();
  return std::sqrt(a * a + b * b);
};

[[maybe_unused]] static void point_template_unconstrained()
{
  {
    auto p1 = Point2D{2, 2};
    auto p2 = Point2D{6, 5};
    auto d = dist(p1, p2);
    std::cout << d << std::endl;
  }
  // The code below doesn't compile because type int doesn't have x() and y() functions.
  // Needed is better error messages when trying to instantiate this function template.
  // dist(3, 4);
  {
    // We don't want this to compile: Point2D<const char*>
    auto from = Point2D{"2.0", "2.0"};
    auto to = Point2D{"6.0", "5.0"};
    auto d = dist(from, to);
    // Prints nonsense.
    std::cout << d << std::endl;
  }
}

template <typename T>
concept floating_point = std::is_floating_point_v<T>;

template <typename T>
concept number = floating_point<T> || std::is_integral_v<T>;

template <typename T>
concept range = requires(T& t) {
  begin(t);
  end(t);
};

// Constraining types with concepts.

template <typename T>
requires std::integral<T> auto mod(T v, T n) {
  return v % n;
}

// Using concepts with abbreviated function templates.

std::integral auto mod_abbreviated(std::integral auto v, std::integral auto n) {
  return v % n;
}

template <typename T>
requires std::integral<T> struct struct_foo {
  T value;
};

// Use function overloading.

template <std::integral T>
T generic_mod(T v, T n) { // Integral version.
  return v % n;
}

template <std::floating_point T>
T generic_mod(T v, T n) { // Floating point version.
  return std::fmod(v, n);
}

[[maybe_unused]] static void constraints_and_concepts()
{
  {
    auto x = mod(5, 2); // OK: Integral types
    std::cout << "should be 1: " << x << std::endl;
    // Compilation error, not integral types
    // auto y = mod(5.f, 2.f);
  }
  {
    auto x = mod_abbreviated(5, 2); // OK: Integral types
    std::cout << "should be 1: " << x << std::endl;
    // Compilation error, not integral types
    // auto y = mod_abbreviated(5.f, 2.f);
  }
  {
    //auto x = struct_foo<int>{5};
    //std::cout << "should be 1: " << x << std::endl;
    // Compilation error, not integral types
    // auto y = Foo{5.f};
  }
  // Using an integral type.
  {
    auto const value = 7;
    auto const modulus = 5;
    auto const x = generic_mod(value, modulus);
    std::cout << "should be 2: " << x << std::endl;
  }
  // Using a floating point type.
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    auto const x = generic_mod(value, modulus);
    std::cout << "should be 0.5: " << x << std::endl;
  }
}

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept point = requires(T p) {
  requires std::is_same_v<decltype(p.x()), decltype(p.y())>;
  requires arithmetic<decltype(p.x())>;
};

std::floating_point auto dist (point auto p1, point auto p2) {
  auto a = p1.x() - p2.x();
  auto b = p1.y() - p2.y();
  return std::sqrt(a * a + b * b);
}

template <arithmetic T> // T is now constrained.
class Point2D_v2 {
public:
  Point2D_v2 (T x, T y) : x_{x}, y_{y} {}
  auto x() { return x_; }
  auto y() { return y_; }
  // …
private:
  T x_{};
  T y_{};
};

[[maybe_unused]] static void point2d_concepts()
{
  auto p1 = Point2D_v2{2, 2};
  auto p2 = Point2D_v2{6, 5};
  auto d = dist(p1, p2);
  std::cout << d << std::endl;
  // Trying to instatiate Point2D with unrelevant types is no longer possible.
  // auto p = Point2D_v2{"2.0", "2.0"};
}

template <typename T>
constexpr auto make_false() {
  return false;
}

template <typename Dst, typename Src>
auto safe_cast(const Src& v) -> Dst {
  constexpr auto is_same_type = std::is_same_v<Src, Dst>;
  constexpr auto is_pointer_to_pointer = std::is_pointer_v<Src> && std::is_pointer_v<Dst>;
  constexpr auto is_float_to_float = std::is_floating_point_v<Src> && std::is_floating_point_v<Dst>;
  constexpr auto is_number_to_number = std::is_arithmetic_v<Src> && std::is_arithmetic_v<Dst>;
  constexpr auto is_intptr_to_ptr = (std::is_same_v<uintptr_t, Src> || std::is_same_v<intptr_t, Src>) && std::is_pointer_v<Dst>;
  constexpr auto is_ptr_to_intptr = std::is_pointer_v<Src> && (std::is_same_v<uintptr_t, Dst> || std::is_same_v<intptr_t, Dst>);
  
  if constexpr (is_same_type) {
    return v;
  }
  else if constexpr (is_intptr_to_ptr || is_ptr_to_intptr) {
    return reinterpret_cast<Dst>(v);
  }
  else if constexpr (is_pointer_to_pointer) {
    assert(dynamic_cast<Dst>(v) != nullptr);
    return static_cast<Dst>(v);
  }
  else if constexpr (is_float_to_float) {
    auto casted = static_cast<Dst>(v);
    auto casted_back = static_cast<Src>(v);
    assert(!isnan(casted_back) && !isinf(casted_back));
    return casted;
  }
  else if constexpr (is_number_to_number) {
    auto casted = static_cast<Dst>(v);
    auto casted_back = static_cast<Src>(casted);
    assert(casted == casted_back);
    return casted;
  }
  else {
    static_assert(make_false<Src>(), "CastError");
    return Dst{}; // This can never happen, the static_assert should have failed
  }
}

[[maybe_unused]] static void demo_safe_cast()
{
  auto x = safe_cast<int>(42.0f);
  std::cout << x << std::endl;
  // Only compiles on a 16-bits system.
  //auto y = safe_cast<int*>(int16_t{42});
}

constexpr auto hash_function(const char* str) -> size_t {
  auto sum = size_t{0};
  for (auto ptr = str; *ptr != '\0'; ++ptr)
    sum += static_cast<size_t>(*ptr);
  return sum;
}

class PrehashedString {
public:
  template <size_t N>
  constexpr PrehashedString(const char (&str)[N])
  : m_hash{hash_function(&str[0])},
  m_size{N - 1}, // The subtraction is to avoid null at the end.
  m_strptr{&str[0]} {}
  auto operator==(const PrehashedString& s) const {
    return m_size == s.m_size && std::equal(c_str(), c_str() + m_size, s.c_str());
  }
  auto operator!=(const PrehashedString& s) const { return !(*this == s); }
  constexpr auto size() const { return m_size; }
  constexpr auto get_hash() const { return m_hash; }
  constexpr auto c_str() const -> const char* { return m_strptr; }
private:
  size_t m_hash{};
  size_t m_size{};
  const char* m_strptr{nullptr};
};

template <>
struct std::hash<PrehashedString> {
  constexpr auto operator()(const PrehashedString& s) const {
    return s.get_hash();
  }
};

[[maybe_unused]] static void compile_time_hash()
{
  const auto& hash_fn = std::hash<PrehashedString>{};
  const auto& str = PrehashedString("abc");
  std::cout << hash_fn(str) << std::endl;
  std::cout << hash_function("abc") << std::endl;
}

[[maybe_unused]] static void demo_optional()
{
  auto c = std::vector<std::optional<int>>{{3}, {}, {1}, {}, {2}};
  std::sort(c.begin(), c.end());
  const auto sorted = std::vector<std::optional<int>>{{}, {}, {1}, {2}, {3}};
  std::cout << "vectors are equal: " << std::boolalpha << (c==sorted) << std::endl;
}

int main()
{
  demo_optional();
  return EXIT_SUCCESS;
}

#include "main.h"
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
    auto ms = duration_cast<microseconds>(duration).count();
    std::cout << ms << " microseconds " << function_ << '\n';
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
    std::cout << "num_fives " << num_fives << std::endl;
  }
  {
    auto grid = Grid{10, 10};
    auto y = 3;
    auto row = grid.get_row_v3(y);
    std::ranges::generate(row, std::rand);
    auto num_fives = std::ranges::count(row, 5);
    std::cout << "num_fives " << num_fives << std::endl;
  }
}

int main()
{
  algorithm_grid();
  return 0;
}

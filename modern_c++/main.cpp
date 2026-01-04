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
#include <tuple>
#include <future>
#include <random>
#include <atomic>
#include <barrier>
#include <condition_variable>
#include <queue>
#include <latch>
#include <semaphore>
#include <coroutine>
#include <limits>
#include <cassert>
#include <iomanip>

[[maybe_unused]] void test_lambda_capture ()
{
  // Capture by value.
  {
    auto v = 7;
    // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
    // Mark it mutable because it mutates the captured v.
    auto lambda = [v]() mutable {
      std::cout << "Captured v = " << v << std::endl;
      ++v;
    };
    lambda();
    lambda();
    std::cout << "Original v = " << v << std::endl;
  }
  
  // Capture by reference.
  {
    auto v = 7;
    auto lambda = [&v]() {
      std::cout << "Captured v by reference = " << v << std::endl;
      ++v;
    };
    lambda();
    lambda();
    std::cout << "Original v = " << v << std::endl;
  }
}


[[maybe_unused]] void assign_two_lambdas_to_same_function_object ()
{
  // Create an unassigned std::function object.
  auto func = std::function<void(int)>{};
  
  // Assign a lambda without capture to the std::function object.
  func = [](int v) {
    std::cout << "Assigned lambda without capture: " << v << std::endl;
  };
  func(12); // Prints 12.
  
  // Assign a lambda with capture to the same std::function object.
  auto v42 = 42;
  func = [v42](int v) {
    std::cout << "Assigned lambda with capture: " << (v + v42) << std::endl;
  };
  func(12); // Prints 54.
}

// Implementing a button class with std::function.
class button1 {
public:
  button1(std::function<void(void)> click) : m_handler{click} {std::cout << "create object button1 with function click at " << &click << std::endl;}
  button1() {std::cout << "create object button1 wihthout parameters" << std::endl;}
  ~button1(){std::cout << "destroy object button1" << std::endl;}
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


[[maybe_unused]] void std_function_with_button_class()
{
  const auto buttons = create_buttons();
  for (const auto& b : buttons) {
    b.on_click();
  }
  buttons.front().on_click(); // counter has been incremented
  /* Output
   create object button1
   create object button1
   create object button1
   destroy object button1
   destroy object button1
   destroy object button1
   destroy object button1
   destroy object button1
   destroy object button1
   Beep:0
   Bop:
   Beep:1
   destroy object button1
   destroy object button1
   destroy object button1
   */
}


[[maybe_unused]] void stateless_lambdas() {
  // A stateless lambda (function) does not retain any data or memory
  // from one execution to the next.
  const auto x = [] {};
  // Assignable.
  const auto y = x;
  // Default-constructible (i.e. constructor without parameters, or with default parameters.
  const decltype(y) z;
  static_assert(std::is_same_v<decltype(x), decltype(y)>); // passes
  static_assert(std::is_same_v<decltype(x), decltype(z)>); // passes
  static_assert(std::is_same_v<decltype(y), decltype(z)>); // passes
}


[[maybe_unused]] void lambda_auto_typename()
{
  // Using auto.
  auto x = [](auto v) { return v + 1; };
  
  // Using typename.
  auto y = []<typename T>(T v) { return v + 1; };
  
  // Using char.
  std::cout << std::setw(13) << "type char " << x('a') << " " << y('a') << std::endl;
  
  // Using int.
  std::cout << std::setw(13) << "type int " << x(41) << " " << y(41) << std::endl;

  // Using unsigned int 64.
  std::cout << std::setw(13) << "type uint 64 " << x(static_cast<std::uint64_t>(41)) << " " << y(static_cast<std::uint64_t>(41)) << std::endl;
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


[[maybe_unused]] void benchmarking_function()
{
  {
    [[maybe_unused]] auto scoped_timer ("test");
    for (unsigned int i = 0; i < 10000; i++) {
      //std::cout << ".";
    }
    std::cout << std::endl;
  }
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


[[maybe_unused]] void test_contains()
{
  auto bag = std::multiset<std::string>{};
  std::string word {"word"};
  bag.insert(word);
  std::cout << "Multiset bag contains word: " << std::boolalpha << bag.contains(word) << std::endl;
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

[[maybe_unused]] void sum_scores_compare_processing_time ()
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


[[maybe_unused]] void sorting ()
{
  const auto values = std::vector{6, 3, 2, 7, 4, 1, 5};
  const auto print = [](const auto& values) {
    std::ranges::for_each(values, [](const auto& v) {std::cout << v << " ";});
    std::cout << (std::ranges::is_sorted(values) ? "sorted" : "unsorted") << std::endl;
  };
  print(values);
  auto vs = values;
  std::sort(vs.begin(), vs.end());
  print(vs);
  auto vrs = values;
  std::ranges::sort(vrs);
  print(vrs);
}


[[maybe_unused]] void print_ranges_for_each (auto&& r) {
  std::ranges::for_each(r, [](auto&& i) {
    std::cout << i << ' ';
  });
  std::cout << std::endl;
}


[[maybe_unused]] void algorithm_transform()
{
  auto input = std::vector<int>{1, 2, 3, 4};
  auto output = std::vector<int>(input.size());
  auto lambda = [](auto&& i) { return i * i; };
  std::ranges::transform(input, output.begin(), lambda);
  std::cout << "std::ranges::transform: ";
  print_ranges_for_each(output);
}


[[maybe_unused]] void algorithm_fill()
{
  std::vector<int> v(5);
  std::ranges::fill(v, -123);
  std::cout << "std::ranges::fill: ";
  print_ranges_for_each(v);
}


[[maybe_unused]] void algorithm_generate()
{
  std::vector<int> v(4);
  std::ranges::generate(v, std::rand);
  std::cout << "std::ranges::generate: ";
  print_ranges_for_each(v);
}


[[maybe_unused]] void algorithm_iota()
{
  auto v = std::vector<int>(6);
  std::iota(v.begin(), v.end(), 0);
  std::cout << "std::iota: ";
  print_ranges_for_each(v);
  // Output: 0 1 2 3 4 5
}


[[maybe_unused]] void algorithm_find()
{
  {
    const auto numbers = std::list{2, 4, 3, 2, 3, 1};
    auto it = std::ranges::find(numbers, 2);
    if (it != numbers.cend())
      std::cout << "std::ranges::find: " << *it << std::endl;
  }
  {
    struct person {
      unsigned uid;
      std::string name, position;
    };
    const std::vector<person> persons {
      {0, "Ana", "barber"},
      {1, "Bob", "cook"},
      {2, "Eve", "builder"}
    };
    if (const auto it = std::ranges::find(persons, "Bob", &person::name);
        it != persons.cend())
      std::cout << "std::ranges::find: " << it->uid << " " << it->name << " " << it->position << std::endl;
  }
}

[[maybe_unused]] void algorithm_binary_search()
{
  auto v = std::vector<int>{2, 2, 3, 3, 3, 4, 5};
  // A binary search works if the container is sorted.
  std::cout << "container for binary search is sorted: " << std::boolalpha << std::ranges::is_sorted(v) << std::endl;
  bool found = std::ranges::binary_search(v, 3);
  std::cout << "found: " << std::boolalpha << found << std::endl; // Output: true
}


[[maybe_unused]] void algorithm_lower_upper_bound()
{
  const auto v = std::vector<int>{2, 2, 3, 3, 3, 4, 5};
  if (const auto it = std::ranges::lower_bound(v, 3);
      it != v.cend()) {
    auto index = std::distance(v.begin(), it);
    std::cout << "Lower bound position: " << index << '\n';
  }
  if (auto it = std::ranges::upper_bound(v, 3);
      it != v.cend()) {
    auto index = std::distance(v.begin(), it);
    std::cout << "Upper bound position: " << index << '\n';
  }
}


[[maybe_unused]] void algorithm_all_any_none_of()
{
  const auto numbers = std::vector{3, 2, 2, 1, 0, 2, 1};
  std::cout << "Input data: ";
  print_ranges_for_each(numbers);

  const auto is_negative = [](int i) { return i < 0; };
  
  std::cout << "Contains only natural numbers: " << std::boolalpha << std::ranges::none_of(numbers, is_negative) << std::endl;

  std::cout << "Contains only negative numbers: " << std::boolalpha << std::ranges::all_of(numbers, is_negative) << std::endl;

  std::cout << "Contains at least one negative number: " << std::boolalpha << std::ranges::any_of(numbers, is_negative) << std::endl;
}


[[maybe_unused]] void algorithm_count()
{
  const auto numbers = std::list{3, 3, 2, 1, 3, 1, 3};
  std::cout << "Input data: ";
  print_ranges_for_each(numbers);
  const auto n = std::ranges::count(numbers, 3);
  std::cout << "Number 3 occurs " << n << " times" << std::endl; // Output: 4
}


[[maybe_unused]] void algorithm_min_max_clamp()
{
  const auto twohundred = []() { return 200; };
  const auto ten = 10;
  const auto hundred = 100;
  std::cout << "min: " << std::min(twohundred(), hundred) << std::endl;
  std::cout << "max: " << std::max(twohundred(), ten) << std::endl;
  std::cout << "clamp: " << std::clamp(twohundred(), ten, hundred) << std::endl;
}


[[maybe_unused]] void algorithm_minmax()
{
  const auto values = std::vector{4, 2, 1, 7, 3, 1, 5};
  const auto [min, max] = std::ranges::minmax(values);
  std::cout << "min=" << min << " max=" << max << std::endl; // Output: 1 7
}


[[maybe_unused]] void algorithm_projections()
{
  auto names = std::vector<std::string>{"Ralph", "Lisa", "Homer", "Maggie", "Apu", "Bart"};
  std::ranges::sort(names, std::less<>{}, &std::string::size);
  std::cout << "Input data: ";
  print_ranges_for_each(names);
  // The names are now: "Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"
  // Find names with length 3.
  const auto iterator = std::ranges::find(names, 3, &std::string::size);
  std::cout << "Find first name with length 3: " << *iterator << std::endl;
}


[[maybe_unused]] void algorithm_lambda_projections()
{
  struct Player {
    std::string name{};
    int level{};
    float health{};
  };
  auto players = std::vector<Player>{
    {"Aki", 1, 9.f},
    {"Nao", 2, 7.f},
    {"Rei", 2, 3.f}};
  const auto level_and_health = [](const Player& p) {
    return std::tie(p.level, p.health);
  };
  std::cout << "Order players by level, then by health" << std::endl;
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
  // The std::adjacent_find searches the range for two consecutive equal elements.
  return std::adjacent_find(copy.begin(), copy.end()) != copy.end();
}


[[maybe_unused]] void algorithm_contains_duplicates()
{
  auto vals = std::vector{1,4,2,5,3,6,4,7,5,8,6,9,0};
  std::cout << "Input values: ";
  print_ranges_for_each(vals);
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


struct student {
  int year{};
  int score{};
  std::string name{};
};

[[maybe_unused]] auto get_max_score_copy(const std::vector<student>& students, int year)
{
  const auto by_year = [year](const auto& s) { return s.year == year; };
  // The student list needs to be copied in order to filter on the year.
  auto v = std::vector<student>{};
  std::ranges::copy_if(students, std::back_inserter(v), by_year);
  auto it = std::ranges::max_element(v, std::less{}, &student::score);
  return it != v.end() ? it->score : 0;
}

[[maybe_unused]] auto get_max_score_for_loop(const std::vector<student>& students, int year) {
  auto max_score {0};
  for (const auto& student : students)
    if (student.year == year)
      max_score = std::max(max_score, student.score);
  return max_score;
}

[[maybe_unused]] auto max_value(auto&& range) {
  const auto it = std::ranges::max_element(range);
  return it != range.end() ? *it : 0;
}

[[maybe_unused]] auto get_max_score(const std::vector<student>& students, int year)
{
  const auto by_year = [year](auto&& s) { return s.year == year; };
  return max_value(students | std::views::filter(by_year) | std::views::transform(&student::score));
}

[[maybe_unused]] auto get_max_score_explicit_views(const std::vector<student>& s, int year) {
  auto by_year = [year](const auto& s) { return s.year == year; };
  const auto v1 = std::ranges::ref_view{s}; // Wrap container in a view.
  const auto v2 = std::ranges::filter_view{v1, by_year};
  auto v3 = std::ranges::transform_view{v2, &student::score};
  auto it = std::ranges::max_element(v3);
  return it != v3.end() ? *it : 0;
}

[[maybe_unused]] void algorithm_composability()
{
  const auto students = std::vector<student>{
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


[[maybe_unused]] void algorithm_views_transform()
{
  const auto numbers = std::vector {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::cout << "Input numbers: ";
  print_ranges_for_each(numbers);
  const auto square = [](auto v) { return v * v; };
  // Create a view, but do not yet evaluate this view.
  auto squared_view = std::views::transform(numbers, square);
  // Iterate over the squared view, which invokes evaluation and so invokes the lambda.
  std::cout << "Square: ";
  for (auto s : squared_view) std::cout << s << " ";
  std::cout << std::endl;
}


[[maybe_unused]] void algorithm_views_filter()
{
  const auto v = std::vector{4, 5, 6, 7, 6, 5, 4};
  std::cout << "Input numbers: ";
  print_ranges_for_each(v);
  auto odd_view = std::views::filter(v, [](auto i) { return (i % 2) == 1; });
  std::cout << "Odd numbers: ";
  for (const auto odd_number : odd_view) std::cout << odd_number << " ";
  // Output: 5 7 5
  std::cout << std::endl;
}


[[maybe_unused]] void algorithm_views_join()
{
  const auto list_of_lists = std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}, {5}, {4, 3, 2, 1}};
  const auto flattened_view = std::views::join(list_of_lists);
  std::cout << "Flattened view: ";
  for (auto v : flattened_view) std::cout << v << " ";
  // Output: 1 2 3 4 5 5 4 3 2 1
  std::cout << std::endl;
  
  std::cout << "Max value: ";
  const auto max_value = *std::ranges::max_element(flattened_view);
  // max_value is 5
  std::cout << max_value << std::endl;
}


// Materialize the range r into a std::vector.
// See also https://timur.audio/how-to-make-a-container-from-a-c20-range
[[maybe_unused]] auto to_vector(auto&& r) {
  std::vector<std::ranges::range_value_t<decltype(r)>> v;
  if constexpr (std::ranges::sized_range<decltype(r)>) {
    v.reserve(std::ranges::size(r));
  }
  std::ranges::copy(r, std::back_inserter(v));
  return v;
}


[[maybe_unused]] void algorithm_views_materialize()
{
  auto ints = std::list{2, 3, 4, 2, 1};
  std::cout << "Input numbers: ";
  print_ranges_for_each (ints);
  const auto r = ints | std::views::transform([](const auto i) { return std::to_string(i); });
  auto strings = to_vector(r);
  std::cout << "Output strings: ";
  print_ranges_for_each (strings);
}


[[maybe_unused]] void algorithm_views_take()
{
  auto vec = std::vector{4, 2, 7, 1, 2, 6, 1, 5};
  std::cout << "Input numbers: ";
  print_ranges_for_each (vec);
  const auto first_half = vec | std::views::take(vec.size() / 2);
  // This sorts the first half of the original vector via the view.
  std::ranges::sort(first_half);
  std::cout << "First half sorted numbers: ";
  print_ranges_for_each (vec);
}


[[maybe_unused]] void algorithm_views_split_join()
{
  auto csv = std::string{"10,11,12"};
  std::cout << "CSV: " << csv << std::endl;
  auto digits = csv | std::ranges::views::split(',');
  // [ [1, 0], [1, 1], [1, 2] ]
  std::cout << "Split: ";
  for (const auto digit : digits) {
    std::cout << " [ ";
    for (const auto element : digit)
      std::cout << element;
    std::cout << " ] ";
  }
  std::cout << std::endl;
  auto joined = digits | std::views::join;
  // [ 1, 0, 1, 1, 1, 2 ]
  std::cout << "Joined: ";
  print_ranges_for_each (joined);
}


[[maybe_unused]] void algorithm_views_sampling()
{
  auto vec = std::vector{1, 2, 3, 4, 5, 4, 3, 2, 1};
  std::cout << "Input numbers: ";
  print_ranges_for_each (vec);
  // Apply the range adaptor that represents view of elements from an underlying sequence,
  // beginning at the first element for which the predicate returns false.
  auto v = vec | std::views::drop_while([](auto i) { return i < 5; });
  std::cout << "After drop while: ";
  print_ranges_for_each (v);
  // Prints 5 4 3 2 1
}


[[maybe_unused]] void algorithm_views_stream()
{
  const auto s{"1.4142 1.618 2.71828 3.14159 6.283"};
  std::cout << "Input: " << s << std::endl;
  auto iss = std::istringstream{s};
  for (auto f : std::ranges::istream_view<float>(iss)) {
    std::cout << f << std::endl;
  }
}


[[maybe_unused]] void memory_placement_new()
{
  struct User {
    User(const std::string& name) : m_name(name) { }
    std::string m_name;
  };
  // Established way.
  {
    // Allocate sufficient memory for the User object.
    auto* memory = std::malloc(sizeof(User));
    // Construct new object in existing memory.
    auto* user = new (memory) User("John");
    std::cout << "Pre-allocated " << user->m_name << std::endl;
    // Call destructor: This does not yet free the memory.
    user->~User();
    // Free memory on heap.
    std::free(memory);
  }
  // Modern way in C++20.
  {
    auto* memory = std::malloc(sizeof(User));
    auto* user_ptr = reinterpret_cast<User*>(memory);
    std::uninitialized_fill_n(user_ptr, 1, User{"John"});
    std::construct_at (user_ptr, User{"John"});
    std::cout << "Constructed at " << user_ptr->m_name << std::endl;
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

[[maybe_unused]] void template_or_auto_or_remove_cvref_methods()
{
  {
    auto result = pow_n_traditional<float>(2.0f, 3);
    std::cout << "Traditional float: " << result << std::endl;
  }
  {
    auto result = pow_n_traditional<int>(3, 3);
    std::cout << "Traditional int: " << result << std::endl;
  }
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
auto generic_modulus_combined(const T v, const T n) -> T {
  assert(n != 0);
  if constexpr (std::is_floating_point_v<T>) {
    return std::fmod(v, n);
  } else {
    // If T is a floating point this code is skipped, it won't be compiled, so no compile errors.
    return v % n;
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
    auto const result = generic_modulus_combined(value, modulus);
    std::cout << result << std::endl;
  }
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    auto const result = generic_modulus_combined(value, modulus);
    std::cout << result << std::endl;
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
T generic_mod_overload(T v, T n) { // Integral version.
  return v % n;
}

template <std::floating_point T>
T generic_mod_overload(T v, T n) { // Floating point version.
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
    auto const x = generic_mod_overload(value, modulus);
    std::cout << "should be 2: " << x << std::endl;
  }
  // Using a floating point type.
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    auto const x = generic_mod_overload(value, modulus);
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

[[maybe_unused]] static auto make_saturn() { return std::tuple{"Saturn", 82, true}; }

// Automatic type deduction by defining the struct within the function itself.
[[maybe_unused]] static auto make_earth() {
  struct Planet { std::string name; int n_moons; bool rings; };
  return Planet{"Earth", 1, false};
}

[[maybe_unused]] static void demo_tuple()
{
  {
    // Using std::get<N>
    auto t = make_saturn();
    auto name = std::get<0>(t);
    auto n_moons = std::get<1>(t);
    auto rings = std::get<2>(t);
    std::cout << name << ' ' << n_moons << ' ' << rings << std::endl;
    // Output: Saturn 82 true
  }
  {
    // Using std::tie
    auto name = std::string{};
    auto n_moons = int{};
    auto rings = bool{};
    std::tie(name, n_moons, rings) = make_saturn();
    std::cout << name << ' ' << n_moons << ' ' << rings << std::endl;
  }
  {
    const auto& [name, n_moons, rings] = make_saturn();
    std::cout << name << ' ' << n_moons << ' ' << rings << '\n';
  }
  {
    auto planets = {std::tuple{"Mars", 2, false}, std::tuple{"Neptune", 14, true}};
    for (auto&& [name, n_moons, rings] : planets) {
      std::cout << name << ' ' << n_moons << ' ' << rings << std::endl;
    }
  }
  {
    auto p = make_earth();
    std::cout << p.name << ' ' << p.n_moons << ' ' << p.rings << std::endl;
  }
}

template <size_t Index, typename TupleType, typename Functor>
auto tuple_at(const TupleType& tpl, const Functor& func) -> void {
  const auto& val = std::get<Index>(tpl);
  func(val);
}

template <typename TupleType, typename Functor, size_t Index = 0>
auto tuple_for_each(const TupleType& tpl, const Functor& ifunctor) -> void {
  constexpr auto tuple_size = std::tuple_size_v<TupleType>;
  if constexpr (Index < tuple_size) {
    tuple_at<Index>(tpl, ifunctor);
    tuple_for_each<TupleType, Functor, Index + 1>(tpl, ifunctor);
  }
}

template <typename... Ts> auto make_string(const Ts&... values) {
  auto sstr = std::ostringstream{};
  // Create a tuple of the variadic parameter pack.
  auto tuple = std::tie(values...);
  // Iterate the tuple.
  tuple_for_each(tuple, [&sstr](const auto& v) { sstr << std::boolalpha << v << " "; });
  return sstr.str();
}

[[maybe_unused]] static void demo_variadic_pack()
{
  auto str = make_string(42, "hi", true, false, 42.0f);
  std::cout << str << std::endl;
}

[[maybe_unused]] static void heterogenous_collections_with_variant()
{
  {
    using namespace std::string_literals;
    using VariantType = std::variant<int, std::string, bool>;
    auto container = std::vector<VariantType>{};
    container.push_back(false);
    container.push_back("I am a string"s);
    container.push_back("I am also a string"s);
    container.push_back(13);
    container.pop_back();
    std::reverse(container.begin(), container.end());
  }
  {
    using namespace std::string_literals;
    using VariantType = std::variant<int, std::string, bool>;
    auto v = std::vector<VariantType>{42, "needle"s, true};
    for (const auto& item : v) {
      std::visit([](const auto& x) { std::cout << x << std::endl; }, item);
    }
    auto num_bools = std::count_if(v.begin(), v.end(), [](const auto& item) {
      return std::holds_alternative<bool>(item);
    });
    std::cout << "num_bools: " << num_bools << std::endl;
    auto contains_needle_string =
    std::any_of(v.begin(), v.end(), [](const auto& item) {
      return std::holds_alternative<std::string>(item) &&
      std::get<std::string>(item) == "needle";
    });
    std::cout << "contains_needle_string " << std::boolalpha << contains_needle_string << std::endl;
  }
}


[[maybe_unused]] static void projections_and_comparison_operators()
{
  struct Player {
    std::string name{};
    int level{};
    int score{};
  };
  
  constexpr auto create_players = []() {
    return std::vector<Player>{
      {"Kai", 34, 23092},
      {"Ali", 56, 43423},
      {"Mel", 34, 12981}
    };
  };

  // Manually written comparison function.
  {
    auto players = create_players();
    
    auto cmp = [](const Player& lhs, const Player& rhs) {
      if (lhs.level == rhs.level) {
        return lhs.score < rhs.score;
      } else {
        return lhs.level < rhs.level;
      }
    };
    
    std::sort(players.begin(), players.end(), cmp);
    std::cout << "should be 34: " << players.front().level << std::endl;
    std::cout << "should be 12981: " << players.front().score << std::endl;
  }

  // Comparison function via std::tie.
  {
    auto players = create_players();
    
    auto cmp = [](const Player& lhs, const Player& rhs) {
      auto p1 = std::tie(lhs.level, lhs.score); // Projection
      auto p2 = std::tie(rhs.level, rhs.score); // Projection
      return p1 < p2;
    };
    std::sort(players.begin(), players.end(), cmp);
    std::cout << "should be 34: " << players.front().level << std::endl;
    std::cout << "should be 12981: " << players.front().score << std::endl;
  }

  // Comparison using std::tie and std::ranges.
  {
    auto players = create_players();
    std::ranges::sort(players, std::less{}, [](const Player& p) {
      return std::tie(p.level, p.score);
    });
    std::cout << "should be 34: " << players.front().level << std::endl;
    std::cout << "should be 12981: " << players.front().score << std::endl;
  }
}

[[maybe_unused]] static void lazy_evaluation()
{
  // This is to postpone or not execute parts not needed for obtaining the result.
  // If for example finding which distance is larger,
  // then the formula is square root from the square,
  // but for finding the larger one, the square root is not needed,
  // it is enough to calculate the square of the two values.
}

[[maybe_unused]] static void pipe_operator()
{
  {
    const auto r = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    auto odd_positive_numbers = r | std::views::filter([](auto v) { return v > 0; }) | std::views::filter([](auto v) { return (v % 2) == 1; });
    auto it = std::begin(odd_positive_numbers);
    std::cout << *it << std::endl;
    it++;
    std::cout << *it << std::endl;
    it++;
    std::cout << *it << std::endl;
  }
}

template <typename T> struct ContainsProxy { const T& m_value; };

template <typename Range, typename T>
auto operator|(const Range& r, const ContainsProxy<T>& proxy) {
  const auto& v = proxy.m_value;
  return std::find(r.begin(), r.end(), v) != r.end();
}

template <typename T> auto contains(const T& v) { return ContainsProxy<T>{v}; }

[[maybe_unused]] static void proxy_objects()
{
  {
    auto numbers = std::vector<int>{1, 3, 5, 7, 9};
    auto seven = 7;
    auto proxy = ContainsProxy<decltype(seven)>{seven};
    bool has_seven = numbers | proxy;
    std::cout << std::boolalpha << has_seven << std::endl;
  }
  {
    auto penguins = std::vector<std::string>{"Ping", "Roy", "Silo"};
    bool has_silo = penguins | contains("Silo");
    std::cout << std::boolalpha << has_silo << std::endl;
  }
}

[[maybe_unused]] static void test_async()
{
  auto divide = [](int a, int b) -> int {
    if (!b) throw std::runtime_error("Cannot divide by zero");
    std::cout << "Lambda running on thread id " << std::this_thread::get_id() << std::endl;
    return a / b;
  };
  
  std::cout << "Main running on thread id " << std::this_thread::get_id() << std::endl;
  std::future future = std::async(divide, 45, 5);
  int result = future.get();
  std::cout << 45 / 5 << " " << result << std::endl;
}

struct Stats {
  int heads{};
  int tails{};
};

/* Does not compile yet on macOS Ventura
auto random_int(int min, int max) {
  // One engine instance per thread
  static thread_local auto engine =
  std::default_random_engine{std::random_device{}()};
  
  auto dist = std::uniform_int_distribution<>{min, max};
  return dist(engine);
}

void flip_coin(std::size_t n, Stats& outcomes) {
  auto flip = [&outcomes](auto n) {
    auto heads = std::atomic_ref<int>{outcomes.heads};
    auto tails = std::atomic_ref<int>{outcomes.tails};
    for (auto i = 0u; i < n; ++i) {
      random_int(0, 1) == 0 ? ++heads : ++tails;
    }
  };
  auto t1 = std::jthread{flip, n / 2};       // First half
  auto t2 = std::jthread{flip, n - (n / 2)}; // The rest
}
 */

[[maybe_unused]] static void atomic_references()
{
  auto stats = Stats{};
  //flip_coin(5000, stats); // Flip 5000 times
  std::cout << "heads: " << stats.heads << ", tails: " << stats.tails << std::endl;
}

// No compiler support yet.
/*
namespace SpinLock {

class SimpleMutex {
  std::atomic_flag is_locked{}; // Cleared by default
public:
  auto lock() noexcept {
    while (is_locked.test_and_set()) {
      while (is_locked.test()) // C++20
        ;                      // Spin here
    }
  }
  auto unlock() noexcept { is_locked.clear(); }
};

TEST(Atomics, SimpleSpinLock) {
  
  constexpr auto n = 1'000'000;
  auto counter = 0; // Counter will be protected by mutex
  auto counter_mutex = SimpleMutex{};
  
  auto increment_counter = [&] {
    for (int i = 0; i < n; ++i) {
      counter_mutex.lock();
      ++counter;
      counter_mutex.unlock();
    }
  };
  
  auto t1 = std::thread{increment_counter};
  auto t2 = std::thread{increment_counter};
  
  t1.join(); // Or use std::jthread
  t2.join();
  
  std::cout << counter << '\n';
  // If we don't have a data race, this assert should hold:
  ASSERT_EQ(n * 2, counter);
}

} // namespace SpinLock

namespace WaitAndNotify {

class SimpleMutex {
  std::atomic_flag is_locked{};
  
public:
  auto lock() noexcept {
    while (is_locked.test_and_set())
      is_locked.wait(true); // Don’t spin, wait
  }
  
  auto unlock() noexcept {
    is_locked.clear();
    is_locked.notify_one(); // Notify blocked thread
  }
};

TEST(Atomics, WaitAndNotify) {
  
  constexpr auto n = 1000000;
  auto counter = 0; // Counter will be protected by mutex
  auto counter_mutex = SimpleMutex{};
  
  auto increment_counter = [&] {
    for (int i = 0; i < n; ++i) {
      counter_mutex.lock();
      ++counter;
      counter_mutex.unlock();
    }
  };
  
  auto t1 = std::thread{increment_counter};
  auto t2 = std::thread{increment_counter};
  t1.join();
  t2.join();
  
  std::cout << counter << '\n';
  // If we don't have a data race, this assert should hold:
  ASSERT_EQ(n * 2, counter);
}

} // namespace WaitAndNotify
*/


// This demonstrates how to use std::lock to lock multiple locks at the simultaneously.
// This avoids the risk of having deadlocks in the transfer function.
[[maybe_unused]] static void avoid_deadlock()
{
  struct account {
    int m_balance{0};
    std::mutex m_mutex{};
  };

  auto transfer_money = [](account& from, account& to, int amount) -> void {
    // Define two deferred unique locks.
    auto lock1 = std::unique_lock<std::mutex>{from.m_mutex, std::defer_lock};
    auto lock2 = std::unique_lock<std::mutex>{to.m_mutex, std::defer_lock};
    // Lock both unique_locks at the same time to avoid a deadlock.
    std::lock(lock1, lock2);
    // Do the transfer.
    from.m_balance -= amount;
    to.m_balance += amount;
    // End of scope releases locks.
  };

  auto account1 = account{100};
  auto account2 = account{30};
  transfer_money(account1, account2, 20);
  std::cout << "Account 1: " << account1.m_balance << std::endl;
  std::cout << "Account 2: " << account2.m_balance << std::endl;
}

[[maybe_unused]] static void barriers()
{
  auto random_int = [] (int min, int max) -> int {
    // One engine instance per thread
    thread_local static auto engine = std::default_random_engine{std::random_device{}()};
    auto distribution = std::uniform_int_distribution<>{min, max};
    return distribution(engine);
  };

  constexpr auto n_dice = 5;
  
  auto done = false;
  auto dice = std::array<int, n_dice>{};
  auto threads = std::vector<std::thread>{};
  auto n_turns = 0;
  
  // Completion function.
  auto check_result = [&] {
    ++n_turns;
    auto is_six = [](auto i) { return i == 6; };
    done = std::all_of(dice.begin(), dice.end(), is_six);
  };

  auto barrier = std::barrier{n_dice, check_result};
  for (size_t i = 0; i < n_dice; ++i) {
    threads.emplace_back([&, i] () {
      while (!done) {
        // Roll dice.
        dice[i] = random_int(1, 6);
        // Decrement the count by 1, wait here till the barrier is 0,
        // and then until the phase completion step of the current phase is run.
        barrier.arrive_and_wait();
      }
    });
  }
  for (auto&& t : threads) {
    t.join();
  }
  std::cout << "number of turns: " << n_turns << std::endl;
}

[[maybe_unused]] static void condition_variables()
{
  auto condition_variable = std::condition_variable{};
  auto queue = std::queue<int>{};
  // Protect the shared queue.
  auto mutex = std::mutex{};
  // Special value to signal that we are done.
  constexpr int sentinel = -1;

  // For verifying the test.
  std::vector<int> result;
  
  auto print_ints = [&] () -> void {
    auto i = int{0};
    while (i != sentinel) {
      {
        auto lock = std::unique_lock<std::mutex>{mutex};
        while (queue.empty()) {
          condition_variable.wait(lock); // The lock is released while waiting
        }
        // An alternative approach using a predicate:
        // condition_variable.wait(lock, [] { return !q.empty(); });
        i = queue.front();
        queue.pop();
      }
      if (i != sentinel) {
        std::cout << "Got: " << i << std::endl;
        result.push_back(i);
      }
    }
  };
  
  auto generate_ints = [&] () -> void {
    for (auto i : {1, 2, 3, sentinel}) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      {
        auto lock = std::scoped_lock{mutex};
        queue.push(i);
      }
      condition_variable.notify_one();
    }
  };

  std::thread producer(generate_ints);
  std::thread consumer(print_ints);
  
  producer.join();
  consumer.join();

  for (auto i : result) std::cout << i << " ";
  std::cout << std::endl;
}

[[maybe_unused]] static void data_race_demo()
{
  std::atomic<int> atomic_counter {};
  int normal_counter {};
  // Counter protected by mutex.
  int mutex_counter {};
  std::mutex counter_mutex;

  const auto increment_atomic_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      ++atomic_counter;
    }
    std::cout << "atomic ready" << std::endl;
  };

  const auto increment_normal_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      ++normal_counter;
    }
    std::cout << "normal ready" << std::endl;
  };

  const auto increment_mutex_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      auto lock = std::scoped_lock{counter_mutex};
      ++mutex_counter;
    }
    std::cout << "mutex ready" << std::endl;
  };

  constexpr auto n_times = 1'000'000;
  std::thread thread1(increment_atomic_counter, n_times);
  std::thread thread2(increment_atomic_counter, n_times);
  std::thread thread3(increment_normal_counter, n_times);
  std::thread thread4(increment_normal_counter, n_times);
  std::thread thread5(increment_mutex_counter, n_times);
  std::thread thread6(increment_mutex_counter, n_times);

  thread1.join();
  thread2.join();
  thread3.join();
  thread4.join();
  thread5.join();
  thread6.join();

  // If we don't have a data race, this should hold:
  std::cout << "without a data race the counters should be: " << n_times * 3 << std::endl;
  std::cout << "atomic: " << atomic_counter << std::endl;;
  std::cout << "normal: " << normal_counter << std::endl;;
  std::cout << "mutex : " << mutex_counter << std::endl;;
}

// It should be possible to set an exception in a promise.
[[maybe_unused]] static void future_and_promise_and_exception()
{
  const auto divide = [] (int a, int b, std::promise<int>& promise) {
    try {
      if (!b) {
        throw std::runtime_error("Cannot divide by zero");
      }
      const auto result = a / b;
      promise.set_value(result);
    } catch(...) {
      try {
        // store anything thrown in the promise
        promise.set_exception(std::current_exception());
        // or throw a custom exception instead
        // p.set_exception(std::make_exception_ptr(MyException("mine")));
      } catch(...) {} // set_exception() may throw too
    }
  };

  {
    std::promise<int> promise;
    std::thread thread {divide, 45, 5, std::ref(promise)};
    auto future = promise.get_future();
    auto result = future.get();
    std::cout << "result should be " << 45 / 5 << " and it is " << result << std::endl;
    thread.join();
  }
  try {
    std::promise<int> promise;
    std::thread thread {divide, 45, 0, std::ref(promise)};
    auto future = promise.get_future();
    auto result = future.get();
    std::cout << result << std::endl;
    std::cout << "result should give an exception" << std::endl;
    thread.join();
  } catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
  }
}

[[maybe_unused]] static void joinable_thread()
{
  [[maybe_unused]] auto print = []() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Thread ID: " << std::this_thread::get_id() << '\n';
  };
  std::cout << "main begin" << std::endl;
#ifdef __cpp_lib_jthread
  auto joinable_thread = std::jthread{print};
#else
  std::cout << "no support for jthread" << std::endl;
#endif
  std::cout << "main end" << std::endl;
  // OK: jthread will join automatically
}

[[maybe_unused]] static void latches()
{
  auto prefault_stack = [] () {
    // We don't know the size of the stack.
    constexpr auto stack_size = 500u * 1024u;
    // Make volatile to avoid optimization.
    volatile unsigned char mem[stack_size];
    std::fill(std::begin(mem), std::end(mem), 0);
    std::cout << "Thread stack has been pre-faulted" << std::endl;
  };
  
  constexpr auto n_threads = 3;
  auto latch = std::latch{n_threads};
  auto threads = std::vector<std::thread>{};

  for (auto i = 0; i < n_threads; ++i) {
    threads.emplace_back([&] {
      prefault_stack();
      latch.arrive_and_wait(); // Or just count_down();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      std::cout << "Thread is working" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
  }

  latch.wait();
  std::cout << "Threads have been initialized, starting to work" << std::endl;
  for (auto&& thread : threads) {
    thread.join();
  }
}

template <class T, size_t N> class lock_free_queue {
  std::array<T, N> m_buffer{};   // Used by both threads
  std::atomic<size_t> m_size{0}; // Used by both threads
  size_t m_read_pos{0};          // Used by reader thread
  size_t m_write_pos{0};         // Used by writer thread
  static_assert(std::atomic<size_t>::is_always_lock_free);
  
  bool do_push(T&& t) {
    if (m_size.load() == N) {
      return false;
    }
    m_buffer[m_write_pos] = std::forward<decltype(t)>(t);
    m_write_pos = (m_write_pos + 1) % N;
    m_size.fetch_add(1);
    return true;
  }
  
public:
  // Writer thread.
  bool push(T&& t) { return do_push(std::move(t)); }
  bool push(const T& t) { return do_push(t); }
  
  // Reader thread.
  auto pop() -> std::optional<T> {
    auto val = std::optional<T>{};
    if (m_size.load() > 0) {
      val = std::move(m_buffer[m_read_pos]);
      m_read_pos = (m_read_pos + 1) % N;
      m_size.fetch_sub(1);
    }
    return val;
  }
  auto size() const noexcept { return m_size.load(); }
};

[[maybe_unused]] static void demo_lock_free_queue()
{
  constexpr auto max_size = 5;

  lock_free_queue<std::optional<int>, max_size> queue;
  
  auto writer = std::thread{[&queue]() {
    for (auto i = 0; i < 10; ++i) {
      bool pushed = queue.push({i});
      if (!pushed) {
        i--;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    }
    queue.push(std::nullopt);
  }};

  std::vector<int> result;

  auto reader = std::thread{[&queue, &result]() {
    for (;;) {
      while (queue.size() == 0) {
        // busy wait
      }
      auto element = *queue.pop();
      if (element.has_value()) {
        std::cout << "Got: " << *element << std::endl;
        result.push_back(*element);
      } else {
        break;
      }
    }
  }};
  
  writer.join();
  reader.join();

  std::cout << "Result: ";
  for (auto i : result) std::cout << i << " ";
  std::cout << std::endl;
  // Should be std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}).
}

template <class T, int N> class bounded_buffer {
  std::array<T, N> m_buffer;
  std::size_t m_read_pos{};
  std::size_t m_write_pos{};
  std::mutex m_mutex;
  std::counting_semaphore<N> m_n_empty_slots{N};
  std::counting_semaphore<N> m_n_full_slots{0};
  
  void do_push(auto&& item) {
    // Take one of the empty slots.
    // Atomically decrements the internal counter by 1 if it is greater than ​0.
    // Otherwise blocks until it is greater than ​0​.
    m_n_empty_slots.acquire();
    try {
      auto lock = std::unique_lock{m_mutex};
      m_buffer[m_write_pos] = std::forward<decltype(item)>(item);
      m_write_pos = (m_write_pos + 1) % N;
    } catch (...) {
      // Atomically increments the internal counter by 1.
      // Any thread(s) waiting for the counter to be greater than ​0​,
      // such as ones being blocked in acquire, will subsequently be unblocked.
      m_n_empty_slots.release();
      throw;
    }
    // Increment the counter and signal that there is one more full slot.
    m_n_full_slots.release(); // New
  }
  
public:
  void push(const T& item) { do_push(item); }
  void push(T&& item) { do_push(std::move(item)); }
  
  auto pop() {
    // Take one of the full slots (might block).
    m_n_full_slots.acquire();
    auto item = std::optional<T>{};
    try {
      auto lock = std::unique_lock{m_mutex};
      item = std::move(m_buffer[m_read_pos]);
      m_read_pos = (m_read_pos + 1) % N;
    } catch (...) {
      m_n_full_slots.release();
      throw;
    }
    // Increment and signal that there is one more empty slot.
    m_n_empty_slots.release();
    return std::move(*item);
  }
};

[[maybe_unused]] static void semaphores()
{
  auto buffer = bounded_buffer<std::string, 3>{};
  auto sentinel = std::string{""};
  
  auto producer = std::thread{[&]() {
    buffer.push("1");
    buffer.push("2");
    buffer.push("3");
    buffer.push("4");
    buffer.push("5");
    buffer.push("6");
    buffer.push(sentinel);
  }};
  
  auto consumer = std::thread{[&]() {
    while (true) {
      auto s = buffer.pop();
      if (s == sentinel) return;
      std::cout << "Got: " << s << '\n';
    }
  }};
  
  producer.join();
  consumer.join();
}

// No need to pass a promise ref here.
static int task_divide(int a, int b) {
  if (b == 0) {
    throw std::runtime_error{"Divide by zero exception"};
  }
  return a / b;
}

[[maybe_unused]] static void tasks()
{
  std::packaged_task<decltype(task_divide)> task(task_divide);
  auto future = task.get_future();
  std::thread thread(std::move(task), 45, 5);
  try {
    auto result = future.get();
    std::cout << result << std::endl;
  } catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
  }
  thread.join();
}

[[maybe_unused]] static void stop_token()
{
  const auto print = [] (std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      std::cout << std::this_thread::get_id() << '\n';
      std::this_thread::sleep_for(std::chrono::seconds{1});
    }
    std::cout << "Stop requested" << std::endl;
  };

  auto joinable_thread = std::jthread(print);
  std::cout << "Main: goes to sleep" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds{3});
  std::cout << "Main: request jthread to stop" << std::endl;
  joinable_thread.request_stop();
  
}

// The resumable return object.
class Resumable {
  struct Promise {
    auto get_return_object() {
      using Handle = std::coroutine_handle<Promise>;
      return Resumable{Handle::from_promise(*this)};
    }
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
  };
  std::coroutine_handle<Promise> m_coroutine_handle;
  explicit Resumable(std::coroutine_handle<Promise> handle) : m_coroutine_handle{handle} {}
  
public:
  using promise_type = Promise;
  Resumable(Resumable&& r) : m_coroutine_handle{std::exchange(r.m_coroutine_handle, {})} {}
  ~Resumable() { if (m_coroutine_handle) m_coroutine_handle.destroy(); }
  bool resume() {
    if (!m_coroutine_handle.done()) m_coroutine_handle.resume();
    return !m_coroutine_handle.done();
  }
};

template <typename T>
struct Generator {
  
private:
  struct Promise {
    T m_value;
    auto get_return_object() -> Generator {
      using Handle = std::coroutine_handle<Promise>;
      return Generator{Handle::from_promise(*this)};
    }
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    void return_void() {}
    void unhandled_exception() { throw; }
    auto yield_value(T&& value) {
      m_value = std::move(value);
      return std::suspend_always{};
    }
    auto yield_value(const T& value) {
      m_value = value;
      return std::suspend_always{};
    }
  };
  
  struct Sentinel {};
  
  struct Iterator {
    
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    std::coroutine_handle<Promise> m_coroutine_handle;
    Iterator& operator++() {
      m_coroutine_handle.resume();
      return *this;
    }
    void operator++(int) { (void)operator++(); }
    T operator*() const { return  m_coroutine_handle.promise().m_value; }
    pointer operator->() const { return std::addressof(operator*()); }
    bool operator==(Sentinel) const { return m_coroutine_handle.done(); }
  };
  
  std::coroutine_handle<Promise> m_coroutine_handle;
  explicit Generator(std::coroutine_handle<Promise> co_handle) : m_coroutine_handle(co_handle) {}
  
public:
  using promise_type = Promise;
  
  Generator(Generator&& g) : m_coroutine_handle(std::exchange(g.m_coroutine_handle, {})) {}
  ~Generator() { if (m_coroutine_handle) { m_coroutine_handle.destroy();  } }
  
  auto begin() {
    m_coroutine_handle.resume();
    return Iterator{m_coroutine_handle};
  }
  auto end() { return Sentinel{}; }
};

[[maybe_unused]] static void simple_coroutine_example()
{
  auto iota = [] (int start) -> Generator<int> {
    for (int i = start; i < std::numeric_limits<int>::max(); ++i) {
      co_yield i;
    }
  };

  auto take_until = [](Generator<int>& generator, int until) -> Generator<int> {
    for (auto value : generator) {
      if (value > until) {
        co_return;
      }
      co_yield value;
    }
  };
  
  Generator<int> int_generator = iota(2);
  auto values = take_until (int_generator, 5);
  // Pull values lazily.
  for (const auto value : values) {
    std::cout << value << " ";
    // Prints: 2 3 4 5
  }
  std::cout << std::endl;
}

[[maybe_unused]] static void simple_resumable_demo()
{
  auto coroutine = []() -> Resumable {
    // Initial suspend.
    std::cout << "3 ";
    // Explicit suspend.
    co_await std::suspend_always{};
    std::cout << "5 ";
  };

  std::cout << "1 ";
  // Create coroutine state.
  auto resumable = coroutine();
  std::cout << "2 ";
  // Resume.
  resumable.resume();
  std::cout << "4 ";
  // Resume.
  resumable.resume();
  std::cout << "6 " << std::endl;
  // Prints: 1 2 3 4 5 6
  // Destroy coroutine state
}

[[maybe_unused]] static void pass_coroutine_around()
{
  auto coroutine = []() -> Resumable {
    // Initial suspend.
    std::cout << "c1" << std::endl;
    // Explicit suspend.
    co_await std::suspend_always{};
    std::cout << "c2 " << std::endl;
  };

  auto resumable = coroutine();

  // Resume from main.
  resumable.resume();
  
  auto thread = std::thread{[r = std::move(resumable)]() mutable {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(500ms);
    // Resume from thread.
    r.resume();
  }};
  thread.join();
}

template <typename T>
auto lin_value(T start, T stop, std::size_t index, std::size_t n) {
  assert(n > 1 && index < n);
  const auto amount = static_cast<T>(index) / (n - 1);
  const auto v = start + amount * (stop - start);
  return v;
}


template <typename T>
auto lineair_space_coroutine(T start, T stop, std::size_t n) -> Generator<T> {
  for (auto i = 0u; i < n; ++i) {
    co_yield lin_value(start, stop, i, n);
  }
}

template <typename T>
auto lineair_space_ranges(T start, T stop, std::size_t n) {
  return std::views::iota(0ul, n)
  |
  std::views::transform([=](auto i) {
    return lin_value(start, stop, i, n);
  });
}

[[maybe_unused]] static void linear_space_with_coroutines()
{
  for (auto v : lineair_space_coroutine(2.0, 3.0, 5)) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
  for (auto v : lineair_space_ranges(2.0, 3.0, 5)) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
  // Prints: 2, 2.25, 2.5, 2.75, 3,
}

struct ScopedNanoTimer
{
  std::chrono::high_resolution_clock::time_point timepoint0;
  std::function<void(long)> callback;
  
  ScopedNanoTimer(std::function<void(long)> callback)
  : timepoint0(std::chrono::high_resolution_clock::now())
  , callback(callback) { }
  ~ScopedNanoTimer()
  {
    const auto timepoint1 = std::chrono::high_resolution_clock::now();
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(timepoint1-timepoint0).count();
    callback(nanos);
  }
};

struct ScopedNanoResults
{
  void operator()(long ns)
  {
    std::cout << "Elapsed time is " << ns << " nanoseconds" << std::endl;
  }
};

[[maybe_unused]] void scoped_nano_timer ()
{
  ScopedNanoResults scoped_nano_results;
  ScopedNanoTimer scoped_nano_timer(scoped_nano_results);
  
  // Do some work you want to time.
  constexpr int N {100000};
  long double sum {0.0};
  for (int i {0}; i < N; i++)
    sum += i * 45.678;
  std::cout << "The sum is " << sum << std::endl;
}

int main()
{
  test_lambda_capture();
  assign_two_lambdas_to_same_function_object();
  std_function_with_button_class();
  stateless_lambdas();
  lambda_auto_typename();
  test_contains();
  sum_scores_compare_processing_time();
  sorting();
  algorithm_transform();
  algorithm_fill();
  algorithm_generate();
  algorithm_iota();
  algorithm_find();
  algorithm_binary_search();
  algorithm_lower_upper_bound();
  algorithm_all_any_none_of();
  algorithm_count();
  algorithm_min_max_clamp();
  algorithm_minmax();
  algorithm_projections();
  algorithm_lambda_projections();
  algorithm_contains_duplicates();
  algorithm_composability();
  algorithm_views_transform();
  algorithm_views_filter();
  algorithm_views_join();
  algorithm_views_materialize();
  algorithm_views_take();
  algorithm_views_split_join();
  algorithm_views_sampling();
  algorithm_views_stream();
  memory_placement_new();
  template_or_auto_or_remove_cvref_methods();
  return EXIT_SUCCESS;
}

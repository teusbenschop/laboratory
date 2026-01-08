#include <algorithm>
#include <array>
#include <atomic>
#include <barrier>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <cstdint>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <latch>
#include <limits>
#include <list>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <semaphore>
#include <set>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <version>
#include <filesystem>
#include <source_location>
#include <bit>
#include <format>
#include <syncstream>
#include <thread>
#include <mutex>


// Template with a default type.
template<typename D = std::chrono::nanoseconds>
class scoped_timer {
  std::chrono::time_point<std::chrono::system_clock, D> m_started_at;
public:
  scoped_timer() noexcept : m_started_at(std::chrono::time_point_cast<D>(std::chrono::system_clock::now())) {}
  ~scoped_timer() noexcept {
    const auto destruction_time = std::chrono::time_point_cast<D>(std::chrono::system_clock::now());
    std::cout << destruction_time - m_started_at << std::endl;
  }
};


void test_lambda_capture ()
{
  return;
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


void assign_two_lambdas_to_same_function_object ()
{
  return;
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


void std_function_with_button_class()
{
  return;
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


void lambda_auto_typename()
{
  return;
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


void benchmarking_function()
{
  return;
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


void test_contains()
{
  return;
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
  scoped_timer t{};
  auto sum = 0ul;
  for (const auto& obj : objects) {
    sum += static_cast<size_t>(obj.score_);
  }
  return sum;
}

void sum_scores_compare_processing_time ()
{
  return;
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


void sorting ()
{
  return;
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


void print_ranges_for_each (auto&& r)
{
  std::ranges::for_each(r, [](auto&& i) {
    std::cout << i << ' ';
  });
  std::cout << std::endl;
}


void algorithm_transform()
{
  return;
  auto input = std::vector<int>{1, 2, 3, 4};
  auto output = std::vector<int>(input.size());
  auto lambda = [](auto&& i) { return i * i; };
  std::ranges::transform(input, output.begin(), lambda);
  std::cout << "std::ranges::transform: ";
  print_ranges_for_each(output);
}


void algorithm_fill()
{
  return;
  std::vector<int> v(5);
  std::ranges::fill(v, -123);
  std::cout << "std::ranges::fill: ";
  print_ranges_for_each(v);
}


void algorithm_generate()
{
  return;
  std::vector<int> v(4);
  std::ranges::generate(v, std::rand);
  std::cout << "std::ranges::generate: ";
  print_ranges_for_each(v);
}


void algorithm_iota()
{
  return;
  auto v = std::vector<int>(6);
  std::iota(v.begin(), v.end(), 0);
  std::cout << "std::iota: ";
  print_ranges_for_each(v);
  // Output: 0 1 2 3 4 5
}


void algorithm_find()
{
  return;
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


void algorithm_binary_search()
{
  return;
  auto v = std::vector<int>{2, 2, 3, 3, 3, 4, 5};
  // A binary search works if the container is sorted.
  std::cout << "container for binary search is sorted: " << std::boolalpha << std::ranges::is_sorted(v) << std::endl;
  bool found = std::ranges::binary_search(v, 3);
  std::cout << "found: " << std::boolalpha << found << std::endl; // Output: true
}


void algorithm_lower_upper_bound()
{
  return;
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


void algorithm_all_any_none_of()
{
  return;
  const auto numbers = std::vector{3, 2, 2, 1, 0, 2, 1};
  std::cout << "Input data: ";
  print_ranges_for_each(numbers);

  const auto is_negative = [](int i) { return i < 0; };
  
  std::cout << "Contains only natural numbers: " << std::boolalpha << std::ranges::none_of(numbers, is_negative) << std::endl;

  std::cout << "Contains only negative numbers: " << std::boolalpha << std::ranges::all_of(numbers, is_negative) << std::endl;

  std::cout << "Contains at least one negative number: " << std::boolalpha << std::ranges::any_of(numbers, is_negative) << std::endl;
}


void algorithm_count()
{
  return;
  const auto numbers = std::list{3, 3, 2, 1, 3, 1, 3};
  std::cout << "Input data: ";
  print_ranges_for_each(numbers);
  const auto n = std::ranges::count(numbers, 3);
  std::cout << "Number 3 occurs " << n << " times" << std::endl; // Output: 4
}


void algorithm_min_max_clamp()
{
  return;
  const auto twohundred = []() { return 200; };
  const auto ten = 10;
  const auto hundred = 100;
  std::cout << "min: " << std::min(twohundred(), hundred) << std::endl;
  std::cout << "max: " << std::max(twohundred(), ten) << std::endl;
  std::cout << "clamp: " << std::clamp(twohundred(), ten, hundred) << std::endl;
}


void algorithm_minmax()
{
  return;
  const auto values = std::vector{4, 2, 1, 7, 3, 1, 5};
  const auto [min, max] = std::ranges::minmax(values);
  std::cout << "min=" << min << " max=" << max << std::endl; // Output: 1 7
}


void algorithm_projections()
{
  return;
  auto names = std::vector<std::string>{"Ralph", "Lisa", "Homer", "Maggie", "Apu", "Bart"};
  std::ranges::sort(names, std::less{}, &std::string::size);
  std::cout << "Input data: ";
  print_ranges_for_each(names);
  // The names are now: "Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"
  // Find names with length 3.
  const auto iterator = std::ranges::find(names, 3, &std::string::size);
  std::cout << "Find first name with length 3: " << *iterator << std::endl;
}


void algorithm_lambda_projections()
{
  return;
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


void algorithm_contains_duplicates()
{
  return;
  auto vals = std::vector{1,4,2,5,3,6,4,7,5,8,6,9,0};
  std::cout << "Input values: ";
  print_ranges_for_each(vals);
  {
    scoped_timer();
    auto a = contains_duplicates_n2(vals.begin(), vals.end());
    std::cout << "Contains duplicates: " << std::boolalpha << a << std::endl;
  }
  {
    scoped_timer();
    auto b = contains_duplicates_allocating(vals.begin(), vals.end());
    std::cout << "Contains duplicates: " << std::boolalpha << b << std::endl;
  }
}


struct student {
  int year{};
  int score{};
  std::string name{};
};

auto get_max_score_copy(const std::vector<student>& students, int year)
{
  const auto by_year = [year](const auto& s) { return s.year == year; };
  // The student list needs to be copied in order to filter on the year.
  auto v = std::vector<student>{};
  std::ranges::copy_if(students, std::back_inserter(v), by_year);
  auto it = std::ranges::max_element(v, std::less{}, &student::score);
  return it != v.end() ? it->score : 0;
}

auto get_max_score_for_loop(const std::vector<student>& students, int year) {
  auto max_score {0};
  for (const auto& student : students)
    if (student.year == year)
      max_score = std::max(max_score, student.score);
  return max_score;
}

auto max_value(auto&& range) {
  const auto it = std::ranges::max_element(range);
  return it != range.end() ? *it : 0;
}

auto get_max_score(const std::vector<student>& students, int year)
{
  const auto by_year = [year](auto&& s) { return s.year == year; };
  return max_value(students | std::views::filter(by_year) | std::views::transform(&student::score));
}

auto get_max_score_explicit_views(const std::vector<student>& s, int year) {
  auto by_year = [year](const auto& s) { return s.year == year; };
  const auto v1 = std::ranges::ref_view{s}; // Wrap container in a view.
  const auto v2 = std::ranges::filter_view{v1, by_year};
  auto v3 = std::ranges::transform_view{v2, &student::score};
  auto it = std::ranges::max_element(v3);
  return it != v3.end() ? *it : 0;
}

void algorithm_composability()
{
  return;
  const auto students = std::vector<student>{
    {3, 120, "A"},
    {2, 140, "B"},
    {3, 190, "C"},
    {2, 110, "D"},
    {2, 120, "E"},
    {3, 130, "F"},
  };
  {
    scoped_timer ();
    auto score = get_max_score_copy(students, 2); // score is 140
    std::cout << score << std::endl;
  }
  {
    scoped_timer ();
    auto score = get_max_score_for_loop(students, 2); // score is 140
    std::cout << score << std::endl;
  }
  {
    scoped_timer ();
    auto score = get_max_score(students, 2);
    std::cout << score << std::endl;
  }
  {
    scoped_timer ();
    auto score = get_max_score_explicit_views(students, 2);
    std::cout << score << std::endl;
  }
}


void algorithm_views_transform()
{
  return;
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


void algorithm_views_filter()
{
  return;
  const auto v = std::vector{4, 5, 6, 7, 6, 5, 4};
  std::cout << "Input numbers: ";
  print_ranges_for_each(v);
  auto odd_view = std::views::filter(v, [](auto i) { return (i % 2) == 1; });
  std::cout << "Odd numbers: ";
  for (const auto odd_number : odd_view) std::cout << odd_number << " ";
  // Output: 5 7 5
  std::cout << std::endl;
}


void algorithm_views_join()
{
  return;
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
auto to_vector(auto&& r) {
  std::vector<std::ranges::range_value_t<decltype(r)>> v;
  if constexpr (std::ranges::sized_range<decltype(r)>) {
    v.reserve(std::ranges::size(r));
  }
  std::ranges::copy(r, std::back_inserter(v));
  return v;
}


void algorithm_views_materialize()
{
  return;
  auto ints = std::list{2, 3, 4, 2, 1};
  std::cout << "Input numbers: ";
  print_ranges_for_each (ints);
  const auto r = ints | std::views::transform([](const auto i) { return std::to_string(i); });
  auto strings = to_vector(r);
  std::cout << "Output strings: ";
  print_ranges_for_each (strings);
}


void algorithm_views_take()
{
  return;
  auto vec = std::vector{4, 2, 7, 1, 2, 6, 1, 5};
  std::cout << "Input numbers: ";
  print_ranges_for_each (vec);
  const auto first_half = vec | std::views::take(vec.size() / 2);
  // This sorts the first half of the original vector via the view.
  std::ranges::sort(first_half);
  std::cout << "First half sorted numbers: ";
  print_ranges_for_each (vec);
}


void algorithm_views_split_join()
{
  return;
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


void algorithm_views_sampling()
{
  return;
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


void algorithm_views_stream()
{
  return;
  const auto s{"1.4142 1.618 2.71828 3.14159 6.283"};
  std::cout << "Input: " << s << std::endl;
  auto iss = std::istringstream{s};
  for (auto f : std::ranges::istream_view<float>(iss)) {
    std::cout << f << std::endl;
  }
}


void memory_placement_new()
{
  return;
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


auto pow_n_remove_const_volatile_reference (const auto&v, int n) {
  typename std::remove_cvref<decltype(v)>::type product {1};
  for (int i = 0; i < n; i++) {
    product *= v;
  }
  return product;
}

void template_or_auto_or_remove_cvref_methods()
{
  return;
  {
    const auto result = pow_n_traditional<float>(3.0f, 3);
    std::cout << "Traditional float: " << result << std::endl;
  }
  {
    const auto result = pow_n_traditional<int>(3, 3);
    std::cout << "Traditional int: " << result << std::endl;
  }
  {
    const auto result = pow_n_abbreviated(3.0f, 3);
    std::cout << "Abbreviated float: " << result << std::endl;
  }
  {
    const auto input {3.0f};
    const auto result = pow_n_remove_const_volatile_reference(input, 3);
    std::cout << "Remove const/volatile/reference float: " << result << std::endl;
  }
  {
    auto pow_n_function = []<class T>(const T& v, int n) {
      auto product = T{1};
      for (int i = 0; i < n; ++i) {
        product *= v;
      }
      return product;
    };
    auto result = pow_n_function(3, 3); // x is an int
    std::cout << "Lambda function with T& int: " << result << std::endl;
  }
}


void demo_type_traits()
{
  return;
  {
    const auto uint8t_type_equals_unsigned_char_type = std::is_same_v<uint8_t, unsigned char>;
    static_assert(uint8t_type_equals_unsigned_char_type);
  }

  {
    const auto is_float_or_double = std::is_floating_point_v<decltype(3.f)>;
    static_assert(is_float_or_double);
  }

  {
    class Planet {};
    class Mars : public Planet {};
    class Sun {};
    static_assert(std::is_base_of_v<Planet, Mars>);
    static_assert(std::derived_from<Mars, Planet>);
    static_assert(std::is_convertible_v<Mars, Planet>);
    static_assert(not std::is_base_of_v<Planet, Sun>);
    static_assert(not std::is_base_of_v<Mars, Planet>);
  }

  {
    static_assert(std::is_unsigned_v<unsigned int>);
    static_assert(not std::is_unsigned_v<int>);
  }
  
  struct foo
  {
    void m() { std::cout << "non-const non-volatile" << std::endl; }
    void m() const { std::cout << "const" << std::endl; }
    void m() volatile { std::cout << "volatile" << std::endl; }
    void m() const volatile { std::cout << "const volatile" << std::endl; }
  };
  foo{}.m();
  std::add_const<foo>::type{}.m();
  std::add_volatile<foo>::type{}.m();
  std::add_cv<foo>::type{}.m(); // Add const volatile.
  
  int bar {10};
  std::cout << bar << std::endl;

}


struct Bear {
  auto roar() const { std::cout << "roar" << std::endl; }
};
struct Duck {
  auto quack() const { std::cout << "quack" << std::endl; }
};

// The function speak can't be used with both Bear and Duck classes.
template <typename Animal>
auto speak(const Animal& a) {
  if (std::is_same_v<Animal, Bear>) {
    a.roar();
  }
  if (std::is_same_v<Animal, Duck>) {
    a.quack();
  }
}

template <typename Animal>
void constexpr_if_speak(const Animal& a) {
  if constexpr (std::is_same_v<Animal, Bear>) {
    a.roar();
  }
  if constexpr (std::is_same_v<Animal, Duck>) {
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

void demo_if_constexpr()
{
  return;
  {
    Bear bear;
    Duck duck;
    // Won't compile because if constexpr is not used.
    // speak(bear);
    // speak(duck);
    std::cout << "Bear: ";
    constexpr_if_speak(bear);
    std::cout << "Duck: ";
    constexpr_if_speak(duck);
  }
  {
    auto const value = 7;
    auto const modulus = 5;
    auto const result = generic_modulus_combined(value, modulus);
    std::cout << "Integer modulus: " << result << std::endl;
  }
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    auto const result = generic_modulus_combined(value, modulus);
    std::cout << "Floating pointer modulus: " << result << std::endl;
  }
}


template <typename T>
class Point2D {
public:
  Point2D(T x, T y) : m_x{x}, m_y{y} {}
  auto x() { return m_x; }
  auto y() { return m_y; }
private:
  T m_x{};
  T m_y{};
};

void demo_unconstrained_templates()
{
  return;
  const auto distance = [](auto point1, auto point2) {
    auto a = point1.x() - point2.x();
    auto b = point1.y() - point2.y();
    return std::sqrt(a * a + b * b);
  };
  
  {
    auto p1 = Point2D{2, 2};
    auto p2 = Point2D{6, 5};
    auto d = distance(p1, p2);
    std::cout << "Distance: " << d << std::endl;
  }
  // The code below doesn't compile because type int doesn't have x() and y() functions.
  // Needed is better error messages when trying to instantiate this function template.
  //distance(3, 4);
  {
    // We don't want this to compile: Point2D<const char*>
    auto from = Point2D{"2.0", "2.0"};
    auto to = Point2D{"6.0", "5.0"};
    auto d = distance(from, to);
    // Prints nonsense.
    std::cout << "Nonsense: " << d << std::endl;
  }
}

template <typename T>
concept floating_point = std::is_floating_point_v<T>;

template <typename T>
concept number = floating_point<T> || std::is_integral_v<T>;

template <typename T>
concept range = requires(T& t) {
  std::begin(t);
  std::end(t);
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

// Declaration of the concept "hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template<typename T>
concept hashable = requires(T a)
{
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};


// Constrained C++20 function template.
template<hashable T>
void constrained_func1(T) {}

// Alternative ways to apply the same constraint:
template<typename T>
requires hashable<T>
void constrained_func2(T) {}

template<typename T>
void constrained_func3(T) requires hashable<T> {}

void constrained_func4(hashable auto /*parameterName*/) {}


// https://en.cppreference.com/w/cpp/language/constraints
// Class templates, function templates, and non-template functions
// (typically members of class templates)
// may be associated with a constraint,
// which specifies the requirements on template arguments,
// which can be used to select the most appropriate function overloads and template specializations.
// Named sets of such requirements are called concepts.
// Each concept is a predicate, evaluated at compile time,
// and becomes a part of the interface of a template where it is used as a constraint.
 void demo_constraints_and_concepts()
{
   return;
  {
    auto x = mod(5, 2); // OK: Integral types
    std::cout << "Integral type modulus: " << x << std::endl;
    // Compilation error, not integral types
    // auto y = mod(5.f, 2.f);
  }
  {
    auto x = mod_abbreviated(5, 2); // OK: Integral types
    std::cout << "Integral type mod abbreviated: " << x << std::endl;
    // Compilation error, not integral types
    // auto y = mod_abbreviated(5.f, 2.f);
  }
  {
    [[maybe_unused]] auto x = struct_foo<int>{5};
    //std::cout << "should be 1: " << x << std::endl;
    // Compilation error, not integral type.
    // auto y = struct_foo{5.f};
  }
  // Using an integral type.
  {
    auto const value = 7;
    auto const modulus = 5;
    auto const x = generic_mod_overload(value, modulus);
    std::cout << "Modulus of integral types: " << x << std::endl;
  }
  // Using a floating point type.
  {
    auto const value = 1.5f;
    auto const modulus = 1.0f;
    auto const x = generic_mod_overload(value, modulus);
    std::cout << "Modulus of floating point types: " << x << std::endl;
  }
  {
    constrained_func1("abc");    // OK, std::string satisfies Hashable
    constrained_func1(std::string("abc"));
    //struct meow {};
    //constrained_func1(meow{}); // Error: meow does not satisfy Hashable
  }
}


template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept point = requires(T p) {
  requires std::is_same_v<decltype(p.x()), decltype(p.y())>;
  requires arithmetic<decltype(p.x())>;
};

std::floating_point auto get_distance (point auto p1, point auto p2) {
  auto a = p1.x() - p2.x();
  auto b = p1.y() - p2.y();
  return std::sqrt(a * a + b * b);
}

template <arithmetic T> // T is now constrained.
class Point2D_v2 {
public:
  Point2D_v2 (T x, T y) : m_x{x}, m_y{y} {}
  auto x() { return m_x; }
  auto y() { return m_y; }
private:
  T m_x{};
  T m_y{};
};

void demo_point2d_concepts()
{
  return;
  const auto p1 = Point2D_v2{2, 2};
  const auto p2 = Point2D_v2{6, 5};
  auto d = get_distance(p1, p2);
  std::cout << "Concepts demo: distance: " << d << std::endl;
  // Trying to instantiate Point2D with unrelevant types is no longer possible.
  //auto p = Point2D_v2{"2.0", "2.0"};
}


template <typename T>
constexpr auto make_false() {
  return false;
}

template <typename Dst, typename Src>
auto safe_cast(const Src& v) -> Dst {
  constexpr auto is_same_type = std::is_same_v<Src, Dst>;
  constexpr auto is_pointer_to_pointer = std::is_pointer_v<Src> and std::is_pointer_v<Dst>;
  constexpr auto is_float_to_float = std::is_floating_point_v<Src> and std::is_floating_point_v<Dst>;
  constexpr auto is_number_to_number = std::is_arithmetic_v<Src> and std::is_arithmetic_v<Dst>;
  constexpr auto is_intptr_to_ptr = (std::is_same_v<uintptr_t, Src> or std::is_same_v<intptr_t, Src>) and std::is_pointer_v<Dst>;
  constexpr auto is_ptr_to_intptr = std::is_pointer_v<Src> and (std::is_same_v<uintptr_t, Dst> or std::is_same_v<intptr_t, Dst>);
  
  if constexpr (is_same_type) {
    return v;
  }
  else if constexpr (is_intptr_to_ptr or is_ptr_to_intptr) {
    return reinterpret_cast<Dst>(v);
  }
  else if constexpr (is_pointer_to_pointer) {
    assert(dynamic_cast<Dst>(v) != nullptr);
    return static_cast<Dst>(v);
  }
  else if constexpr (is_float_to_float) {
    auto casted = static_cast<Dst>(v);
    auto casted_back = static_cast<Src>(v);
    assert(!isnan(casted_back) and !isinf(casted_back));
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

void demo_safe_cast()
{
  return;
  const auto x = safe_cast<int>(42.6f);
  std::cout << "Safe cast from float to int: " << x << std::endl;
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

void demo_compile_time_string_hash()
{
  return;
  constexpr const auto hash_fn = std::hash<PrehashedString>{};
  constexpr const auto str = PrehashedString("abc");
  constexpr const auto hash_fn_str = hash_fn(str);
  std::cout << "hash_fn(str): " << hash_fn_str << std::endl;
  constexpr const auto hash_function_abc = hash_function("abc");
  std::cout << "hash_function_abc: " << hash_function_abc << std::endl;
}


void demo_optional()
{
  return;
  auto input = std::vector<std::optional<int>> {{3}, {}, {1}, {}, {2}, {0}, {-1}};
  std::ranges::sort(input);
  const auto sorted = decltype(input) {{}, {}, {-1}, {0}, {1}, {2}, {3}};
  std::cout << "vectors are equal: " << std::boolalpha << (input == sorted) << std::endl;
}


void demo_automatic_type_deduction()
{
  return;
  // Automatic type deduction by defining the struct within the function itself.
  const auto make_earth = []() {
    struct Planet {
      std::string name{};
      int n_moons{};
      bool rings{};
    };
    return Planet{"Earth", 1, false};
  };

  const auto p = make_earth();
  std::cout << "Planet name: " << std::quoted(p.name) << ", number of moons: " << p.n_moons << ", has rings: " << std::boolalpha << p.rings << std::endl;
}


void demo_tuple()
{
  return;
  const auto make_saturn = []() {
    return std::tuple{"Saturn", 82, true};
  };
  
  {
    // Using std::get<N>
    constexpr const auto planet = make_saturn();
    constexpr const auto name = std::get<0>(planet);
    constexpr const auto n_moons = std::get<1>(planet);
    constexpr const auto rings = std::get<2>(planet);
    std::cout << "Planet name: " << std::quoted(name) << ", number of moons: " << n_moons << ", has rings: " << std::boolalpha << rings << std::endl;
    // Output: Saturn 82 true
  }
  {
    // Using std::tie
    auto name = std::string{};
    auto n_moons = int{};
    auto rings = bool{};
    std::tie(name, n_moons, rings) = make_saturn();
    std::cout << "Planet name: " << std::quoted(name) << ", number of moons: " << n_moons << ", has rings: " << std::boolalpha << rings << std::endl;
  }
  {
    const auto& [name, n_moons, rings] = make_saturn();
    std::cout << "Planet name: " << std::quoted(name) << ", number of moons: " << n_moons << ", has rings: " << std::boolalpha << rings << std::endl;
  }
  {
    const auto planets = {
      std::tuple{"Mars", 2, false},
      std::tuple{"Neptune", 14, true}
    };
    for (auto&& [name, n_moons, rings] : planets) {
      std::cout << "Planet name: " << std::quoted(name) << ", number of moons: " << n_moons << ", has rings: " << std::boolalpha << rings << std::endl;
    }
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

void demo_variadic_pack()
{
  return;
  const auto make_string = [](const auto&... values) {
    auto sstr = std::ostringstream{};
    // Create a tuple of the variadic parameter pack.
    const auto tuple = std::tie(values...);
    // Iterate the tuple.
    tuple_for_each(tuple, [&sstr](const auto& v) {
      sstr << std::boolalpha << v << " ";
    });
    return sstr.str();
  };
  
  const auto str = make_string(123, "hi", true, false, 12.3f);
  std::cout << "Variadic pack output: " << str << std::endl;
}



void heterogenous_collections_with_variant()
{
  return;
  using variant_t = std::variant<int, std::string, bool>;
  {
    auto container = std::vector<variant_t> { false, "hello", "world", 13 };
    std::ranges::reverse(container);
  }
  {
    constexpr const auto needle {"needle"};
    const auto v = std::vector<variant_t>{42, needle, true};
    for (const auto& item : v) {
      std::visit([](const auto& x) { std::cout << "Variant has: " << x << std::endl; }, item);
    }
    const auto num_bools = std::ranges::count_if(v, [](const auto& item) {
      return std::holds_alternative<bool>(item);
    });
    std::cout << "Number of booleans: " << num_bools << std::endl;
    auto contains_needle_string = std::ranges::any_of(v, [](const auto& item) {
      return std::holds_alternative<std::string>(item) and std::get<std::string>(item) == needle;
    });
    std::cout << "Contains " << std::quoted(needle) << ": " << std::boolalpha << contains_needle_string << std::endl;
  }
}


void projections_and_comparison_operators()
{
  return;
  struct Player {
    std::string name{};
    int level{};
    int score{};
  };

  constexpr const auto i34    {34};
  constexpr const auto i12981 {12981};

  constexpr const auto create_players = []() {
    return std::vector<Player>{
      {"Kai", i34,  23092},
      {"Ali",  56,  43423},
      {"Mel", i34, i12981}
    };
  };

  const auto print_first_player = [](const auto& players) {
    std::cout << "First player level should be " << i34 << ": " << players.front().level << std::endl;
    std::cout << "First player score should be " << i12981 << ": " << players.front().score << std::endl;
  };

  // Sort the players through a manually written comparison function.
  {
    auto players = create_players();
    
    auto cmp = [](const Player& lhs, const Player& rhs) {
      if (lhs.level == rhs.level) {
        return lhs.score < rhs.score;
      } else {
        return lhs.level < rhs.level;
      }
    };
    
    std::ranges::sort(players, cmp);
    print_first_player(players);
  }

  // Sort the players through a comparison function via std::tie.
  {
    auto players = create_players();
    
    auto cmp = [](const Player& lhs, const Player& rhs) {
      auto p1 = std::tie(lhs.level, lhs.score); // Projection
      auto p2 = std::tie(rhs.level, rhs.score); // Projection
      return p1 < p2;
    };
    std::ranges::sort(players, cmp);
    print_first_player(players);
  }

  // Sort the players through comparison using std::tie and std::ranges.
  {
    auto players = create_players();
    std::ranges::sort(players, std::less{}, [](const Player& p) {
      return std::tie(p.level, p.score);
    });
    print_first_player(players);
  }
}


void lazy_evaluation()
{
  // Lazy evaluation is to postpone or not execute parts not needed for obtaining the result.
  // If for example finding which distance is larger,
  // then the formula is square root from the square,
  // but for finding the larger one, the square root is not needed,
  // it is enough to calculate the square of the two values.
}


void pipe_operator()
{
  return;
  const auto range = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
  auto odd_positive_numbers = range | std::views::filter([](const auto v) { return v > 0; }) | std::views::filter([](const auto v) { return (v % 2) == 1; });
  auto it = odd_positive_numbers.begin();
  std::cout << "Odd positive number: " << *it << std::endl;
  it++;
  std::cout << "Odd positive number: " << *it << std::endl;
  it++;
  std::cout << "Odd positive number: " << *it << std::endl;
}


template <typename T>
struct ContainsProxy {
  const T& m_value;
};

template <typename Range, typename T>
auto operator | (const Range& r, const ContainsProxy<T>& proxy) {
  const auto& v = proxy.m_value;
  return std::find(r.begin(), r.end(), v) != r.end();
}

template <typename T>
auto contains(const T& v) {
  return ContainsProxy<T>{v};
}

void demo_proxy_objects()
{
  return;
  {
    constexpr const auto seven = 7;
    const auto numbers = {1, 3, 5, seven, 9};
    const auto proxy = ContainsProxy<decltype(seven)>{seven};
    const bool has_seven = numbers | proxy;
    std::cout << "Has " << seven << ": " << std::boolalpha << has_seven << std::endl;
  }
  {
    constexpr const auto silo {"Silo"};
    const auto penguins = {"Ping", "Roy", silo};
    const bool has_silo = penguins | contains(silo);
    std::cout << "Has " << std::quoted(silo) << ": " << std::boolalpha << has_silo << std::endl;
  }
}


void test_async()
{
  return;
  auto divide = [](int a, int b) -> int {
    if (!b) throw std::runtime_error("Cannot divide by zero");
    std::cout << "Lambda running on thread id " << std::this_thread::get_id() << std::endl;
    return a / b;
  };
  
  std::cout << "Main running on thread id " << std::this_thread::get_id() << std::endl;
  std::future future = std::async(divide, 45, 5);
  int result = future.get();
  std::cout << "45 / 5 = " << result << std::endl;
}


void atomic_references()
{
  return;
  struct Stats {
    int heads{};
    int tails{};
  };
  auto stats = Stats{};

  const auto random_int = [](int min, int max) {
    // One engine instance per thread.
    static thread_local auto engine = std::default_random_engine{std::random_device{}()};
    auto distribution = std::uniform_int_distribution<>{min, max};
    return distribution(engine);
  };

  const auto flip_coin = [&](std::size_t n, auto& outcomes) {
    auto flip = [&](auto n) {
      auto heads = std::atomic_ref<int>{outcomes.heads};
      auto tails = std::atomic_ref<int>{outcomes.tails};
      for (auto i = 0u; i < n; ++i) {
        random_int(0, 1) == 0 ? ++heads : ++tails;
      }
    };
    auto t1 = std::jthread{flip, n / 2};       // First half.
    auto t2 = std::jthread{flip, n - (n / 2)}; // The rest.
  };

  flip_coin(5000, stats); // Flip 5000 times
  std::cout << "heads: " << stats.heads << ", tails: " << stats.tails << std::endl;
}


void atomics_simple_spin_lock()
{
  return;
  constexpr auto n {1'000'000};
  auto counter {0};
  std::mutex mutex{};

  const auto run = [&]() {
    const auto increment_counter = [&] {
      for (int i = 0; i < n; ++i) {
        std::lock_guard lock (mutex);
        ++counter;
      }
    };
    std::jthread{increment_counter};
    std::jthread{increment_counter};
  };

  run();
  
  // If we don't have a data race, this assert should hold.
  std::cout << "Counter is " << counter << " which should be double of " << n << std::endl;
}


// This demonstrates how to use std::lock to lock multiple locks at once simultaneously.
// This avoids the risk of having deadlocks in the transfer function.
void demo_lock_multiple_simultaneously()
{
  return;
  struct account {
    int m_balance{0};
    std::mutex m_mutex{};
  };

  const auto transfer_money = [](account& from, account& to, int amount) -> void {
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


// https://en.cppreference.com/w/cpp/thread/barrier
void demo_barriers()
{
  return;
  const auto random_int = [] (const int min, const int max) -> int {
    // One engine instance per thread.
    thread_local static auto engine = std::default_random_engine{std::random_device{}()};
    auto distribution = std::uniform_int_distribution<>{min, max};
    return distribution(engine);
  };

  constexpr auto n_dice = 5;
  
  auto done = false;
  auto dice = std::array<int, n_dice>{};
  auto threads = std::vector<std::thread>{};
  auto n_turns = 0;
  
  // A function to run on completion of a barrier.
  auto on_barrier_completion = [&] {
    ++n_turns;
    auto is_six = [](auto i) { return i == 6; };
    done = std::ranges::all_of(dice, is_six);
  };

  // Define the barrier to use with the completion callback.
  // Barriers are reusable after all arriving threads are unblocked.
  auto barrier = std::barrier{n_dice, on_barrier_completion};
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
  std::cout << "Number of turns to have all dice on 6: " << n_turns << std::endl;
}


void condition_variables()
{
  return;
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
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      {
        auto lock = std::scoped_lock{mutex};
        queue.push(i);
      }
      condition_variable.notify_one();
    }
  };

  {
    std::jthread producer(generate_ints);
    std::jthread consumer(print_ints);
  }
  
  for (auto i : result)
    std::cout << i << " ";
  std::cout << std::endl;
}


namespace stream_insertion {

class Object {
public:
  Object (std::string text, int number) {
    m_text = std::move(text);
    m_number = number;
  };
private:
  std::string m_text{};
  int m_number{};
  friend std::ostream& operator<<(std::ostream& os, const Object&) noexcept;
};

std::ostream& operator<<(std::ostream& os, const Object&) noexcept;
const std::string to_string(const Object&) noexcept;

std::ostream& operator<<(std::ostream& os, const Object& object) noexcept
{
  os << object.m_text << " " << object.m_number;
  return os;
}

const std::string to_string(const Object& object) noexcept
{
  std::ostringstream ss;
  ss << object;
  return std::move(ss).str();
}

void overloading_stream_insertion_operators()
{
  return;
  Object object { "hello world", 1 };
  std::cout << "Stream Object " << object << std::endl;
  std::cout << "Stream Object " << to_string(object) << std::endl;
}

}


void demo_barrier_jthread_stop_token()
{
  return;
  constexpr const auto marker {"Demo barrier / jthread / stop_token: "};
  std::cout << marker << "Start" << std::endl;
  
  // This lambda starts the failing processes.
  // On any failure, it restarts the processes.
  const auto resilient_consumers = [](const std::stop_token stop_token) {
    
    // Protect the barrier from being arrived at multiple times
    // due to multiple processes that may all fail simultaneously.
    std::atomic<bool> barrier_active{false};
    // As soon as the barrier is complete, clear the associated protecting flag.
    auto on_barrier_completion = [&]() noexcept {
      barrier_active = false;
    };
    // The barrier has count 2:
    // One to allow arriving and waiting at the barrier after all processes have been created.
    // The second one to use for the error callback.
    std::barrier barrier(2, on_barrier_completion);
    
    // Allow the main program to interrupt the processes loop.
    std::stop_callback stop_callback(stop_token, [&]() {
      if (barrier_active) {
        barrier_active = false;
        [[maybe_unused]] const auto token = barrier.arrive();
      }
    });

    bool slow_restart_down {false};

    while (!stop_token.stop_requested()) {
      try {

        const auto on_event = [&](const std::string & error) {
          std::cout << "Error event: " << error << std::endl;
          // Arrive at the barrier.
          // This will open the barrier, so all processes get recreated.
          if (barrier_active) {
            barrier_active = false;
            [[maybe_unused]] const auto token = barrier.arrive();
            slow_restart_down = true;
          }
        };
        
        // Wait shortly to avoid fast repeating error events.
        if (slow_restart_down) {
          std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }

        std::cout << "Start process..." << std::endl;
        barrier_active = true;

        const auto generate_error = [&]() {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          on_event("Error");
        };
        std::thread (generate_error).detach(); // Better not detach in production code.
                
        std::cout << "All processes started" << std::endl;
        
        // Wait here till an error occurs or the program shuts down.
        barrier.arrive_and_wait();
      }
      catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
    std::cout << "Stopping processes" << std::endl;
  };

  std::jthread consumers_thread(resilient_consumers);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  std::cout << marker << "Stop" << std::endl;
}


void demo_timer_with_timed_mutex_and_condition_variable()
{
  return;
  std::timed_mutex mx;
  std::condition_variable_any cv;

  const auto timer = [&](const std::stop_token stoken) {
    constexpr const auto interval = std::chrono::milliseconds(1);
    while (!stoken.stop_requested()) {
      std::unique_lock ulk(mx);
      if (cv.wait_for(ulk, stoken, interval, [&stoken]() { return stoken.stop_requested();}))
        break;
      std::cout << "timer cycle" << std::endl;
    }
  };

  std::cout << "Timer start" << std::endl;
  std::jthread thread(timer);
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  std::cout << "Timer stop" << std::endl;
}


void data_race_demo()
{
  return;
  std::atomic<int> atomic_counter {};
  int normal_counter {};
  int mutex_counter {};
  std::mutex counter_mutex;

  const auto increment_atomic_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      ++atomic_counter;
    }
    std::cout << "Atomic count ready" << std::endl;
  };

  const auto increment_normal_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      ++normal_counter;
    }
    std::cout << "Normal count ready" << std::endl;
  };

  const auto increment_mutex_counter = [&] (int n) {
    for (int i = 0; i < n; i++) {
      auto lock = std::scoped_lock{counter_mutex};
      ++mutex_counter;
    }
    std::cout << "Mutex count ready" << std::endl;
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
  std::cout << "Without a data race the counters should be: " << n_times * 2 << std::endl;
  std::cout << "Atomic: " << atomic_counter << std::endl;;
  std::cout << "Normal: " << normal_counter << std::endl;;
  std::cout << "Mutex : " << mutex_counter << std::endl;;
}


// It should be possible to set an exception in a promise.
void future_and_promise_and_exception()
{
  return; // Fails on macOS Sequoia.
  const auto divide = [] (int a, int b, std::promise<int>& promise) {
    try {
      const auto result = a / b;
      promise.set_value(result);
    } catch(...) {
      try {
        // store anything thrown in the promise
        promise.set_exception(std::current_exception());
        // or throw a custom exception instead
        // promise.set_exception(std::make_exception_ptr(MyException("mine")));
      } catch(...) {} // set_exception() may throw too.
    }
  };

  {
    std::promise<int> promise;
    std::thread thread {divide, 45, 5, std::ref(promise)};
    auto future = promise.get_future();
    auto result = future.get();
    std::cout << "Result should be " << 45 / 5 << " and it is " << result << std::endl;
    thread.join();
  }
  try {
    std::promise<int> promise;
    std::thread thread {divide, 45, 0, std::ref(promise)};
    auto future = promise.get_future();
    auto result = future.get();
    std::cout << result << std::endl;
    std::cout << "Result should give an exception" << std::endl;
    thread.join();
  } catch (const std::exception& exception) {
    std::cout << exception.what() << std::endl;
  }
}


// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.
void demo_sync_stream()
{
  return;
  
  const auto stream_worker = [](int id) {
    using namespace std::literals::chrono_literals;
    for (int i = 0; i < 2; i++) {
      std::this_thread::sleep_for(1ms);
      std::osyncstream synced_out(std::cout);
      synced_out << "worker " << id << std::endl;
    }
  };

  std::jthread threads [4];
  for (int i = 0; i < 4; ++i) {
    threads[i] = std::jthread(stream_worker, i);
  }
}


// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be cancelled/stopped in certain situations.
void demo_jthread_joinable_thread()
{
  return;
  const auto worker = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "Within thread with ID " << std::this_thread::get_id() << std::endl;
  };
  std::cout << "Main before jthread is started" << std::endl;
  {
    auto joinable_thread = std::jthread{worker};
    // The jthread will join automatically when it goes out of scope.
  }
  std::cout << "Main after jthread is out of scope" << std::endl;
  
  std::cout << std::endl << "Start stoppable jthread" << std::endl;
  {
    using namespace std::literals::chrono_literals;
    std::jthread stoppable_thread { [] (std::stop_token stoptoken) {
      while (!stoptoken.stop_requested()) {
        std::cout << "." << std::flush;
        std::this_thread::sleep_for(1ms);
      }
    }};
    std::this_thread::sleep_for(25ms);
    std::cout << std::endl << "Request stop" << std::endl;
    stoppable_thread.request_stop();
    //  stoppable_thread.join(); // Not needed here.
  }
  std::cout << "Stopped" << std::endl;

  {
    using namespace std::literals::chrono_literals;
    
    std::mutex mutex{};
    std::jthread threads [4];
    
    const auto print = [](const std::stop_source &source)
    {
      std::cout << std::boolalpha
      << "stop_source stop_possible = " << source.stop_possible() << std::endl
      << "stop_requested = " << source.stop_requested() << std::endl;
    };
    
    // Common stop source.
    std::stop_source stop_source;
    print(stop_source);
    
    const auto joinable_thread_worker = [&mutex] (const int id, std::stop_source stop_source)
    {
      using namespace std::literals::chrono_literals;
      std::stop_token stoken = stop_source.get_token();
      while (true) {
        std::this_thread::sleep_for(3ms);
        std::lock_guard lock (mutex);
        if (stoken.stop_requested()) {
          std::cout << "worker " << id << " is requested to stop" << std::endl;
          return;
        }
        std::cout << "worker " << id << " goes back to sleep" << std::endl;
      }
    };
    
    // Create worker threads.
    for (int i = 0; i < 4; ++i) {
      threads[i] = std::jthread(joinable_thread_worker, i+1, stop_source);
    }
    
    std::this_thread::sleep_for(7ms);
    
    std::cout << "request stop" << std::endl;
    stop_source.request_stop();
    
    print(stop_source);
    // Note: destructor of jthreads will call join so no need for explicit calls
  }
  
  {
    std::cout << "Advanced jthread use" << std::endl;
    // A worker thread.
    // It will wait until it is requested to stop.
    std::jthread worker_till_stop_request([] (std::stop_token stoptoken) {
      std::cout << "Worker thread id: " << std::this_thread::get_id() << std::endl;
      std::mutex mutex;
      std::unique_lock lock(mutex);
      std::condition_variable_any().wait(lock, stoptoken, [&stoptoken] {
        return stoptoken.stop_requested();
      });
    });
    
    // Register a stop callback on the worker thread.
    std::stop_callback callback(worker_till_stop_request.get_stop_token(), [] {
      std::cout << "Stop callback executed by thread: " << std::this_thread::get_id() << std::endl;
    });
    
    // The stop_callback objects can be destroyed prematurely to prevent execution.
    {
      std::stop_callback scoped_callback(worker_till_stop_request.get_stop_token(), [] {
        // This will not be executed.
        std::cout<< "Scoped stop callback executed by thread: " << std::this_thread::get_id() << std::endl;
      });
    }
    
    // Demonstrate which thread executes the stop_callback and when.
    // Define a stopper function
    auto stopper_func = [&worker_till_stop_request] {
      if(worker_till_stop_request.request_stop())
        std::cout << "Stop request executed by thread: " << std::this_thread::get_id() << std::endl;
      else
        std::cout << "Stop request not executed by thread: " << std::this_thread::get_id() << std::endl;
    };
    
    // Let multiple threads compete for stopping the worker thread.
    {
      std::jthread stopper1(stopper_func);
      std::jthread stopper2(stopper_func);
    }
    
    // After a stop has already been requested, a new stop_callback executes immediately.
    std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
    std::stop_callback callback_after_stop(worker_till_stop_request.get_stop_token(), [] {
      std::cout << "Stop callback executed by thread: " << std::this_thread::get_id() << std::endl;
    });
  }
}


// https://en.cppreference.com/w/cpp/thread/latch
void demo_latches()
{
  return;
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
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
  }

  latch.wait();
  std::cout << "Threads have been initialized, starting to work" << std::endl;
  for (auto&& thread : threads) {
    thread.join();
  }
}
  
  
// https://en.cppreference.com/w/cpp/thread/counting_semaphore
void demo_semaphores()
{
  return;
  // The semaphore counts are set to zero.
  // The semaphore is in a non-signaled state.
  // The counting_semaphore contains an internal counter initialized by the constructor.
  // This counter is decremented by calls to acquire() and related methods,
  // and is incremented by calls to release().
  // When the counter is zero, acquire() blocks until the counter is incremented.
  // The binary_semaphore is a counting_semaphore with max count = 1.
  std::binary_semaphore signal_main_to_thread_semaphore{0};
  std::binary_semaphore signal_thread_to_main_semaphore{0};
  
  const auto thread_processor = [&]() {
    // Wait for a signal from the main process by attempting to decrement the semaphore.
    // This call blocks until the semaphore's count is increased from the main process.
    signal_main_to_thread_semaphore.acquire();
    std::cout << "Thread got the signal" << std::endl;
    
    // Wait shortly to imitate some work being done by the thread.
    using namespace std::literals;
    std::this_thread::sleep_for(10ms);
    
    // Signal the main process back.
    std::cout << "Thread sends the signal" << std::endl;
    signal_thread_to_main_semaphore.release();
  };
  
  // Create a worker thread
  std::jthread worker_thread(thread_processor);
  
  // Signal the worker thread to start working by increasing the semaphore's count.
  std::cout << "Main sends the signal" << std::endl;
  signal_main_to_thread_semaphore.release();
  
  // Wait until the worker thread is done doing the work
  // by attempting to decrement the semaphore's count.
  signal_thread_to_main_semaphore.acquire();
  
  std::cout << "Main got the signal" << std::endl;
}


// No need to pass a promise ref here.
int task_divide(int a, int b) {
  if (!b)
    throw std::runtime_error{"Divide by zero exception"};
  return a / b;
}


void tasks()
{
  return;
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


void demo_stop_token()
{
  return;
  const auto print = [] (std::stop_token stoken) -> void {
    while (!stoken.stop_requested()) {
      std::cout << std::this_thread::get_id() << '\n';
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    std::cout << "Stop requested" << std::endl;
  };

  auto joinable_thread = std::jthread(print);
  std::cout << "Main: goes to sleep" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds{3});
  std::cout << "Main: request jthread to stop" << std::endl;
  joinable_thread.request_stop(); // Same as when the jthread goes out of scope.
}


void scoped_nano_timer()
{
  return;
  scoped_timer<std::chrono::microseconds> timer;
  
  // Do some work you want to time.
  long double sum {};
  for (int i {0}; i < 100'000; i++)
    sum += i * 45.678f;
  std::cout << "The sum is " << sum << std::endl;
}


void demo_std_set_insert_iter_success()
{
  return;
  std::set <std::string> myset {"hello"};
  int inserted {0}, skipped {0};
  for (int i {2}; i; --i) {
    if (auto [iter, success] = myset.insert("Hello"); success) {
      std::cout << "Inserted: " << *iter << std::endl;
      inserted++;
    }
    else
      skipped++;
  }
  std::cout << "Inserted: " << inserted << std::endl;
  std::cout << "Skipped: " << skipped << std::endl;
}


void demo_structured_binding()
{
  return;
  struct fields {
    int a {1}, b {2}, c {3}, d {4};
  };
  {
    const auto [w, x, y, z] = fields{};
    std::cout << w << " " << x << " " << y << " " << z << std::endl;
    // 1 2 3 4
  }
  {
    const auto [b, d, p, q] = [] {
      return fields{4, 3, 2, 1};
    } ();
    std::cout << b << " " << d << " " << p << " " << q << std::endl;
    // 4 3 2 1
  }
  {
    struct cpp17_struct {
      mutable int x1 : 2;
      mutable std::string y1;
    };
    const auto [x, y] = [] {
      return cpp17_struct {1, "2"};
    } ();
    std::cout << x << " " << y << std::endl;
  }
  static_assert(true);
  {
    [[maybe_unused]] auto x1 = { 1, 2 };
    [[maybe_unused]] auto x2 = { 1.0, 2.0 };
    [[maybe_unused]] auto x3 { 2 };
    [[maybe_unused]] auto x4 = { 3 };
  }
  {
    float f {1.1};
    char  c {'a'};
    int   i {1};
    const std::tuple<float&, char&&, int> tpl (f, std::move(c), i);
    [[maybe_unused]] const auto& [ff,cc,ii] = tpl;
    // ff is a structured binding that refers to f; decltype(ff) is float&
    // cc is a structured binding that refers to c; decltype(cc) is char&&
    // ii is a structured binding that refers to the 3rd element of tpl; decltype(ii) is const int
  }
}


void demo_initializer_in_if_and_switch()
{
  return;
  std::set<std::string> myset {};
  if (const auto [iter, success] = myset.insert("hello"); success) {
    if (*iter != "hello") std::cout << "Inserted: " << " " << *iter << std::endl;
  }
  switch (auto i = 1; i) {
    case 0:
      break;
    case 1:
      break;
    default:
      break;
  }
}


void demo_filesystem()
{
  return;
  {
    std::string url = "/var/log/app";
    std::filesystem::path p (url);
    const auto dirname = p.parent_path().string();
    std::cout << dirname << std::endl;
  }
  
  std::cout << std::filesystem::path::preferred_separator << std::endl;
  
  try {
    std::filesystem::path p ("log");
    std::filesystem::remove (p);
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
  
  try {
    std::filesystem::path path ("/tmp/hi.txt");
    bool exists = std::filesystem::exists (path);
    std::cout << "exists: " << exists << std::endl;
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
  
  try {
    std::filesystem::path path (R"(/tmp)");
    for (const auto& directory_entry : std::filesystem::directory_iterator {path}) {
      std::filesystem::path path = directory_entry.path();
      std::cout << path.string() << std::endl;
    }
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}


void demo_quotes()
{
  return;
  // Quotes in ordinary string literal.
  std::cout << "'hello world'" << std::endl;
  // Quotes in raw string literal. Example: R"(text)".
  std::cout << R"("hello world")" << std::endl;
  // Clean way to insert quotes.
  std::cout << std::quoted ("hello world") << std::endl;
}


void demo_byte_to_integer()
{
  return;
  // Example "03 02 01 00 00"
  using octet_stream = std::vector <unsigned char>;
  octet_stream characters { 3, 2, 1, 0, 0 };
  std::vector<std::byte> bytes {
    std::byte{3}, std::byte{2}, std::byte{1}, std::byte{0}, std::byte{0}
  };
  std::cout << static_cast<int>(characters[0]) << std::endl;
  std::cout << std::to_integer<int>(bytes[0]) << std::endl;
  
  std::cout << "Should be 513:" << std::endl;
  std::cout << (static_cast<int>(characters[1]) << 8) + static_cast<int>(characters[2]) << std::endl;
  std::cout << (std::to_integer<int>(bytes[1]) << 8) + std::to_integer<int>(bytes[2]) << std::endl;
  
  int number1 {0};
  {
    std::stringstream ss;
    ss << 256 * static_cast<int>(characters[1]) + static_cast<int>(characters[2]);
    ss >> std::dec >> number1;
  }
  std::cout << number1 << std::endl;
}


// https://en.cppreference.com/w/cpp/feature_test
// Preprocessor macros to detect the presence of C++ features introduced since C++11.
void demo_feature_testing_macros()
{
  return;
  std::cout << "feature testing macros" << std::endl;
#ifdef __has_include
#  if __has_include(<optional>)
  std::cout << "Has include <optional> so that is great" << std::endl;
#  endif
#endif
#ifdef __has_include
#  if __has_include(<bad>)
  std::cout << "Has include <bad> but that looks unlikely" << std::endl;
#  else
  std::cout << "Does not have include <bad> which is expected" << std::endl;
#  endif
#endif
  std::cout << "C++ version " <<  __cplusplus << std::endl;
  
  if(__has_cpp_attribute(assume))
    std::cout << "Has attribute assume" << std::endl;
  else
    std::cout << "Does not have attribute assume" << std::endl;
  if(__has_cpp_attribute(likely))
    std::cout << "Has attribute likely" << std::endl;
  else
    std::cout << "Does not have attribute likely" << std::endl;
  if(__cpp_auto_cast)
    std::cout << "Supports auto cast" << std::endl;
  if(__cpp_consteval)
    std::cout << "Supports consteval" << std::endl;
#ifdef __cpp_contracts
  std::cout << "Supports contracts" << std::endl;
#else
  std::cout << "Does not support contracts" << std::endl;
#endif
}


void three_way_comparison_operator()
{
  return;
  // https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison
  std::cout << "Three-way comparison operator <=> ";
  constexpr double foo {-0.0f};
  constexpr double bar {+0.0f};
  const std::partial_ordering result {foo <=> bar};
  if (result == std::partial_ordering::less)
    std::cout << foo << " is less than " << bar << std::endl;
  if (result == std::partial_ordering::greater)
    std::cout << foo << " is greater than " << bar << std::endl;
  if (result == std::partial_ordering::equivalent)
    std::cout << foo << " and " << bar << " are equal" << std::endl;
  if (result == std::partial_ordering::unordered)
    std::cout << foo << " and " << bar << " are unordered" << std::endl;
}


void init_statements_in_range_based_for_loops()
{
  return;
  // https://en.cppreference.com/w/cpp/language/range-for
  std::cout << "init statements in range-based for loops" << std::endl;
  
  const std::vector v {0, 1, 2, 3, 4, 5};
  
  // Init statement since C++20.
  for ([[maybe_unused]] auto n = v.size(); auto i : v)
    std::cout << i << " ";
  std::cout << std::endl;
  
  // A typedef declaration as init-statement (C++20).
  for (typedef decltype(v)::value_type elem_t; elem_t i : v)
    std::cout << i << " ";
  std::cout << std::endl;
}


void demo_char_8_t()
{
  return;
  std::cout << "type char_8_t" << std::endl;
  const char* character1 {"Hello"};
  // Error in C++20:
  //const char* character1 {u8"Hello"};
  const char8_t* character8 = u8"Hello";
  const char* chars = reinterpret_cast<const char*>(character8);
  std::cout << chars << std::endl;
  std::cout << sizeof(character8) << std::endl;
  std::cout << sizeof(character1) << std::endl;
}


void demo_attribute_no_unique_address()
{
  return;
  std::cout << "demo of attribute no_unique_address" << std::endl;

  // https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
  
  struct Empty {}; // The size of any object of empty class type is at least 1.
  std::cout << "size: " << sizeof(Empty) << std::endl; // 1 byte
  
  struct X
  {
    int i;
    Empty e; // At least one more byte is needed to give ‘e’ a unique address
  };
  std::cout << "size: " << sizeof(X) << std::endl; // 8 bytes
  
  struct Y
  {
    int i;
    [[no_unique_address]] Empty e; // Empty member optimized out.
  };
  std::cout << "size: " << sizeof(Y) << std::endl; // 4 bytes
  
  struct Z
  {
    char c;
    // e1 and e2 cannot share the same address because they have the
    // same type, even though they are marked with [[no_unique_address]].
    // However, either may share address with ‘c’.
    [[no_unique_address]] Empty e1, e2;
  };
  std::cout << "size: " << sizeof(Z) << std::endl; // 2 bytes

  struct W
  {
    char c[2];
    // e1 and e2 cannot have the same address, but one of
    // them can share with c[0] and the other with c[1]:
    [[no_unique_address]] Empty e1, e2;
  };
  std::cout << "size: " << sizeof(Z) << std::endl; // 2 bytes
}


namespace with_attributes {

constexpr double pow(double x, long long n) noexcept
{
  if (n > 0) [[likely]]
    return x * pow(x, n - 1);
  else [[unlikely]]
    return 1;
}

constexpr long long fact(long long n) noexcept
{
  if (n > 1) [[likely]]
    return n * fact(n - 1);
  else [[unlikely]]
    return 1;
}

constexpr double cos(double x) noexcept
{
  constexpr long long precision{16LL};
  double y{};
  for (auto n{0LL}; n < precision; n += 2LL) [[likely]]
    y += pow(x, n) / (n & 2LL ? -fact(n) : fact(n));
  return y;
}

} // namespace with_attributes

namespace no_attributes {

constexpr double pow(double x, long long n) noexcept
{
  if (n > 0)
    return x * pow(x, n - 1);
  else
    return 1;
}

constexpr long long fact(long long n) noexcept
{
  if (n > 1)
    return n * fact(n - 1);
  else
    return 1;
}

constexpr double cos(double x) noexcept
{
  constexpr long long precision{16LL};
  double y{};
  for (auto n{0LL}; n < precision; n += 2LL)
    y += pow(x, n) / (n & 2LL ? -fact(n) : fact(n));
  return y;
}

} // namespace no_attributes


void demo_attribute_likely_unlikely()
{
  return;
  // https://en.cppreference.com/w/cpp/language/attributes/likely
  // Attribute to hint the compiler for the likely or unlikely path of execution,
  // allowing the compiler to optimize the code.

  static const auto gen_random = []() noexcept -> double
  {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dis(-1.0, 1.0);
    return dis(gen);
  };
  
  volatile double sink{}; // ensures a side effect

  const auto benchmark = [&sink](auto fun, auto rem)
  {
    const auto start = std::chrono::high_resolution_clock::now();
    for (auto size{1ULL}; size != 10'000ULL; ++size)
      sink = fun(gen_random());
    const std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << std::fixed << std::setprecision(6) << diff.count()
    << " sec " << rem << std::endl;
  };
  
  benchmark(with_attributes::cos, "(with attributes)");
  benchmark(no_attributes::cos, "(without attributes)");
  benchmark([](double t) { return std::cos(t); }, "(std::cos)");
}


namespace const_evaluation {

// This constexpr function might be evaluated at compile-time
// if the input is known at compile-time.
// Otherwise, it is executed at run-time.
constexpr unsigned factorial(unsigned n)
{
  return (n < 2) ? 1 : (n * factorial(n - 1));
}

// With consteval we enforce that the function will be evaluated at compile-time.
consteval unsigned combination(unsigned m, unsigned n)
{
  return factorial(n) / factorial(m) / factorial(n - m);
}

static_assert(factorial(6) == 720);
static_assert(combination(4, 8) == 70);

}

void demo_consteval()
{
  return;
  // https://en.cppreference.com/w/cpp/language/consteval
  constexpr unsigned x{const_evaluation::factorial(4)};
  std::cout << x << std::endl;
  
  unsigned y = const_evaluation::factorial(4); // OK
  std::cout << y << std::endl;

  const int i {1};
  unsigned z = const_evaluation::combination(i, 7);
  // If 'i' is not const: compile error: 'i' is not a constant expression.
  std::cout << z << std::endl;
}


namespace const_init {
const char* di() { return "dynamic initialization"; }
constexpr const char* ci(bool p) { return p ? "constant initializer" : di(); }
constinit const char* c = ci(true);     // OK
//constinit const char* d = ci(false); // error
}

void demo_constinit()
{
  return;
  // https://en.cppreference.com/w/cpp/language/constinit
  // Using const_init ensures that the variable is initialized at compile-time,
  // and that the static initialization order fiasco cannot take place.
  std::cout << const_init::di() << std::endl;
  std::cout << const_init::ci(true) << std::endl;
}


void designated_initializers()
{
  return;
  // https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers
  {
    struct S {
      const int x{};
      const int y{};
      const int z{};
    };
    // OK although designator order does not match declaration order.
    const S a {.y = 2, .x = 1};
    // Ok, b.y initialized to 0.
    const S b {.x = 1, .z = 2};
    std::cout << a.x << " " << a.y << " " << a.z << std::endl;
    std::cout << b.x << " " << b.y << " " << b.z << std::endl;
  }
  {
    struct S {
      const std::string s {"s"};
      const int n {42};
      const int m {-1};
    };
    const S a {.m = 21};
    // Initializes s with {}, which calls the default constructor.
    // Then initializes n with = 42.
    // Then initializes m with = 21.
    std::cout << a.s << " " << a.n << " " << a.m << std::endl;
  }
}


// https://en.cppreference.com/w/cpp/language/aggregate_initialization
void aggregate_initialization()
{
  return;
  struct S
  {
    int x{};
    struct Foo
    {
      int i{};
      int j{};
      int a[3];
    } foo;
  };
  
  const auto print = [](const S s) {
    std::cout << s.x << " " << s.foo.i << " " << s.foo.j << std::endl;
    for (const int a : s.foo.a)
      std::cout << a << " ";
    std::cout << std::endl;
  };
  
  // Using direct-list-initialization syntax.
  S s1 =
  {
    1,
    {
      2, 3,
      {
        4, 5, 6
      }
    }
  };
  print(s1);
  
  // Same, but with brace elision.
  // The compiler suggest braces around subobject initialization, rightly so.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  S s2 = {1, 2, 3, 4, 5, 6};
#pragma GCC diagnostic pop
  print(s2);
  
  // Brace elision only allowed with equals sign
  [[maybe_unused]] int ar[] = {1, 2, 3}; // ar is int[3]
  
  // Too many initializer clauses
  // char cr[3] = {'a', 'b', 'c', 'd'};
  
  // Array initialized as {'a', '\0', '\0'}
  [[maybe_unused]] char cr[3] = {'a'};
  
  // Fully-braced 2D array: {1, 2}
  [[maybe_unused]] int ar2d1[2][2] = {{1, 2}, {3, 4}};
  // Compiler suggests braces around subobject.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  [[maybe_unused]] int ar2d2[2][2] = {3, 4};
#pragma GCC diagnostic pop
  // Brace elision.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
  [[maybe_unused]] int ar2d3[2][2] = {1, 2, 3, 4}; // brace elision: {1, 2}
#pragma GCC diagnostic pop
  // Only first column: {1, 0}
  [[maybe_unused]] int ar2d4[2][2] = {{1}, {2}};
  
  [[maybe_unused]] std::array<int, 3> std_ar2{{1, 2, 3}};  // std::array is an aggregate
  [[maybe_unused]] std::array<int, 3> std_ar1 = {1, 2, 3}; // brace-elision okay
  
  // int ai[] = {1, 2.0}; // narrowing conversion from double to int:
  // error in C++11, okay in C++03
  
  std::string ars[] = {
    std::string("one"), // copy-initialization
    "two",              // conversion, then copy-initialization
    {'t', 'h', 'r', 'e', 'e'} // list-initialization
  };
  
  union U
  {
    int a;
    const char* b;
  };
  [[maybe_unused]] U u1 = {1};   // OK, first member of the union
  // U u2 = {0, "asdf"}; // error: too many initializers for union
  // U u3 = {"asdf"};    // error: invalid conversion to int
}


// https://en.cppreference.com/w/cpp/language/coroutines
// Coroutines are designed to be performing as lightweight threads.
// Coroutines provide concurrency but not parallelism [Important!].
// Switching between coroutines need not involve any system/blocking calls
// so no need for synchronization primitives such as mutexes, semaphores.

namespace coroutines {

struct return_object {
  struct promise_type {
    return_object get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };
};

struct awaiter {
  std::coroutine_handle<> *hp_;
  constexpr bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> h) { *hp_ = h; }
  constexpr void await_resume() const noexcept {}
};

// This function runs forever. It increases and prints the value.
// The variable i maintains its value even as control switches repeatedly
// between this function and the function that invoked it.
return_object infinite_counter(std::coroutine_handle<>* continuation) {
  awaiter a{continuation};
  for (int i {0}; ; i++) {
    co_await a;
    std::cout << "in coroutine " << i << std::endl;
  }
}

} // Namespace.

void demo_coroutines()
{
  return;
  std::coroutine_handle<> h;
  coroutines::infinite_counter(&h);
  for (int i = 0; i < 3; ++i) {
    std::cout << "in main function" << std::endl;
    h();
  }
  h.destroy();
}


// https://en.cppreference.com/w/cpp/language/modules
// Modules are a language feature to share declarations and definitions across translation units.
// They are an alternative to some use cases of headers.
// Modules are orthogonal to namespaces.
// export module hello_world;
// C++20 'module' only available with '-fmodules-ts', which is not yet enabled with '-std=c++20'


// https://en.cppreference.com/w/cpp/header/bit
void demo_header_bit()
{
  return;
  {
    constexpr auto endian {std::endian::native};
    if (endian == std::endian::big)
      std::cout << "The system is big-endian" << std::endl;
    if (endian == std::endian::little)
      std::cout << "The system is little-endian" << std::endl;
  }
  {
    // Copy bit by bit from one variable to the other variable.
    unsigned int ui8 {3};
    const int i8 = std::bit_cast<int>(ui8);
    std::cout << "Use std::bit_cast: " << i8 << std::endl;
  }
  {
    for (auto u = 0u; u != 10; ++u) {
      std::cout << "u = " << u << " = " << std::bitset<4>(u);
      if (std::has_single_bit(u)) {
        std::cout << " = 2^" << std::log2(u) << " (is power of two)";
      }
      std::cout << std::endl;
    }
  }
  // bit_ceil
  // Finds the smallest integral power of two not less than the given value.
  // bit_floor
  // Finds the largest integral power of two not greater than the given value.
  // bit_width
  // Finds the smallest number of bits needed to represent the given value.
  // rotl - Rotate bits to the left.
  // rotr - Rotate bits to the right.
  // countl_zero
  // Counts the number of consecutive 0 bits, starting from the most significant bit.
  // countl_one
  // Counts the number of consecutive 1 bits, starting from the most significant bit.
  // countr_zero
  // Counts the number of consecutive 0 bits, starting from the least significant bit.
  // countr_one
  // Counts the number of consecutive 1 bits, starting from the least significant bit.
  // popcount - Counts the number of 1 bits in an unsigned integer.
}


// https://en.cppreference.com/w/cpp/header/compare
void demo_header_compare()
{
  return;
}


// https://en.cppreference.com/w/cpp/utility/format
void formatting_library()
{
  return;
  std::cout << std::format("std::format demo: A={} B={} C={}", "a", std::string("b"), 3) << std::endl;
}


// https://en.cppreference.com/w/cpp/chrono#Calendar
void calendar_library_in_chrono()
{
  return;
  constexpr auto year_month_day {std::chrono::year(2021)/8/2};
  std::cout << "year " << year_month_day.year() << " month " << year_month_day.month() << " day " << year_month_day.day() << std::endl;
  const auto utc = std::chrono::system_clock::now();
  std::cout << "UTC time: " << utc << std::endl;
  const std::time_t time = std::chrono::system_clock::to_time_t(utc);
  std::string time_str = std::ctime(&time);
  time_str.pop_back(); // Remove the \n.
  std::cout << "Local time: " << time_str << std::endl;
  //std::cout << std::chrono::current_zone()->to_local(utc) << std::endl;
}


// https://en.cppreference.com/w/cpp/utility/source_location
void demo_source_location()
{
  return;
  const auto log = [] (const std::string_view message,
                       const std::source_location location = std::source_location::current())
  {
    std::clog
    << location.file_name()
    << '(' << location.line() << ':' << location.column() << ") "
    << location.function_name() << ": "
    << message << std::endl;
  };
  log("hello world");
}


// https://en.cppreference.com/w/cpp/container/span
void demo_span()
{
  return;
  std::cout << "Demo std::span" << std::endl;
  const auto slice = [](auto s, std::size_t offset, std::size_t width) {
    return s.subspan(offset, offset + width <= s.size() ? width : 0);
  };
  constexpr int a[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (std::size_t offset{}; ; ++offset) {
    constexpr std::size_t width {5};
    const auto subspan = slice(std::span{a}, offset, width);
    if (subspan.empty())
      break;
    for (const auto& elem : subspan)
      std::cout << elem << ' ';
    std::cout << std::endl;
  }
}


// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with
void demo_starts_with_and_ends_with()
{
  return;
  
  const std::string hello_world {"hello world"};
  
  const auto test_starts_width = [&hello_world](const auto prefix)
  {
    std::cout << std::quoted(hello_world) << " starts with " << prefix << ": ";
    std::cout << std::boolalpha << hello_world.starts_with(prefix) << std::endl;
  };
  
  const auto test_ends_width = [&hello_world] (const auto prefix)
  {
    std::cout << std::quoted(hello_world) << " ends with " << prefix << ": ";
    std::cout << std::boolalpha << hello_world.ends_with(prefix) << std::endl;
  };
  
  using namespace std::literals;
  
  test_starts_width(std::string_view("hello"));
  test_starts_width(std::string_view("goodbye"));
  test_starts_width('h');
  test_starts_width('x');
  test_starts_width("h");
  test_starts_width("x");
  test_ends_width("ld"sv);
  test_ends_width("ld"s);
  test_ends_width('d');
  test_ends_width('e');
  test_ends_width("d");
}


// https://en.cppreference.com/w/cpp/utility/functional/bind_front
void demo_bind_front_bind_back()
{
  return;
  // Function templates std::bind_front and std::bind_back generate
  // a perfect forwarding call wrapper which allows to invoke the callable target
  // with its first or last sizeof...(Args) parameters bound to args.

  const auto minus = [] (const int a, const int b) -> int {
    return a - b;
  };
  
  constexpr const auto fifty_minus_value = std::bind_front(minus, 50);
  std::cout << fifty_minus_value(3) << std::endl; // equivalent to `minus(50, 3)`: 47.
  
  struct S
  {
    int val;
    int minus(int arg) const noexcept { return val - arg; }
  };
  
  constexpr auto member_minus = std::bind_front(&S::minus, S{50});
  std::cout << member_minus(3) << std::endl; // equivalent to `S tmp{50}; tmp.minus(3)`: 47.
  
  // The noexcept specification is preserved.
  static_assert(!noexcept(fifty_minus_value(3)));
  static_assert(noexcept(member_minus(3)));
  
  // Binding of a lambda.
  constexpr const auto plus = [](int a, int b) { return a + b; };
  constexpr const auto forty_plus = std::bind_front(plus, 40);
  std::cout << forty_plus(7) << std::endl; // equivalent to `plus(40, 7)`: 47.
  
  auto multiply_add = [](int a, int b, int c) { return a * b + c; };
  auto multiply_plus_seven = std::__bind_back(multiply_add, 7);
  std::cout << multiply_plus_seven(4, 10) << std::endl; // equivalent to `madd(4, 10, 7)`: 47.
}


void demo_distribution()
{
  return;
  std::cout << "demo distribution" << std::endl;
  
  constexpr const int nrolls = 10000; // number of experiments
  constexpr const int nstars = 100;   // maximum number of stars to distribute
  
  // Define a Mersenne-Twister pseudo random number generator, only once in the application:
  std::mt19937 generator1;
  
  std::default_random_engine generator;
  
  std::poisson_distribution<int> distribution(4.1);
  
  int p[10]={};
  
  for (int i=0; i<nrolls; ++i) {
    int number = distribution(generator);
    if (number<10) ++p[number];
  }
  
  for (int i = 0; i < 10; ++i)
    std::cout << i << ": " << std::string(p[i]*nstars/nrolls,'*') << std::endl;
  {
    // Define the Poisson distribution engine with the desired mean value:
    std::poisson_distribution<int> distrib(1);
    // Generate a 50-piece sample of integers and print on stdout:
    for (int i=0 ; i<50; i++)
      std::cout << distrib(generator1) << " ";
    std::cout << std::endl;
  }
}


struct ExplicitObjectParameter
{
  // OK.
  // Same as void foo(int i) const &;
  void f1(this ExplicitObjectParameter const& self, int i);
  
  // Error: already declared.
  //void f1(int i) const &;
  
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


void demo_explicit_object_parameter_this()
{
  return;
  
  std::cout << "explicit object parameter" << std::endl;

  // A parameter declaration with the specifier "this" declares an explicit object parameter.
  
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
    int g(this Y const& self, int, int) {
      if (&self == &self)
        return 1;
      return 1;
    };
  };
  
  Y y{};
  
  const auto pf = &Y::f;
  std::cout << pf << std::endl;
  
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


void demo_multidimensional_subscript_operator()
{
  int array3d[4][3][2]{};
  array3d[3][2][1] = 42;
  std::cout << "array3d[3][2][1] = " << array3d[3][2][1] << std::endl;
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
  demo_type_traits();
  demo_if_constexpr();
  demo_unconstrained_templates();
  demo_constraints_and_concepts();
  demo_point2d_concepts();
  demo_safe_cast();
  demo_compile_time_string_hash();
  demo_optional();
  demo_automatic_type_deduction();
  demo_tuple();
  demo_variadic_pack();
  heterogenous_collections_with_variant();
  projections_and_comparison_operators();
  lazy_evaluation();
  pipe_operator();
  demo_proxy_objects();
  test_async();
  atomic_references();
  atomics_simple_spin_lock();
  demo_lock_multiple_simultaneously();
  demo_barriers();
  condition_variables();
  stream_insertion::overloading_stream_insertion_operators();
  demo_barrier_jthread_stop_token();
  demo_timer_with_timed_mutex_and_condition_variable();
  data_race_demo();
  future_and_promise_and_exception();
  demo_sync_stream();
  demo_jthread_joinable_thread();
  demo_latches();
  demo_semaphores();
  tasks();
  demo_stop_token();
  scoped_nano_timer();
  demo_std_set_insert_iter_success();
  demo_structured_binding();
  demo_initializer_in_if_and_switch();
  demo_filesystem();
  demo_quotes();
  demo_byte_to_integer();
  demo_feature_testing_macros();
  three_way_comparison_operator();
  init_statements_in_range_based_for_loops();
  demo_char_8_t();
  demo_attribute_no_unique_address();
  demo_attribute_likely_unlikely();
  demo_consteval();
  demo_constinit();
  designated_initializers();
  aggregate_initialization();
  demo_coroutines();
  demo_header_bit();
  demo_header_compare();
  formatting_library();
  calendar_library_in_chrono();
  demo_source_location();
  demo_span();
  demo_starts_with_and_ends_with();
  demo_bind_front_bind_back();
  demo_distribution();
  demo_explicit_object_parameter_this();
  demo_multidimensional_subscript_operator();
  return EXIT_SUCCESS;
}

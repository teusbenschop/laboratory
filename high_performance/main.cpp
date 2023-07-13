#include "main.h"
#include <iostream>
#include <functional>
#include <chrono>
#include <array>
#include <set>

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
    auto ms = duration_cast<milliseconds>(duration).count();
    std::cout << ms << " ms " << function_ << '\n';
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

static void sum_scores_compare_processing_time ()
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

int main()
{
  std::cout << "main" << std::endl;
  sum_scores_compare_processing_time ();
  return 0;
}

#include "main.h"
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <compare>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <random>
#include <array>
#include <string>
#include <coroutine>
#include <stdexcept>
#include <thread>
#include <future>
#include <concepts>
#include <bit>
#include <bitset>
#include <compare>
#include <version>
#include <format>
#include <string_view>
#include <iterator>
#include <ctime>
#include <source_location>
#include <span>
#include <type_traits>
#include <barrier>
#include <latch>
#include <semaphore>
#include <stop_token>
#include <iostream>
#include <sstream>
#include <syncstream>
#include <functional>
#include <cassert>


// Hands-on study of the new features in C++20.
// https://en.cppreference.com/w/cpp/20

int main()
{
  feature_testing_macros();
  three_way_comparison_operator();
  designated_initializers();
  init_statements_in_range_based_for_loops();
  char_8_t();
  attributes__no_unique_address__likely__unlikely();
  const_eval(2);
  const_init();
  aggregate_initialization();
  coroutines();
  modules();
  constraints_concepts();
  header_bit();
  header_compare();
  formatting_library();
  calendar_library_in_chrono();
  source_location();
  span();
  type_traits();
  barrier();
  latch();
  semaphore();
  joinable_thread_basic();
  joinable_thread_advanced();
  stop_call_back();
  sync_stream();
  starts_ends_with();
  bind_front_back();
  distribution();
  return EXIT_SUCCESS;
}


void feature_testing_macros ()
{
  return;
  // https://en.cppreference.com/w/cpp/feature_test
  // Preprocessor macros to detect the presence of C++ features introduced since C++11.
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
}


void three_way_comparison_operator ()
{
  return;
  // https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison
  constexpr double foo {-0.0};
  constexpr double bar {+0.0};
  const std::partial_ordering result = foo <=> bar;
  if (result == std::partial_ordering::less)
    std::cout << foo << " is less than " << bar << std::endl;
  if (result == std::partial_ordering::greater)
    std::cout << foo << " is greater than " << bar << std::endl;
  if (result == std::partial_ordering::equivalent)
    std::cout << foo << " and " << bar << " are equal" << std::endl;
  if (result == std::partial_ordering::unordered)
    std::cout << foo << " and " << bar << " are unordered" << std::endl;
}


void designated_initializers ()
{
  return;
  // https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers
  {
    struct S {
      int x{};
      int y{};
      int z{};
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
      std::string str {"str"};
      int n {42};
      int m {-1};
    };
    const S a {.m = 21};
    // Initializes str with {}, which calls the default constructor.
    // Then initializes n with = 42.
    // Then initializes m with = 21.
    std::cout << a.str << " " << a.n << " " << a.m << std::endl;
  }
}


void init_statements_in_range_based_for_loops()
{
  return;
  // https://en.cppreference.com/w/cpp/language/range-for
  const std::vector<int> v {0, 1, 2, 3, 4, 5};
  
  // Init statement since C++20.
  for ([[maybe_unused]] auto n = v.size(); auto i : v)
    std::cout << i << ' ';
  std::cout << std::endl;
  
  // A typedef declaration as init-statement (C++20).
  for (typedef decltype(v)::value_type elem_t; elem_t i : v)
    std::cout << i << ' ';
  std::cout << std::endl;
}


void char_8_t()
{
  return;
  const char* character1 {"Hello"};
  // Error in C++20:
  //const char* character1 {u8"Hello"};
  const char8_t* character8 = u8"Hello";
  const char* chars = reinterpret_cast<const char*>(character8);
  std::cout << chars << std::endl;
  std::cout << sizeof(character8) << std::endl;
  std::cout << sizeof(character1) << std::endl;
}


// https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
// https://en.cppreference.com/w/cpp/language/attributes/likely

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

static double gen_random() noexcept
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<double> dis(-1.0, 1.0);
  return dis(gen);
}

volatile double sink{}; // ensures a side effect


void attributes__no_unique_address__likely__unlikely()
{
  return;
  for (const auto x : {0.125, 0.25, 0.5, 1. / (1 << 26)})
    std::cout
    << std::setprecision(53)
    << "x = " << x << '\n'
    << std::cos(x) << '\n'
    << with_attributes::cos(x) << '\n'
    << (std::cos(x) == with_attributes::cos(x) ? "equal" : "differ") << std::endl;
  
  auto benchmark = [](auto fun, auto rem)
  {
    const auto start = std::chrono::high_resolution_clock::now();
    for (auto size{1ULL}; size != 10'000'000ULL; ++size)
      sink = fun(gen_random());
    const std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "Time: " << std::fixed << std::setprecision(6) << diff.count()
    << " sec " << rem << std::endl;
  };
  
  benchmark(with_attributes::cos, "(with attributes)");
  benchmark(no_attributes::cos, "(without attributes)");
  benchmark([](double t) { return std::cos(t); }, "(std::cos)");
  // Notes:
  // 1. On macOS the version without attributes is faster.
  // 2. On Debian sid on Intel the version without attributes was faster.
  exit(0); // Todo
}


// https://en.cppreference.com/w/cpp/language/consteval

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

void const_eval (unsigned int argc)
{
  return;
  constexpr unsigned x{factorial(4)};
  std::cout << x << std::endl;
  
  unsigned y = factorial(argc); // OK
  //unsigned z = combination(argc, 7); // error: 'argc' is not a constant expression
  std::cout << y << std::endl;
}


// https://en.cppreference.com/w/cpp/language/constinit
// Using const_init ensures that the variable is initialized at compile-time,
// and that the static initialization order fiasco cannot take place.
static const char* g() { return "dynamic initialization"; }
constexpr const char* f(bool p) { return p ? "constant initializer" : g(); }
constinit const char* c = f(true);     // OK
//constinit const char* d = f(false); // error
void const_init ()
{
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
  // On macOS the compiler suggest braces around subobject initialization, rightly so.
  //S s2 = {1, 2, 3, 4, 5, 6};
  //print(s2);

  // brace elision only allowed with equals sign
  [[maybe_unused]] int ar[] = {1, 2, 3}; // ar is int[3]

  // Too many initializer clauses
  // char cr[3] = {'a', 'b', 'c', 'd'};
  
  // Array initialized as {'a', '\0', '\0'}
  [[maybe_unused]] char cr[3] = {'a'};

  // Fully-braced 2D array: {1, 2}
  [[maybe_unused]] int ar2d1[2][2] = {{1, 2}, {3, 4}};
  // Compiler suggests braces around subobject.
  //int ar2d1[2][2] =                        {3, 4};
  // Brace elision.
  //int ar2d2[2][2] = {1, 2, 3, 4}; // brace elision: {1, 2}
  // Only first column: {1, 0}
  [[maybe_unused]] int ar2d3[2][2] = {{1}, {2}};

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

namespace cor {

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
    std::cout << "counter " << i << std::endl;
  }
}

} // Namespace.

void coroutines()
{
  return;
  std::coroutine_handle<> h;
  cor::infinite_counter(&h);
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


void modules()
{
  return;
}


// https://en.cppreference.com/w/cpp/language/constraints
// Class templates, function templates, and non-template functions (typically members of class templates)
// may be associated with a constraint,
// which specifies the requirements on template arguments,
// which can be used to select the most appropriate function overloads and template specializations.
// Named sets of such requirements are called concepts.
// Each concept is a predicate, evaluated at compile time,
// and becomes a part of the interface of a template where it is used as a constraint.

// Declaration of the concept "hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template<typename T>
concept hashable = requires(T a)
{
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

struct meow {};

// Constrained C++20 function template.
template<hashable T>
void func(T) {}
//
// Alternative ways to apply the same constraint:
// template<typename T>
//     requires Hashable<T>
// void f(T) {}
//
// template<typename T>
// void f(T) requires Hashable<T> {}
//
// void f(Hashable auto /*parameterName*/) {}

void constraints_concepts()
{
  return;
  func("abc");    // OK, std::string satisfies Hashable
  func(std::string("abc"));
  //func(meow{}); // Error: meow does not satisfy Hashable
}


// https://en.cppreference.com/w/cpp/header/bit
void header_bit ()
{
  return;
  {
#ifdef __cpp_lib_endian
    constexpr auto endian {std::endian::native};
    if (endian == std::endian::big) std::cout << "The system is big-endian" << std::endl;
    else if (endian == std::endian::little) std::cout << "The system is little-endian" << std::endl;
    else std::cout << "The system is mixed-endian" << std::endl;
#else
    std::cout << "No support for std::endian"
#endif
  }
  {
    // Copy bit by bit from one variable to the other variable.
//    unsigned int ui8 {3};
//    const int i8 = std::bit_cast<int>(ui8);
//    std::cout << i8 << std::endl;
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
void header_compare()
{
  return;
}


// https://en.cppreference.com/w/cpp/utility/format
void formatting_library()
{
  return;
  std::string message = std::format("A = {} and B = {}", "a", std::string("b"));
  std::cout << message << std::endl;
}


// https://en.cppreference.com/w/cpp/chrono#Calendar
void calendar_library_in_chrono()
{
  return;
  constexpr auto year_month_day {std::chrono::year(2021)/8/2};
  std::cout << "year " << year_month_day.year() << " month " << year_month_day.month() << " day " << year_month_day.day() << std::endl;
  const auto utc = std::chrono::system_clock::now();
  std::cout << utc << std::endl;
  std::time_t time = std::chrono::system_clock::to_time_t(utc);
  std::string time_str = std::ctime(&time);
  time_str.pop_back();
  std::cout << time_str << std::endl;
  //std::cout << std::chrono::current_zone()->to_local(utc) << std::endl;
}


// https://en.cppreference.com/w/cpp/utility/source_location
void source_location()
{
  return;
  const auto log = [] (const std::string_view message,
                       const std::source_location location = std::source_location::current())
  {
    std::clog
    << location.file_name() << '('
    << location.line() << ':'
    << location.column() << ") "
    << location.function_name() << ": "
    << message << std::endl;
  };
  log("hello");
}


// https://en.cppreference.com/w/cpp/container/span

template<class T, std::size_t N>
[[nodiscard]]
constexpr auto slice(std::span<T,N> s, std::size_t offset, std::size_t width)
{
  return s.subspan(offset, offset + width <= s.size() ? width : 0);
}

void span()
{
  return;
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


void type_traits()
{
  return;
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
  std::add_cv<foo>::type{}.m();
  
  int bar {10};
  std::cout << bar << std::endl;
}


// https://en.cppreference.com/w/cpp/thread/barrier
void barrier ()
{
  return;
  const auto persons = { "Person A", "Person B", "Person C", "Person D" };
  
  // A function to run on completion of a barrier.
  auto on_barrier_completion = []() noexcept {
    // Locking not needed here.
    static auto phase {"Ready working"};
    std::cout << phase << std::endl;
    phase = "Ready cleaning up";
  };
  
  // Define the barrier to use with the completion callback.
  std::barrier barrier(std::ssize(persons), on_barrier_completion);
  
  
  // The worker function.
  std::mutex mutex {};
  auto worker = [&](std::string name)
  {
    {
      std::lock_guard<std::mutex> lock (mutex);
      std::cout << " " << name << " worked" << std::endl;
    }
    barrier.arrive_and_wait();
    {
      std::lock_guard<std::mutex> lock (mutex);
      std::cout << " " << name << " cleaned" << std::endl;
    }
    barrier.arrive_and_wait();
  };
  
  std::cout << "Starting" << std::endl;
  std::vector<std::thread> threads{};
  for (auto const& person : persons) {
    threads.emplace_back(worker, person);
  }
  for (auto& thread : threads) {
    thread.join();
  }
}


// https://en.cppreference.com/w/cpp/thread/latch
void latch()
{
  return;
  
  struct job {
    const std::string name;
    std::string product{};
    std::thread action{};
  };
  
  job jobs[] = {{"Person A"}, {"Person B"}, {"Person C"}};
  
  std::latch work_done {std::size(jobs)};
  std::latch start_clean_up {1};
  
  const auto work = [&](job& my_job) {
    my_job.product = my_job.name + " worked";
    work_done.count_down();
    start_clean_up.wait();
    my_job.product = my_job.name + " cleaned";
  };
  
  std::cout << "Starting jobs...";
  for (auto& job : jobs) {
    job.action = std::thread{work, std::ref(job)};
  }
  work_done.wait();
  std::cout << "ready\n";
  for (auto const& job : jobs) {
    std::cout << job.product << '\n';
  }
  
  std::cout << "Workers cleaning up...";
  start_clean_up.count_down();
  for (auto& job : jobs) {
    job.action.join();
  }
  std::cout << "ready\n";
  for (auto const& job : jobs) {
    std::cout << job.product << '\n';
  }
}


// https://en.cppreference.com/w/cpp/thread/counting_semaphore
void semaphore()
{
  return;
  // Object counts are set to zero.
  // Objects are in non-signaled state.
  // The counting_semaphore contains an internal counter initialized by the constructor.
  // This counter is decremented by calls to acquire() and related methods,
  // and is incremented by calls to release().
  // When the counter is zero, acquire() blocks until the counter is incremented.
  // The binary_semaphore is a counting_semaphore with max count = 1.
  std::binary_semaphore signal_main_to_thread_semaphore{0};
  std::binary_semaphore signal_thread_to_main_semaphore{0};

  const auto thread_processor = [&]()
  {
    // Wait for a signal from the main process by attempting to decrement the semaphore.
    signal_main_to_thread_semaphore.acquire();
    
    // This call blocks until the semaphore's count is increased from the main process.
    std::cout << "Thread got the signal" << std::endl;
    
    // Wait shortly to imitate some work being done by the thread.
    using namespace std::literals;
    std::this_thread::sleep_for(1s);
    
    std::cout << "Thread sends the signal" << std::endl;
    
    // Signal the main process back.
    signal_thread_to_main_semaphore.release();
  };
  
  // Create a worker thread
  std::thread worker_thread(thread_processor);

  // Signal the worker thread to start working by increasing the semaphore's count.
  std::cout << "Main sends the signal" << std::endl;
  signal_main_to_thread_semaphore.release();
  
  // Wait until the worker thread is done doing the work
  // by attempting to decrement the semaphore's count.
  signal_thread_to_main_semaphore.acquire();
  
  std::cout << "Main got the signal" << std::endl;
  worker_thread.join();
}


// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be cancelled/stopped in certain situations.

void joinable_thread_basic()
{
  return;
  
  using namespace std::literals::chrono_literals;
  std::cout << "Joining thread" << std::endl;
  
  std::jthread joining_thread { [] (std::stop_token stoptoken) {
    while (!stoptoken.stop_requested()) {
      std::cout << "." << std::flush;
      std::this_thread::sleep_for(100ms);
    }
    std::cout << std::endl;
  }};
  
  std::this_thread::sleep_for(2500ms);
  std::cout << std::endl << "Request stop" << std::endl;
  joining_thread.request_stop();
//  joining_thread.join(); // Not needed here.
}


void joinable_thread_advanced()
{
  return;
  
  using namespace std::literals::chrono_literals;

  std::mutex mutex{};
  std::jthread threads [4];

  const auto print = [](const std::stop_source &source)
  {
    std::cout << std::boolalpha << "stop_source stop_possible = " << source.stop_possible() << " stop_requested = " << source.stop_requested() << std::endl;
  };
  
  // Common stop source.
  std::stop_source stop_source;
  print(stop_source);
  
  const auto joinable_thread_worker = [&mutex] (const int id, std::stop_source stop_source)
  {
    using namespace std::literals::chrono_literals;
    std::stop_token stoken = stop_source.get_token();
    while (true) {
      std::this_thread::sleep_for(300ms);
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
  
  std::this_thread::sleep_for(700ms);
  
  std::cout << "request stop" << std::endl;
  stop_source.request_stop();
  
  print(stop_source);
  
  // Note: destructor of jthreads will call join so no need for explicit calls
}


// Use a helper class for atomic std::cout streaming
class atomic_writer
{
  std::ostringstream buffer;
public:
  ~atomic_writer() {
    std::cout << buffer.str() << std::endl;
  }
  atomic_writer& operator<<(auto input) {
    buffer << input;
    return *this;
  }
};


void stop_call_back ()
{
  /*
  using namespace std::literals::chrono_literals;

  // A worker thread.
  // It will wait until it is requested to stop.
  std::jthread worker([] (std::stop_token stoptoken) {
    atomic_writer() << "Worker thread id: " << std::this_thread::get_id();
    std::mutex mutex;
    std::unique_lock lock(mutex);
    std::condition_variable_any().wait(lock, stoptoken, [&stoptoken] {
      return stoptoken.stop_requested();
    });
  });
  
  // Register a stop callback on the worker thread.
  std::stop_callback callback(worker.get_stop_token(), [] {
    atomic_writer() << "Stop callback executed by thread: "  << std::this_thread::get_id();
  });
  
  // The stop_callback objects can be destroyed prematurely to prevent execution.
  {
    std::stop_callback scoped_callback(worker.get_stop_token(), [] {
      // This will not be executed.
      atomic_writer() << "Scoped stop callback executed by thread: " << std::this_thread::get_id();
    });
  }
  
  // Demonstrate which thread executes the stop_callback and when.
  // Define a stopper function
  auto stopper_func = [&worker] {
    if(worker.request_stop())
      atomic_writer () << "Stop request executed by thread: " << std::this_thread::get_id();
    else
      atomic_writer() << "Stop request not executed by thread: " << std::this_thread::get_id();
  };
  
  // Let multiple threads compete for stopping the worker thread
  std::jthread stopper1(stopper_func);
  std::jthread stopper2(stopper_func);
  stopper1.join();
  stopper2.join();
  
  // After a stop has already been requested, a new stop_callback executes immediately.
  atomic_writer() << "Main thread: " << std::this_thread::get_id();
  std::stop_callback callback_after_stop(worker.get_stop_token(), [] {
    atomic_writer() << "Stop callback executed by thread: " << std::this_thread::get_id();
  });
   */
}


// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.

static void stream_worker (int id)
{
  using namespace std::literals::chrono_literals;
  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(300ms);
    std::osyncstream synced_out(std::cout);
    synced_out << "worker " << id << std::endl;
  }
}

void sync_stream()
{
  return;
  using namespace std::literals::chrono_literals;
  std::jthread threads [4];
  for (int i = 0; i < 4; ++i) {
    threads[i] = std::jthread(stream_worker, i);
  }
}


// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with

void starts_ends_with()
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

// Function templates std::bind_front and std::bind_back generate
// a perfect forwarding call wrapper which allows to invoke the callable target
// with its first or last sizeof...(Args) parameters bound to args.
void bind_front_back()
{
  return;
  
  const auto minus = [] (int a, int b) -> int {
    return a - b;
  };

  constexpr auto fifty_minus_value = std::bind_front(minus, 50);
  std::cout << fifty_minus_value(3) << std::endl; // equivalent to `minus(50, 3)`

  struct S
  {
    int val;
    int minus(int arg) const noexcept { return val - arg; }
  };

  constexpr auto member_minus = std::bind_front(&S::minus, S{50});
  std::cout << member_minus(3) << std::endl; // equivalent to `S tmp{50}; tmp.minus(3)`
  
  // The noexcept specification is preserved.
  static_assert(!noexcept(fifty_minus_value(3)));
  static_assert(noexcept(member_minus(3)));
  
  // Binding of a lambda.
  constexpr auto plus = [](int a, int b) { return a + b; };
  constexpr auto forty_plus = std::bind_front(plus, 40);
  std::cout << forty_plus(7) << '\n'; // equivalent to `plus(40, 7)`
  
  auto madd = [](int a, int b, int c) { return a * b + c; };
  auto mul_plus_seven = std::__bind_back(madd, 7);
  std::cout << mul_plus_seven(4, 10) << '\n'; // equivalent to `madd(4, 10, 7)`
}


void distribution()
{
  return;
  
  const int nrolls = 10000; // number of experiments
  const int nstars = 100;   // maximum number of stars to distribute

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
    std::cout << "\n\n";
  }
}


#include <algorithm>
#include <array>
#include <barrier>
#include <cassert>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <latch>
#include <mutex>
#include <queue>
#include <random>
#include <ranges>
#include <shared_mutex>
#include <thread>
#include <vector>


// Demonstrate binary_search.
namespace algorithm_binary_search {
// A binary search works if the container is sorted.
// It returns true if the value is found in the container.
constexpr auto container = std::array<int, 7>{2, 2, 3, 3, 3, 4, 5};
static_assert(std::ranges::is_sorted(container));
static_assert(std::ranges::binary_search(container, 3));
}


// Demonstrate lower_bound and upper_bound.
namespace algorithms_lower_bound_and_upper_bound {
void demo()
{
    constexpr auto container = std::array<int, 7>{2, 2, 3, 3, 3, 4, 5};
    // The lower_bound returns an iterator to the first element not less than the given value.
    static_assert(std::ranges::lower_bound(container, 3) != container.cend());
    if (auto it = std::ranges::lower_bound(container, 3);
        it != container.cend())
    {
        auto index = std::distance(container.begin(), it);
        assert(index == 2);
    }
    // The upper_bound returns an iterator to the first element greater than a certain value.
    static_assert(std::ranges::upper_bound(container, 3) != container.cend());
    if (auto it = std::ranges::upper_bound(container, 3);
        it != container.cend())
    {
        auto index = std::distance(container.begin(), it);
        assert(index == 5);
    }
}
}


// Demonstrate any_of / all_of / none_of.
namespace algorithms_any_of_all_of_none_of {
// Input of positive numbers.
constexpr auto numbers = std::array<int, 7>{3, 2, 2, 1, 0, 2, 1};
// Function testing negative number.
constexpr auto is_negative = [](int i) { return i < 0; };
// None of the numbers is negative.
static_assert(std::ranges::none_of(numbers, is_negative));
static_assert(!std::ranges::all_of(numbers, is_negative));
// Not any of the numbers is negative.
static_assert(!std::ranges::any_of(numbers, is_negative));
}


// Demonstrate ranges::count.
namespace ranges_count {
// Count how often number 3 occurs: Should be 4 times.
constexpr auto numbers = std::array<int, 7>{3, 3, 2, 1, 3, 1, 3};
static_assert(std::ranges::count(numbers, 3) == 4);
}


// Demonstrate: min / max / clamp / ranges::minmax.
namespace min_max_clamp_ranges_minmax {
void demo()
{
    constexpr auto i200 = []() { return 200; };
    constexpr auto i10 = 10;
    constexpr auto i100 = 100;
    static_assert(std::min(i200(), i100) == i100);
    static_assert(std::max(i200(), i10) == i200());
    static_assert(std::clamp(i200(), i10, i100) == i100);

    const auto values = std::vector{4, 2, 1, 7, 3, 1, 5};
    const auto [min, max] = std::ranges::minmax(values);
    assert(min == 1);
    assert(max == 7);
}
}


namespace why_constraints_are_needed {
// https://en.cppreference.com/w/cpp/language/constraints
// Class templates, function templates, and non-template functions
// (typically members of class templates)
// may be associated with a constraint,
// which specifies the requirements on template arguments,
// which can be used to select the most appropriate function overloads and template specializations.
// Named sets of such requirements are called concepts.
// Each concept is a predicate, evaluated at compile time,
// and becomes a part of the interface of a template where it is used as a constraint.

// An unconstrained template function.
template <typename T>
// The constexpr makes the static_assert possible below.
constexpr auto basic_sum(T one, T two)
{
    return one + two;
}

// Call the template with numbers: OK.
static_assert(basic_sum(1, 2) == 3);

// Call the template with strings:
// Oops, it works, but not as intended, it concatenates instead of summing.
// It would help if the passed types could be constrained.
static_assert(basic_sum(std::string("a"), std::string("b")) == "ab");

// Call the template with chars:
// OK, it fails:
// invalid operands of types 'const char*' and 'const char*' to binary 'operator+'
// static_assert(basic_sum("a", "b") == "?");
}


namespace demonstrate_constraints {
// Three classes, two related, one unrelated, to demonstrate constraints.
struct Base
{
    static constexpr int value{10};
};

struct Derived : Base
{
};

struct Unrelated
{
    static constexpr int value{20};
};

template <typename Type>
// This template has a constraint to make sure the correct type is passed.
// The template function requires that the parameter is derived from the Base struct.
    requires std::derived_from<Type, Base>
constexpr int get_value(Type object)
{
    return object.value;
}

static_assert(get_value(Base()) == 10); // This compiles.
static_assert(get_value(Derived()) == 10); // This compiles.
// static_assert(get_value(Unrelated() == 10);
// The above fails to compile due to the constraint:
// error: no matching function for call to 'get_value(template_with_constraint::Unrelated&)'

// Define a basic concept.
template <typename T>
concept floating_point = std::is_floating_point_v<T>;

// Define a concept consisting of another concept and a type trait.
template <typename T>
concept number = floating_point<T> || std::is_integral_v<T>;

// Define a concept that requires the type to have the given class members.
template <typename T>
concept range = requires(T& t)
{
    std::begin(t);
    std::end(t);
};

// Constraining a type with a concept.
template <typename T>
    requires number<T>
constexpr auto add_both_template(T v, T n)
{
    return v + n;
}

static_assert(add_both_template(10, 11) == 21);

// Using concepts ("number") with abbreviated function templates.
constexpr number auto add_both_abbreviated(number auto v, number auto n)
{
    return v + n;
}

static_assert(add_both_abbreviated(10, 11) == 21);

// Declaration of the concept "hashable", which is satisfied by any type 'T'
// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template <typename T>
concept hashable = requires(T a)
{
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

// Constrained C++20 function template.
template <hashable T>
void constrained_func1(T)
{
}

// Alternative ways to apply the same constraint:
template <typename T>
    requires hashable<T>
void constrained_func2(T)
{
}

template <typename T>
void constrained_func3(T) requires hashable<T>
{
}

void constrained_func4(hashable auto /*parameterName*/)
{
}


// A variadic template function takes a variable number of arguments.
template <typename T, typename... Args>
void variadic_template_print(T t, Args... args)
{
    // The "if constexpr" is evaluated at compile time.
    // Here, if no arguments are passed, it no longer does recursion.
    if constexpr (!sizeof ...(args))
    {
        std::cout << t << std::endl;
    }
    else
    {
        std::cout << t << ", ";
        variadic_template_print(args...);
    }
}

// The above as an abbreviated variadic template function.
void variadic_abbreviated_print(auto t, auto... args)
{
    if constexpr (!sizeof ...(args))
    {
        std::cout << t << std::endl;
    }
    else
    {
        std::cout << t << ", ";
        variadic_abbreviated_print(args...);
    }
}

// Use function overloading.
template <std::integral T>
T generic_mod_overload(T v, T n)
{
    // Integral version.
    return v % n;
}

template <std::floating_point T>
T generic_mod_overload(T v, T n)
{
    // Floating point version.
    return std::fmod(v, n);
}
}


namespace latches_and_barriers {
// Latches and barriers are thread coordination mechanisms
// that allow any number of threads to block until an expected number of threads arrive.
// A latch cannot be reused, while a barrier can be used repeatedly.

// The std::latch is a downward counter which can be used to synchronize threads.
// The value of the counter is initialized on creation.
// Threads may block on the latch until the counter is decremented to zero.
// There is no possibility to increase or reset the counter, the latch is single-use.

// The std::barrier is similar to the std::latch with these differences:
// 1. It can be reused.
// 2. It executes possibly empty callable before unblocking threads.

constexpr auto n_threads = 3;
// The std::latch starts with a given count.
static auto latch = std::latch{n_threads};

void demo_latch()
{
    auto threads = std::vector<std::thread>{};

    for (auto i = 0; i < n_threads; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        threads.emplace_back([&]
        {
            std::cout << "Thread " << std::this_thread::get_id() << " waits at latch and decreases it" << std::endl;
            latch.arrive_and_wait(); // Or just count_down();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "Thread " << std::this_thread::get_id() << " gets past latch" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }

    std::cout << "Main thread waits at latch till threads have counted it down to zero" << std::endl;
    latch.wait();
    std::cout << "Threads have been initialized, starting to work" << std::endl;
    for (auto&& thread : threads)
    {
        thread.join();
    }
    std::cout << "All threads have completed" << std::endl;
}


void demo_barrier()
{
    auto done{false};
    constexpr auto n_dice = 5;
    auto dice = std::array<int, n_dice>{};
    auto threads = std::vector<std::thread>{};
    auto n_turns{0};

    const auto get_random_int = [](const int min, const int max) -> int
    {
        // One engine instance per thread.
        thread_local auto engine = std::default_random_engine{std::random_device{}()};
        auto distribution = std::uniform_int_distribution{min, max};
        return distribution(engine);
    };

    // A function to run on completion of a barrier.
    // It checks whether all dice have rolled to six simultaneously.
    auto on_barrier_completion = [&]
    {
        ++n_turns;
        const auto is_six = [](auto i) { return i == 6; };
        done = std::ranges::all_of(dice, is_six);
    };

    // Define the barrier to use with the completion callback.
    // After the barrier has counted down to zero,
    // and after the completion callback has run,
    // the barrier resets itself to its initial state.
    // Then the barrier can be used again.
    auto barrier = std::barrier{n_dice, on_barrier_completion};
    for (size_t i = 0; i < n_dice; ++i)
    {
        threads.emplace_back([&, i]
        {
            while (!done)
            {
                // Roll dice.
                dice[i] = get_random_int(1, 6);
                // Decrement the count by 1, wait here till the barrier is 0,
                // and then until the phase completion step of the current phase is run.
                barrier.arrive_and_wait();
            }
        });
    }
    for (auto&& t : threads)
    {
        t.join();
    }
    std::cout << "Number of turns to have all dice on 6: " << n_turns << std::endl;
}
}

namespace mutual_exclusion {
// This example code features a queue.
// Some code writes to it, and some code reads from it.
// To synchronize the read and write operations, the following locks are used:
// 1. A shared lock to enable one or more processes to read from the queue simultaneously.
// 2. A unique lock so only one bit of code can write to the queue.

static std::queue<int> the_queue;
static std::shared_mutex the_mutex;
static std::condition_variable_any the_cv;

void print_queue(const std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        // Wait for something to be broadcast or for a stop request.
        // This uses a unique lock because it might modify the queue.
        std::unique_lock lock(the_mutex);
        the_cv.wait_for(lock, stoken, std::chrono::seconds(1), [&stoken]
        {
            return stoken.stop_requested() || !the_queue.empty();
        });

        // If a stop is requested, do that right now.
        if (stoken.stop_requested())
            break;

        // Print / remove the value at the front of the queue.
        std::cout << "Removing " << the_queue.front() << " from queue" << std::endl;
        the_queue.pop();
    }
}


bool queue_empty()
{
    // This uses a shared lock because it does not modify the queue.
    std::shared_lock lock(the_mutex);
    return the_queue.empty();
}


void shared_and_unique_locks()
{
    std::jthread printer(print_queue);

    for (int i{1}; i <= 5; i++)
    {
        // This uses a unique lock because it modifies the queue.
        std::cout << "Attempt to obtain a unique lock" << std::endl;
        std::unique_lock lock(the_mutex);
        std::cout << "The unique lock was obtained" << std::endl;
        std::cout << "Push " << i << " onto queue" << std::endl;
        the_queue.push(i);
    }

    // Wait till the queue is empty.
    while (!queue_empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
}


namespace timed_mutex_demo {
// If a normal mutex cannot be obtained, this leads to a deadlock.
// A timed mutex will assist in such a case.
// If a lock is requested on a timed mutex, a timeout can be passed too.
// If the lock cannot be obtained in time, it falls in a timeout, not in a deadlock.

static std::timed_mutex the_mutex;

void timed_mutex()
{
    std::cout << "Obtain first lock on the timed mutex" << std::endl;
    const std::unique_lock lock1(the_mutex, std::chrono::seconds(1));
    std::cout << "Obtained first lock" << std::endl;
    std::cout << "Obtain second lock on the same timed mutex" << std::endl;
    const std::unique_lock lock2(the_mutex, std::chrono::milliseconds(10));
    if (!lock2)
    {
        std::cout << "Failed to obtain second lock within 10 milliseconds second" << std::endl;
    }
    else
    {
        std::cout << "Obtained second lock" << std::endl;
    }
}
}

int main()
{
    algorithms_lower_bound_and_upper_bound::demo();
    min_max_clamp_ranges_minmax::demo();
    // latches_and_barriers::demo_latch();
    // latches_and_barriers::demo_barrier();
    // mutual_exclusion::shared_and_unique_locks();
    // timed_mutex_demo::timed_mutex();
    return EXIT_SUCCESS;
}

/*
Copyright (©) 2021-2026 Teus Benschop.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <algorithm>
#include <array>
#include <barrier>
#include <cassert>
#include <chrono>
#include <complex>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <execution>
#include <filesystem>
#include <forward_list>
#include <functional>
#include <future>
#include <ios>
#include <iostream>
#include <iterator>
#include <latch>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <syncstream>
#include <thread>
#include <vector>

#include "bad_coding.h"
#include "bits.h"
#include "bounds_limits.h"
#include "classes.h"
#include "concurrency.h"
#include "constraints.h"
#include "containers.h"
#include "coroutines.h"
#include "counting.h"
#include "exceptions.h"
#include "expected.h"
#include "fold_expressions.h"
#include "functional.h"
#include "language.h"
#include "latches_barriers.h"
#include "modules.h"
#include "clocking.h"
#include "filesystem.h"
#include "searching.h"
#include "shared_mutex.h"
#include "templates.h"
#include "text.h"
#include "transformations.h"
#include "variables.h"












namespace priority_queue {
void demo()
{
    // The input data.
    const auto data = {1, 8, 5, 6, 3, 4, 0, 9, 7, 2};

    // A container adaptor that provides constant time lookup of the largest (by default) element.
    std::priority_queue<int> max_priority_queue;

    // Fill the priority queue.
    for (int n : data)
        max_priority_queue.push(n);

    // Check the top of the queue: contains the largest element.
    assert(max_priority_queue.top() == 9);

    // The std::greater<int> makes the max priority queue act as a min priority queue.
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_priority_queue1(data.begin(), data.end());

    // Check the top of the queue: contains the smallest element.
    assert(min_priority_queue1.top() == 0);

    // Second way to define a min priority queue.
    std::priority_queue min_priority_queue2(data.begin(), data.end(), std::greater<int>());
    assert(min_priority_queue2.top() == 0);

    // Using a custom function object to compare elements.
    struct
    {
        bool operator()(const int l, const int r) const { return l > r; }
    } custom_less;
    std::priority_queue custom_priority_queue(data.begin(), data.end(), custom_less);
    assert(custom_priority_queue.top() == 0);

    // Using lambda to compare elements.
    // This allows for custom ordering, as in this example.
    const auto cmp = [](int left, int right) { return (left ^ 1) < (right ^ 1); };
    std::priority_queue<int, std::vector<int>, decltype(cmp)> lambda_priority_queue(cmp);
    for (int n : data)
        lambda_priority_queue.push(n);
    std::vector<int> lambda_output{};
    while (!lambda_priority_queue.empty())
    {
        lambda_output.push_back(lambda_priority_queue.top());
        lambda_priority_queue.pop();
    }
    std::vector<int> lambda_standard{8, 9, 6, 7, 4, 5, 2, 3, 0, 1};
    assert(lambda_output == lambda_standard);
}
}


namespace time_formatting {
void demo()
{
    const std::time_t t = std::time(nullptr);
    const std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    // The locale determines the language etc.
    //oss.imbue(std::locale("us_EN.utf8"));
    // E.g.: Tue Mar 17 20:18:21 2026 CET
    oss << std::put_time(&tm, "%c %Z") << std::endl;
}
}


namespace basic_memory_management {
void demo()
{
    struct User
    {
        User(const std::string& name) : m_name(name)
        {
        }
        std::string m_name;
    };
    // Established way.
    {
        // Allocate sufficient memory for the User object.
        auto* memory = std::malloc(sizeof(User));
        // Construct new object in existing memory.
        auto* user = new(memory) User("John");
        assert(user->m_name == "John");
        // Call destructor: This does not yet free the memory.
        user->~User();
        // Free memory on heap.
        std::free(memory);
    }
    // Modern way in C++20.
    {
        auto* memory = std::malloc(sizeof(User));
        auto* user_ptr = reinterpret_cast<User*>(memory);
        // Method 1:
        std::uninitialized_fill_n(user_ptr, 1, User{"John"});
        assert(user_ptr->m_name == "John");
        // Method 2:
        std::construct_at(user_ptr, User{"John"});
        assert(user_ptr->m_name == "John");
        // And destroy / free again.
        std::destroy_at(user_ptr);
        std::free(memory);
    }
}
}

namespace linux_signals {
void demo()
{
    return;
    sigset_t blocked_signals;

    bool keep_going{true};

    // Sending a Linux signal can be handled in various ways.
    // 1. It gets handled right away.
    // 2. The process ignores it, nothing happens.
    // 3. The kernel keeps that signal pending,
    //    and delivers it to the process when the process asks for it.

    // A set of signals that the kernel will not deliver to the process asynchronically.
    // The kernel will block those signals and deliver them when asked to do so.
    const auto sync_signals = {SIGINT, SIGUSR1, SIGUSR2, SIGHUP, SIGTERM};

    // A set of ignored signals.
    const auto ignored_signals = {SIGALRM, SIGCHLD, SIGPIPE};

    // A set of signals processed asynchronically, that is, in real time.
    // Commented out for macOS.
    const std::initializer_list<int> async_signals = {/*SIGRTMIN, SIGRTMAX*/};

    // Initialize empty set of signals.
    sigemptyset(&blocked_signals);
    // Add the blocked signals to the set.
    for (const int signal : sync_signals)
        sigaddset(&blocked_signals, signal);
    // Inform kernel to block the list of signals.
    pthread_sigmask(SIG_BLOCK, &blocked_signals, NULL);

    // Ignore the given signals.
    // const struct sigaction sa_ignore = {SIG_IGN, {}, {}, {}};
    // for (const int signal : ignored_signals)
    //     sigaction(signal, &sa_ignore, nullptr);

    // Raise the synchronized signals.
    // Notice that the kernel will not deliver them to the process right away.
    for (const int signal : sync_signals)
    {
        std::cout << "Raise synchronized signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // Raise the ignored signals -> nothing will happen.
    for (const int signal : ignored_signals)
    {
        std::cout << "Raise ignored signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // A real time handler for the asynchronically delivered signals.
    // const struct sigaction sa_async = {
    //     [](const int signal)
    //     {
    //         std::cout << "Processing real time signal " << signal << " " << strsignal(signal) << std::endl;
    //     },
    //     {}, {}, {}
    // };
    // for (const int s : async_signals)
    // {
    //     sigaction(s, &sa_async, nullptr);
    // }

    // Raise the real time signals.
    for (const int signal : async_signals)
    {
        std::cout << "Raise real time signal " << signal << " " << strsignal(signal) << std::endl;
        std::raise(signal);
    }

    // Start processing raised signals in a controlled manner.
    // It will get the lowest pending signal first.
    while (keep_going)
    {
        const timespec timespec{.tv_sec = 1, .tv_nsec = 0};
        const int signal = -1;
        //sigtimedwait(&blocked_signals, nullptr, &timespec);
        //sigwait(&blocked_signals, &signal);
        if (signal == -1)
        {
            switch (errno)
            {
            case EINVAL:
                std::cout << "Invalid timespec" << std::endl;
                break;
            case EAGAIN:
                std::cout << "Timeout waiting for signal" << std::endl;
                keep_going = false;
                break;
            case EINTR:
            default:
                std::cout <<
                    "The signal handler was interrupted by an asynchronous signal handled somewhere else "
                    << strerror(errno) << std::endl;
            }
        }
        else
        {
            std::cout << "Processing synchronized signal " << signal << " " << strsignal(signal) << std::endl;
        }
    }
}
}

namespace meta_programming_recursive_calculation {
// What is template metaprogramming?
// It means that the compiler, using templates, calculates values at compile time.
// Here is an example of recursion to let the compiler calculate values.

// A factorial of, say, 4 means: multiply all numbers from 4 down to 1.

// This template recursively calls itself to calculate its value.
template <int N>
struct Factorial
{
    static constexpr int value = N * Factorial<N - 1>::value;
};

// This template is specific for a passed value of 0.
template <>
struct Factorial<0>
{
    static constexpr int value = 1;
};

// A recursive template is very expensive to process by the compiler.
// A recursive constexpr function is much cheaper.
constexpr int factorial(int n) { return n <= 1 ? 1 : (n * factorial(n - 1));}

static_assert(Factorial<1>::value == 1);
static_assert(factorial(1) == 1);
static_assert(Factorial<4>::value == 24);
static_assert(factorial(4) == 24);
static_assert(Factorial<6>::value == 720);
static_assert(factorial(6) == 720);


// Another example of recursive template calls.
template <int N>
struct Power
{
    enum { value = 2 * Power<N - 1>::value };
};

template <>
struct Power<0>
{
    enum { value = 1 };
};

static_assert(Power<8>::value == 256);
// The above static assert calculates 2 to the power of 8 (2^8).
// Calculation is done at compile time.
// Essential steps.
// 1. Notice the value passed to Power, in this case value 8.
// 2. The "enum" types could also be constexpr types, they work the same.
// 3. When the compiler sees value 8 to the Power, it instantiates the template.
// 4. The compiler sees that the value from Power<7> is needed.
// 5. The compiler keeps creating new instances of Power<n> till Power<0>.
// 6. The compiler now can create Power<8>.
}


namespace remove_const_volatile_reference {
// Demo of removing const, volatile, and reference.
static_assert(std::is_same_v<std::remove_cvref_t<int>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<int&&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<const int[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<const int(&)[2]>, int[2]>);
static_assert(std::is_same_v<std::remove_cvref_t<int(int)>, int(int)>);

void demo()
{
    auto power = [](const auto& v, int n)
    {
        //auto product = decltype(v){1};
        //typename std::remove_cvref<decltype(v)>::type product {1};
        std::remove_cvref_t<decltype(v)> product{1};
        for (int i = 0; i < n; i++)
        {
            product *= v;
        }
        return product;
    };
    assert(power(2, 4) == 16);
}
}


namespace demo_simple_type_traits {
static_assert(std::is_same_v<uint8_t, unsigned char>);
static_assert(std::is_floating_point_v<decltype(3.f)>);
static_assert(std::is_unsigned_v<unsigned int>);
static_assert(not std::is_unsigned_v<int>);

class Planet
{
};

class Mars : public Planet
{
};

class Sun
{
};

static_assert(std::is_base_of_v<Planet, Mars>);
static_assert(std::derived_from<Mars, Planet>);
static_assert(std::is_convertible_v<Mars, Planet>);
static_assert(not std::is_base_of_v<Planet, Sun>);
static_assert(not std::is_base_of_v<Mars, Planet>);

static_assert(std::is_scoped_enum_v<int> == false);

class A
{
};

static_assert(not std::is_scoped_enum_v<A>);

enum B { self_test = std::is_scoped_enum_v<B> };

static_assert(not std::is_scoped_enum_v<B>);
static_assert(!self_test);

enum struct C
{
};

static_assert(std::is_scoped_enum_v<C>);

enum class D : int
{
};

static_assert(std::is_scoped_enum_v<D>);

enum class E;
static_assert(std::is_scoped_enum_v<E>);

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
// static_assert(std::is_implicit_lifetime_v<int>); // arithmetic type is a scalar type
// static_assert(std::is_implicit_lifetime_v<const int>); // cv-qualified a scalar type

// enum E { e };
//    static_assert(std::is_implicit_lifetime_v<E>); // enumeration type is a scalar type
//    static_assert(std::is_implicit_lifetime_v<int*>); // pointer type is a scalar type
//    static_assert(std::is_implicit_lifetime_v<std::nullptr_t>); // scalar type

// struct S
// {
//     int x, y;
// };
//    S is an implicit-lifetime class: an aggregate without user-provided destructor
//    static_assert(std::is_implicit_lifetime_v<S>);
//
//    static_assert(std::is_implicit_lifetime_v<int S::*>); // pointer-to-member

// struct X { ~X() = delete; };
// X is not implicit-lifetime class due to deleted destructor
//    static_assert(!std::is_implicit_lifetime_v<X>);
//
//    static_assert(std::is_implicit_lifetime_v<int[8]>); // array type
//    static_assert(std::is_implicit_lifetime_v<volatile int[8]>); // cv-qualified array type
//
//    static_assert(std::reference_constructs_from_temporary_v<int&&, int> == true);
//    static_assert(std::reference_constructs_from_temporary_v<const int&, int> == true);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, int&&> == false);
//    static_assert(std::reference_constructs_from_temporary_v<const int&, int&&> == false);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, long&&> == true);
//    static_assert(std::reference_constructs_from_temporary_v<int&&, long> == true);


void demo()
{
    struct foo
    {
        void m()
        {
        }

        void m() const
        {
        }

        void m() volatile
        {
        }

        void m() const volatile
        {
        }
    };
    foo{}.m();
    std::add_const_t<foo>{}.m();
    std::add_volatile_t<foo>{}.m();
    std::add_cv_t<foo>{}.m(); // Add const volatile.
}
}


namespace pseudo_random_number_generation {
void demo()
{
    {
        // Poisson distribution.
        // The probability of an event happening a certain number of times (k)
        // within a given interval of time.
        // Construct it around a given mean event count.
        constexpr const float mean_event_count{4.5};
        std::poisson_distribution<int> distribution(mean_event_count);
        std::default_random_engine generator;

        // Store how often an event occurs.
        constexpr const int array_size{10};
        std::array<int, array_size> events{};

        constexpr const int number_of_experiments{1000};
        [[maybe_unused]] constexpr const int number_of_stars{100};

        for (int i = 0; i < number_of_experiments; ++i)
        {
            const int number = distribution(generator);
            if (number < array_size) events[number]++;
        }

        // The Poisson distribution is around a given mean event count.
        assert(mean_event_count == 4.5);
        for (int i = 0; i < array_size; ++i)
        {
            //std::cout << i << ": " << std::string(events[i] * number_of_stars / number_of_experiments, '*') << std::endl;
        }
        // 0: *
        // 1: *****
        // 2: ***********
        // 3: ******************
        // 4: ******************
        // 5: *****************
        // 6: ***********
        // 7: ********
        // 8: ****
        // 9: **

        // Simulate throwing a 6-sided dice.
        {
            // A seed source for the random number engine.
            std::random_device rd;
            // A Mersenne Twister_random engine seeded with the random device.
            std::mt19937 mt_generator(rd());
            // A uniform distribution between 1 and 6 inclusive.
            std::uniform_int_distribution<> uniform_distribution(1, 6);

            // Throw the dice multiple times.
            // std::cout << "Normal distribution for throwing a dice" << std::endl;
            // for (int n = 0; n < 20; n++)
            //     std::cout << uniform_distribution(mt_generator) << " ";
            // std::cout << std::endl;
            // The above will have a normal distribution like this:
            // 4 5 6 6 3 6 6 1 6 1 6 1 1 4 2 2 1 5 1 2
        }
    }
}
}


namespace ranges_views_filter_drop_reverse {
void demo()
{
    // Start from a list of numbers.
    const auto numbers = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Define a function to select even numbers.
    const auto even = [](const int i) -> bool { return 0 == i % 2; };
    // Define a view that filters on even numbers, then drops the first number, then reverses them.
    using namespace std::ranges::views;
    // ReSharper disable once CppLocalVariableMayBeConst
    auto result = numbers | filter(even) | drop(1) | reverse;
    // The view contains: 8 6 4 2
    for (const int i : result)
        assert(i == 8 or i == 6 or i == 4 or i == 2);
}
}


namespace contains_and_contains_subrange {
void demo()
{
    constexpr auto haystack = std::array{3, 1, 4, 1, 5};
    constexpr auto needle = std::array{1, 4, 1};
    constexpr auto bodkin = std::array{2, 5, 2};
    static_assert(std::ranges::contains(haystack, 4));
    static_assert(not std::ranges::contains(haystack, 6));
    static_assert(std::ranges::contains_subrange(haystack, needle));
    static_assert(not std::ranges::contains_subrange(haystack, bodkin));

    constexpr std::array<std::complex<double>, 3> nums{
        {
            {1, 2},
            {3, 4},
            {5, 6}
        }
    };
    static_assert(std::ranges::contains(nums, std::complex<double>{3, 4}));
}
}


namespace ranges_sorting_demo {
void demo()
{
    {
        // Standard sorting demo.
        auto values = std::vector{6, 3, 2, 7, 4, 1, 5};
        assert(!std::ranges::is_sorted(values));
        // Regular C++ sort: std::sort(non_const_values.begin(), non_const_values.end());
        std::ranges::sort(values);
        const auto standard = std::vector{1, 2, 3, 4, 5, 6, 7};
        assert(values == standard);
        assert(std::ranges::is_sorted(values));
    }
    {
        // Demo of sorting on property, in this case, word length.
        auto names = std::vector<std::string>{"Ralph", "Lisa", "Homer", "Maggie", "Apu", "Bart"};
        std::ranges::sort(names, std::less{}, &std::string::size);
        // The names are now sorted on length: "Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"
    }
    {
        // Demo of sorting a struct on two fields.
        struct Player
        {
            std::string name{};
            int level{};
            float health{};
        };
        auto players = std::vector<Player>{
            {"Aki", 1, 9.1f},
            {"Nao", 2, 7.2f},
            {"Rei", 2, 3.3f}
        };
        const auto level_and_health = [](const Player& player)
        {
            return std::tie(player.level, player.health);
        };
        // Order players by level, then by health.
        std::ranges::sort(players, std::less<>{}, level_and_health);
        assert(players.at(0).name == "Aki");
        assert(players.at(1).name == "Rei");
        assert(players.at(2).name == "Nao");
    }
}
}


namespace demo_ranges_various {
void demo()
{
    {
        // ranges::transform.
        const auto input = std::vector{1, 2, 3, 4};
        auto output = std::vector<int>(input.size());
        auto square = [](auto&& i) -> int { return i * i; };
        std::ranges::transform(input, output.begin(), square);
        // The output will be: 1 4 9 16
        const auto standard = std::vector<int>{1, 4, 9, 16};
        assert(output == standard);
    }
    {
        // ranges::views::transform.
        const auto input = std::vector{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        const auto square = [](auto i) { return i * i; };
        // Create a view, but do not yet evaluate this view.
        auto squared_view = std::ranges::views::transform(input, square);
        // Iterate over the squared view, which invokes evaluation and so invokes the lambda.
        int sum{0};
        for (auto s : squared_view)
            sum += s;
        // The transformed output will be: 1 4 9 16 25 36 49 64 81 100
        assert(sum == 385);
    }
    {
        // Demonstration of ranges fill.
        std::vector<int> v(5);
        std::ranges::fill(v, 123);
        // The output will be:  123 123 123 123 123
        const std::vector<int> standard = {123, 123, 123, 123, 123};
        assert(v == standard);
    }
    {
        // Demonstration of ranges generate.
        std::vector<int> v(4);
        std::ranges::generate(v, std::rand);
        // The output will be random, e.g. 16807 282475249 1622650073 984943658
    }
    {
        // Demonstration of ranges iota.
        auto v = std::list<int>(6);
        // Fill the list with ascending values: 0, 1, 2, ...
        std::iota(v.begin(), v.end(), 0);
        const auto standard = std::list<int>({0, 1, 2, 3, 4, 5});
        assert(v == standard);
        //std::ranges::iota(v, 0);

        // A vector of iterators.
        // Fill with iterators to consecutive list's elements.
        std::vector<std::list<int>::iterator> vec(v.size());
        std::iota(vec.begin(), vec.end(), v.begin());
        //std::ranges::iota(vec.begin(), vec.end(), list.begin());
    }
    {
        // Demo of shuffle.
        // Numbers in sequential order.
        auto numbers = std::vector{1, 2, 3, 4, 5};
        assert(std::ranges::is_sorted(numbers));

        // A Mersenne Twister generator.
        std::random_device rd;
        std::mt19937 mtg(rd());

        // Shuffle the numbers.
        std::ranges::shuffle(numbers, mtg);

        // The output is in random order, e.g. 2 4 3 5 1.
        assert(!std::ranges::is_sorted(numbers ));
    }

    // Simple find integer value.
    {
        const auto numbers = std::list{2, 4, 3, 2, 3, 1};
        if (const auto it = std::ranges::find(numbers, 2);
            it != numbers.cend())
            assert(*it == 2);
    }

    // Find by specifying to search on a struct member.
    {
        struct person
        {
            unsigned id;
            const char* name;
            const char* job;
        };
        const auto persons = std::list<person>{
            {0, "Ana", "barber"},
            {1, "Bob", "cook"},
            {2, "Eve", "builder"}
        };
        if (const auto it = std::ranges::find(persons, "Bob", &person::name);
            it != persons.cend())
        {
            assert(it->id == 1);
            assert(it->name == std::string("Bob"));
            assert(it->job == std::string("cook"));
        }
    }

    // Demo of find on word length.
    {
        const auto names = std::vector<std::string>{"Apu", "Lisa", "Bart", "Ralph", "Homer", "Maggie"};
        const auto iterator = std::ranges::find(names, 4, &std::string::size);
        assert(iterator != names.cend());
        assert(*iterator == "Lisa");
    }


    // Demo of finding duplicates in a range.
    {
        const auto contains_duplicates_n2 = [](auto begin, auto end)
        {
            for (auto it = begin; it != end; ++it)
                if (std::find(std::next(it), end, *it) != end)
                    return true;
            return false;
        };
        const auto contains_duplicates_allocating = [](auto first, auto last)
        {
            // As (*first) returns a reference, we have to get the base type using std::decay_t
            using value_type = std::decay_t<decltype(*first)>;
            auto copy = std::vector<value_type>(first, last);
            std::sort(copy.begin(), copy.end());
            // The std::adjacent_find searches the sorted range for two consecutive equal elements.
            return std::adjacent_find(copy.begin(), copy.end()) != copy.end();
        };
        const auto vals = std::vector{1, 4, 2, 5, 3, 6, 4, 7, 5, 8, 6, 9, 0};
        const auto a = contains_duplicates_n2(vals.cbegin(), vals.cend());
        assert(a);
        const auto b = contains_duplicates_allocating(vals.cbegin(), vals.cend());
        assert(b);
    }

    // Demo of ranges::find_if
    {
        const auto values = {4, 1, 3, 2};
        const auto is_even = [](int x) { return x % 2 == 0; };
        const auto iter = std::ranges::find_if(values, is_even);
        assert(iter != values.end());
        assert(*iter == 4);
    }

    // Getting the maximum value.
    {
        struct Student
        {
            int year{};
            int score{};
            std::string name{};
        };

        auto get_max_score_copy = [](const std::vector<Student>& students, int year)
        {
            const auto by_year = [year](const auto& s) { return s.year == year; };
            // The student list needs to be copied in order to filter on the year.
            auto v = std::vector<Student>{};
            std::ranges::copy_if(students, std::back_inserter(v), by_year);
            auto it = std::ranges::max_element(v, std::less{}, &Student::score);
            return it != v.end() ? it->score : 0;
        };

        auto get_max_score = [](const std::vector<Student>& students, int year)
        {
            auto max_value = [](auto&& range)
            {
                const auto it = std::ranges::max_element(range);
                return it != range.end() ? *it : 0;
            };
            const auto by_year = [year](auto&& s) { return s.year == year; };
            return max_value(students | std::views::filter(by_year) | std::views::transform(&Student::score));
        };

        auto get_max_score_explicit_views = [](const std::vector<Student>& s, int year)
        {
            auto by_year = [year](const auto& s) { return s.year == year; };
            const auto v1 = std::ranges::ref_view{s}; // Wrap container in a view.
            const auto v2 = std::ranges::filter_view{v1, by_year};
            auto v3 = std::ranges::transform_view{v2, &Student::score};
            auto it = std::ranges::max_element(v3);
            return it != v3.end() ? *it : 0;
        };

        const auto students = std::vector<Student>{
            {3, 120, "A"},
            {2, 140, "B"},
            {3, 190, "C"},
            {2, 110, "D"},
            {2, 120, "E"},
            {3, 130, "F"},
        };
        {
            auto score = get_max_score_copy(students, 2);
            assert(score == 140);
        }
        {
            auto score = get_max_score(students, 2);
            assert(score == 140);
        }
        {
            auto score = get_max_score_explicit_views(students, 2);
            assert(score == 140);
        }
    }

    // Demo of join.
    {
        const auto list_of_lists = std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}, {5}, {4, 3, 2, 1}};
        const auto flattened_view = std::views::join(list_of_lists);
        std::vector<int> result = flattened_view | std::ranges::to<std::vector<int>>();
        // Result: 1 2 3 4 5 5 4 3 2 1
        const auto standard = std::vector<int>{1, 2, 3, 4, 5, 5, 4, 3, 2, 1};
        assert(result == standard);
    }

    // Demo of take.
    {
        auto vec = std::vector{4, 2, 7, 1, 2, 6, 1, 5};
        // Create a view of the first half of the container.
        const auto first_half = vec | std::views::take(vec.size() / 2);
        // This sorts the first half of the original vector via the view.
        std::ranges::sort(first_half);
        const auto standard = std::vector{1, 2, 4, 7, 2, 6, 1, 5};
        assert(vec == standard);
    }

    // Demo of split and join.
    {
        // The input CSV data.
        auto csv = std::string{"10,11,12"};
        auto digits = csv | std::ranges::views::split(',');
        // Result:  [ [1, 0], [1, 1], [1, 2] ]
        auto joined = digits | std::views::join;
        // Result [ 1, 0, 1, 1, 1, 2 ]
        auto result = joined | std::ranges::to<std::vector<char>>();
        std::vector standard = {'1', '0', '1', '1', '1', '2'};
        assert(result == standard);
    }

    // Demo of drop_while.
    {
        auto vec = std::vector{1, 2, 3, 4, 5, 4, 3, 2, 1};
        // Apply the range adaptor that represents view of elements from an underlying sequence,
        // beginning at the first element for which the predicate returns false.
        auto v = vec | std::views::drop_while([](auto i) { return i < 5; });
        // Result: 5 4 3 2 1
        auto result= v | std::ranges::to<std::vector<int>>();
        auto standard = std::vector<int>{5, 4, 3, 2, 1};
        assert(result == standard);
    }

    // Demo of istream_view.
    {
        const auto s{"1.4142 1.618 2.71828 3.14159 6.283"};
        auto iss = std::istringstream{s};
        auto floats = std::ranges::istream_view<float>(iss);
        auto result = floats | std::ranges::to<std::vector<float>>();
        std::vector<float> standard = {1.4142, 1.618, 2.71828, 3.14159, 6.283};
        assert(result == standard);
    }
}
}


namespace regex {
void demo()
{
    std::string s =
        R"(Some people, when confronted with a problem, think "I'll use regular expressions." Now they have two problems.)";

    // Case-insensitive search for "regular expressions".
    std::regex self_regex("REGULAR EXPRESSIONS", std::regex_constants::ECMAScript | std::regex_constants::icase);
    assert(std::regex_search(s, self_regex));

    // Regex that finds words.
    std::regex word_regex(R"((\w+))");
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
    auto words_end = std::sregex_iterator();

    // The number of words found.
    assert(std::distance(words_begin, words_end) == 18);

    // Words it found:
    // Some people when confronted with a problem think I ll use regular expressions Now they have two problems
    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        std::smatch match = *i;
        [[maybe_unused]] std::string match_str = match.str();
    }

    std::regex long_word_regex(R"((\w{7,}))");
    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
    assert(
        new_s ==
        R"(Some people, when [confronted] with a [problem], think "I'll use [regular] [expressions]." Now they have two [problems].)");
}
}


namespace source_location {
// https://en.cppreference.com/w/cpp/utility/source_location
void demo()
{
    constexpr int line = __LINE__;
    const std::source_location location = std::source_location::current();
    const std::string file_name = location.file_name();
    assert(file_name.contains("main.cpp"));
    assert(location.line() == line + 1);
    assert(location.column() == 43);
    assert(location.function_name() == std::string("void source_location::demo()"));
}
}


namespace barrier_jthread_stop_token {
void demo()
{
    // This lambda starts the failing processes.
    // On any failure, it restarts the processes.
    const auto resilient_processes = [](const std::stop_token& stop_token)
    {
        // Protect the barrier from being arrived at multiple times
        // due to multiple processes that may all fail simultaneously.
        std::atomic<bool> barrier_active{false};
        // As soon as the barrier is complete, clear the associated protecting flag.
        auto on_barrier_completion = [&]() noexcept
        {
            barrier_active = false;
        };
        // The barrier has count 2:
        // One to allow arriving and waiting at the barrier after all processes have been created.
        // The second one to use for the error callback.
        std::barrier barrier(2, on_barrier_completion);

        // Allow the main program to interrupt the processes loop.
        std::stop_callback stop_callback(stop_token, [&]()
        {
            if (barrier_active)
            {
                barrier_active = false;
                [[maybe_unused]] const auto token = barrier.arrive();
            }
        });

        bool slow_restart_down{false};

        while (!stop_token.stop_requested())
        {
            try
            {
                const auto on_event = [&]([[maybe_unused]] const std::string& error)
                {
                    // Arrive at the barrier.
                    // This will open the barrier, so all processes get recreated.
                    if (barrier_active)
                    {
                        barrier_active = false;
                        [[maybe_unused]] const auto token = barrier.arrive();
                        slow_restart_down = true;
                    }
                };

                // Wait shortly to avoid fast repeating error events.
                if (slow_restart_down)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }

                // The processes start here.
                barrier_active = true;

                // Call the on_event callback to simulate an error.
                const auto generate_error = [&]()
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    on_event("Error");
                };
                std::thread(generate_error).detach(); // Better not detach in production code.

                // At this point all processes have started.

                // Wait here till an error occurs or the program shuts down.
                barrier.arrive_and_wait();
            }
            catch (const std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

    std::jthread thread(resilient_processes);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // The thread goes out of scope here, so it gets the stop token.
}
}


namespace heterogenous_collections_with_variant {
void demo()
{
    using variant_t = std::variant<int, std::string, bool>;
    {
        auto container = std::vector<variant_t> { false, "hello", "world", 13 };
        std::ranges::reverse(container);
    }
    {
        constexpr std::string needle {"needle"};
        const auto v = std::vector<variant_t>{42, needle, true};
        for (const auto& item : v) {
            std::visit([]([[maybe_unused]] const auto& x)
            {
            //     assert ((x == 42) or (std::string(x) == needle) or (x == true));
            //     //std::cout << "Variant has: " << x << std::endl;
            }, item);
        }
        const auto num_bools = std::ranges::count_if(v, [](const auto& item) {
          return std::holds_alternative<bool>(item);
        });
        assert(num_bools == 1);
        auto contains_needle_string = std::ranges::any_of(v, [&](const auto& item) {
          return std::holds_alternative<std::string>(item) and std::get<std::string>(item) == needle;
        });
        assert(contains_needle_string);
    }
}
}


namespace atomic_reference {
void demo()
{
    struct Statistics {
        int heads{};
        int tails{};
    };
    auto stats = Statistics{};

    const auto random_int = [](int min, int max) {
        // One engine instance per thread.
        static thread_local auto engine = std::default_random_engine{std::random_device{}()};
        auto distribution = std::uniform_int_distribution<>{min, max};
        return distribution(engine);
    };

    const auto flip_coin = [&](std::size_t n, auto& outcomes) {
        auto flip = [&](auto n) {
            const auto heads = std::atomic_ref<int>{outcomes.heads};
            const auto tails = std::atomic_ref<int>{outcomes.tails};
            for (auto i = 0u; i < n; ++i) {
                random_int(0, 1) == 0 ? ++heads : ++tails;
            }
        };
        auto t1 = std::jthread{flip, n / 2};       // First half.
        auto t2 = std::jthread{flip, n - (n / 2)}; // The rest.
    };

    flip_coin(5000, stats); // Flip 5000 times
    assert(stats.heads + stats.tails == 5000);
}
}










int main()
{
    scoped_timer::demo();
    searching::demo();
    lower_bound_and_upper_bound::demo();
    counting::demo();
    min_max_clamp_ranges_minmax::demo();
    latches::demo();
    barriers::demo();
    unconstrained_errors::demo();
    constraint_derived_from::demo();
    demonstrate_constraints::demo();
    shared_mutex::demo();
    clocking::demo();
    filesystem::demo();
    member_function::demo();
    priority_queue::demo();
    time_formatting::demo();
    basic_memory_management::demo();
    linux_signals::demo();
    remove_const_volatile_reference::demo();
    demo_simple_type_traits::demo();
    pseudo_random_number_generation::demo();
    ranges_views_filter_drop_reverse::demo();
    contains_and_contains_subrange::demo();
    ranges_sorting_demo::demo();
    demo_ranges_various::demo();
    regex::demo();
    source_location::demo();
    barrier_jthread_stop_token::demo();
    heterogenous_collections_with_variant::demo();
    atomic_reference::demo();
    coroutines::demo();
    modules::demo();
    concurrency::demo();
    fold_expressions::demo();
    containers::demo();
    language::demo();
    templates::demo();
    variables::demo();
    functional::demo();
    bad_coding::demo();
    expected::demo();
    text::demo();
    classes::demo();
    exceptions::demo();
    transformations::demo();
    bits::demo();

    return EXIT_SUCCESS;
}


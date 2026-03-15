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
#include <bit>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <latch>
#include <list>
#include <mutex>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <thread>
#include <unordered_set>
#include <vector>


namespace snippets {
template <typename D>
class scoped_timer
{
    std::chrono::time_point<std::chrono::system_clock, D> m_started_at;

public:
    scoped_timer() noexcept : m_started_at(std::chrono::time_point_cast<D>(std::chrono::system_clock::now()))
    {
    }

    ~scoped_timer() noexcept
    {
        const auto stopped_at = std::chrono::time_point_cast<D>(std::chrono::system_clock::now());
        std::cout << "elapsed " << (stopped_at - m_started_at) << std::endl;
    }
};

void demo_scoped_timer()
{
    const auto timer = scoped_timer<std::chrono::microseconds>{};
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}
}


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
    return;
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
    return;
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

void print_queue(const std::stop_token& stoken)
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
    return;
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
    return;
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

namespace containers_library {
void demo_contains()
{
    constexpr auto word{"word"};
    auto bag = std::multiset<std::string>{};
    bag.insert(word);
    assert(bag.contains(word));
    assert(!bag.contains(""));
}

// If a std::vector gets resized, it moves all of its data to another memory location.
// The std::list does not do that.
// This demo demonstrates that behavior.
void demo_vector_vs_list_allocation()
{
    std::vector<int> vector{};
    std::list<int> list{};
    vector.resize(10);
    list.resize(10);
    const auto address_vector = std::addressof(vector.front());
    const auto address_list = std::addressof(list.front());
    for (auto i{0}; i < 100; ++i)
    {
        vector.push_back(1);
        list.push_back(1);
    }
    assert(address_vector != std::addressof(vector.front()));
    assert(address_list == std::addressof(list.front()));
}

void demo_set_with_hash_key_and_comparator()
{
    struct Display
    {
        int id;
        std::string message;
    };

    // A hash function object to provide a fingerprint of the visual characteristics of a Display.
    struct DisplayHash
    {
        // Enable the C++14 overloads of std::find (but no longer needed now).
        using is_transparent = void;

        size_t operator()(const Display& display) const noexcept
        {
            return std::hash<std::string>{}(display.message);
        }
    };

    // An Equal-to object to satisfy the visual characteristics' comparison.
    struct DisplayEquals
    {
        // Enable the C++14 overloads of std::find (but no longer needed now).
        using is_transparent = void;

        bool operator()(const Display& dl, const Display& dr) const noexcept
        {
            return dl.id == dr.id and dl.message == dr.message;
        }
    };

    // An unordered set with hash function to generate the key, and an equal operator.
    std::unordered_set<Display, DisplayHash, DisplayEquals> display_set{};

    // Insert three Displays into the set.
    // Check that the size is three.
    const Display display1{.id = 1, .message = "1"};
    const Display display2{.id = 2, .message = "2"};
    const Display display3{.id = 3, .message = "3"};
    display_set.insert(display1);
    display_set.insert(display2);
    display_set.insert(display3);
    assert(display_set.size() == 3);

    // Attempt to insert a Display that already exists in the set.
    // The hash function ensures that this does not get inserted into the set.
    // Check that the size remains 3.
    display_set.insert(display3);
    assert(display_set.size() == 3);

    // Check that the find function works.
    const auto iter1 = display_set.find(display1);
    assert(iter1 != display_set.end());
    assert(iter1->id == display1.id);
    assert(iter1->message == display1.message);
}
}


namespace demo_piecewise_construct {
void demo()
{
    enum Construction { from_tuple, from_int_float };

    struct Foo
    {
        Construction construction;

        // Constructor from a tuple.
        explicit Foo(std::tuple<int, float>)
        {
            construction = from_tuple;
        }

        // Constructor from an int and a float.
        explicit Foo(int, float)
        {
            construction = from_int_float;
        }
    };

    const std::tuple<int, float> tuple(1, 1.0f);

    // This creates a pair of Foo objects from a tuple.
    const std::pair<Foo, Foo> p1{tuple, tuple};
    assert(p1.first.construction == Construction::from_tuple);
    assert(p1.second.construction == Construction::from_tuple);

    // This creates the same as above but then piecewise, so from an int and a float.
    const std::pair<Foo, Foo> p2{std::piecewise_construct, tuple, tuple};
    assert(p2.first.construction == Construction::from_int_float);
    assert(p2.second.construction == Construction::from_int_float);
}
}

namespace demo_forward_as_tuple {
void demo()
{
    // The helper "forward_as_tuple" takes a variadic pack of arguments
    // and creates a tuple out of them.
    // It determines the types of the elements of the arguments:
    // if it receives an lvalue then it will have an lvalue reference,
    // and if it receives an rvalue then it will have an rvalue reference.

    // Function returns an rvalue.
    const auto universe = [] -> std::string
    {
        return "universe";
    };

    // The variable i is a lvalue.
    int i = 42;

    [[maybe_unused]] auto tuple = std::forward_as_tuple(i, universe());

    // An lvalue reference binds to an lvalue. Marked with one ampersand (&).
    // An rvalue reference binds to an rvalue. Marked with two ampersands(&&)
    static_assert(std::is_same_v<decltype(tuple), std::tuple<int&, std::string&&>>);
}
}


namespace class_design_idioms {
// 1. RAII (Resource Acquisition Is Initialization)
void raii_resource_acquisition_is_initialization()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(10); // Resource acquired
    // Use ptr
    // Resource released automatically when ptr goes out of scope
}

// 2. Pimpl (Pointer to Implementation)
// Public interface is in header file.
// Private implementation is in another file.
// The public interface has a pointer to the implementation.
class PimplPublic
{
public:
    PimplPublic();
    ~PimplPublic();
    void work() const;

private:
    class Pimpl; // Forward declaration.
    Pimpl* m_pimpl; // Pointer to implementation;
};

class PimplPublic::Pimpl
{
public:
    void internal_work() const
    {
        // Implementation details...
    }
};

PimplPublic::PimplPublic() : m_pimpl(new Pimpl())
{
}

PimplPublic::~PimplPublic() { delete m_pimpl; }

void PimplPublic::work() const
{
    m_pimpl->internal_work();
}


// 3. CRTP (Curiously Recurring Template Pattern).

template <typename Derived>
class CrtpBase
{
public:
    void common_function()
    {
        // Base class implementation.
        static_cast<Derived*>(this)->specific_function(); // Call derived class method.
    }

    virtual ~CrtpBase() = default;
};

class DerivedClass : public CrtpBase<DerivedClass>
{
public:
    void specific_function()
    {
        value++;
    }

    int value{};
};

void curiously_recurring_template_pattern()
{
    DerivedClass derived;
    // Call function from base class -> calls the derived function.
    derived.common_function();
    // Call to the derived function.
    derived.specific_function();
    // Assert it ran "specific_function" in both cases.
    assert(derived.value == 2);
}

// 4. Copy and Swap Idiom
// It swaps the current object with a copy of the object being assigned.

class CopySwapClass
{
public:
    // Constructor.
    CopySwapClass(size_t size) : m_size(size), m_data(new int(size))
    {
    }

    // Destructor.
    ~CopySwapClass() { delete m_data; }
    // Copy constructor.
    CopySwapClass(const CopySwapClass& other)
        : m_size(other.m_size), m_data(new int(other.m_size))
    {
        std::copy(other.m_data, other.m_data + m_size, m_data);
    }

    // The swap function.
    friend void swap(CopySwapClass& first, CopySwapClass& second) noexcept
    {
        std::swap(first.m_size, second.m_size);
        std::swap(first.m_data, second.m_data);
    }

    // Copy assignment operator using copy and swap.
    CopySwapClass& operator=(CopySwapClass other) noexcept
    {
        swap(*this, other);
        return *this;
    }

private:
    size_t m_size;
    int* m_data;
};
}

namespace class_design_principles {
namespace single_responsibility_principle {
// 1. Single Responsibility Principle
// ----------------------------------
// A class should have only one reason to change.

// Class with 3 responsibilities, hence 3 reasons to change.
class BadInvoice
{
    float calculate() { return 0.0f; }
    void print() { std::cout << "Invoice" << std::endl; }
    void save() { std::cout << "Save" << std::endl; }
};

// Good: Classes with each only one responsibility.
class GoodInvoice
{
};

class CalculateInvoice
{
    CalculateInvoice([[maybe_unused]] const GoodInvoice invoice)
    {
    };

    void calculate()
    {
    }
};

class PrintInvoice
{
public:
    PrintInvoice([[maybe_unused]] const GoodInvoice invoice)
    {
    };

    void print()
    {
    }
};

class SaveInvoice
{
    SaveInvoice([[maybe_unused]] const GoodInvoice invoice)
    {
    };

    void save()
    {
    }
};
}

namespace open_closed_principle {
// 2. Open-Closed Principle
// ------------------------
// Open for extension, closed for modification.
// Add new functionality without altering existing functionality.
// Existing code: Class aave to database.
// New code: Class save to file.
// Solution:
// 1. Base class with virtual "save" method.
// 2. Database class that overrides "save".
// 3. File class that overrides "save".
// Hence, if database save already exists,
// file save can be added without changing existing code.
class Invoice
{
};

class BaseSaveInvoice
{
public:
    virtual void save(Invoice invoice) = 0;
    virtual ~BaseSaveInvoice() = default;
};

class DatabaseSave : public BaseSaveInvoice
{
public:
    void save([[maybe_unused]] Invoice invoice) override
    {
    };
};

class FileSave : public BaseSaveInvoice
{
public:
    void save([[maybe_unused]] Invoice invoice) override
    {
    };
};
}

namespace liskov_substitution_principle {
// 3. Liskov Substitution Principle
// --------------------------------
// The derived (sub)class should be passable instead of the base (parent) class.
// The derived class should extend the capabilities of the base class and not narrow it down.
class Bike
{
public:
    virtual void start_engine() const = 0;
};

class Motorcycle : public Bike
{
public:
    void start_engine() const
    {
    };
};

class Bicycle : public Bike
{
public:
    void start_engine() const { throw std::runtime_error("No engine available"); };
};

void demo()
{
    const auto start_engine = [](const Bike& bike)
    {
        bike.start_engine();
    };
    Motorcycle motorcycle;
    Bicycle bicycle;
    start_engine(motorcycle); // Works.
    try
    {
        start_engine(bicycle); // Throws.
    }
    catch (const std::exception& exception)
    {
        assert(exception.what() == std::string("No engine available"));
    }
}
}

namespace interface_segregation_principle {
// 4. Interface Segregation Principle
// ----------------------------------
// Split larger interfaces up into more specific ones.
// Clients should only know about methods of interest to them.
// Clients should not be forced to depend on methods it does not use.

struct BadEmployee
{
    BadEmployee()
    {
    };
    virtual void serve() = 0;
    virtual void manage() = 0;
};

struct BadWaiter : BadEmployee
{
    void serve() override
    {
    };

    void manage() override
    {
        /* not my work */
    }
};

struct IGoodEmployee
{
    IGoodEmployee()
    {
    };
};

struct IGoodWaiter : IGoodEmployee
{
    virtual void serve() { std::cout << "serving" << std::endl; }
};

struct IGoodManager : IGoodEmployee
{
    virtual void manage() { std::cout << "managing" << std::endl; }
};

struct Waiter : IGoodWaiter
{
    void serve() override { std::cout << "waiting" << std::endl; };
};

struct Director : IGoodManager
{
    void manage() override { std::cout << "directing" << std::endl; };
};
}

namespace dependency_inversion_principle {
// 5. Dependency Inversion Principle
// ---------------------------------
// A class should depend on interfaces or abstract classes rather than on concrete types.
// This makes the class more flexible.

struct BadKeyboard
{
};

struct BadLaptop
{
    BadLaptop(const BadKeyboard keyboard) : m_keyboard(keyboard)
    {
    }

    BadKeyboard m_keyboard;
};

struct GoodKeyboard
{
    virtual ~GoodKeyboard() = default;
    virtual int get_type() const = 0;
};

struct WiredKeyboard : GoodKeyboard
{
    int get_type() const override { return 1; }
};

struct GoodLaptop
{
    GoodLaptop(const GoodKeyboard* keyboard) : m_keyboard(keyboard)
    {
    }

    const GoodKeyboard* m_keyboard;
};

// In the bad implementation only one keyboard type can be passed.
// In the good implementation any keyboard type derived from the interface can be passed.
}
}

namespace language_declarations {
// The keyword "const" indicates immutability.
const int i1 = 10;

// Keyword "constexpr":
// If possible evaluate it at compile time.
constexpr int i2 = 10;

// Will be evaluated at compile time if the parameters "var1" and "var2" are known at compile time.
// Will be evaluated at run time if the parameters are not known.
constexpr int constexpr_add(int var1, int var2)
{
    return var1 + var2;
}

// consteval : Force evaluation of a function at compile time.
consteval int consteval_add(int var1, int var2)
{
    return var1 + var2;
}

// constinit : Static initialization. Force evaluation at compile time. The variable is not const.
constinit int i3 = 1;

void declarations()
{
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1, 2) == 3);
    static_assert(constexpr_add(i1, i2) == 20);

    // Variable i3 is assigned at runtime.
    [[maybe_unused]] int i3 = 2;
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);

    // The variable is initialized at runtime although the function is constexpr.
    [[maybe_unused]] int i4 = constexpr_add(i3, i3);
    assert(i4 == 4);

    // OK, evaluated at compile-time.
    static_assert(consteval_add(1, 1) == 2);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1, 2) == 3);
    static_assert(constexpr_add(i1, i2) == 20);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);

    constexpr auto if_constexpr_add = [](auto var1, auto var2)
    {
        // This section is compiled only if it passes, else it's omitted, and so cannot cause compiler errors.
        if constexpr (std::is_same_v<decltype(var1), int>)
            return var1 + var2;
        return var1 + var2;
    };
    static_assert(if_constexpr_add(1, 2) == 3);

    {
        constexpr const auto xdigit = [](int n) -> char
        {
            // This is a constexpr variable in a constexpr lambda function: OK in C++23.
            constexpr const char digits[] = "0123456789abcdef";
            return digits[n];
        };
        static_assert(xdigit(2) == '2');
    }
}
}

namespace language_exceptions {
// It is good practice to have a hierarchy of exception types.

// Example: The base exception is derived from the standard exception.
// The type 1 and type 2 exceptions are derived from the base exception.

class BaseException : public std::exception
{
    const std::string m_what;

public:
    explicit BaseException(std::string_view what = "") noexcept : m_what(what)
    {
    }

    const char* what() const noexcept final override { return m_what.c_str(); }
};

struct Type1Exception : BaseException
{
    explicit Type1Exception(std::string_view what = "") noexcept : BaseException(what)
    {
    }
};

struct Type2Exception : BaseException
{
    explicit Type2Exception(std::string_view what = "") noexcept : BaseException(what)
    {
    }
};

// This template function gets passed an exception, throws it, and catches it.
// The "catch" clauses are put in the correct order:
// 1. Catch the most derived types.
// 2. Catch the base exception.
// 3. Catch the standard exception.
template <typename Exception>
void demo_exception_catch_hierarchy(const Exception& e)
{
    try
    {
        throw e;
    }
    catch (const Type1Exception& exception)
    {
        std::cout << "Catch Type1Exception" << std::endl;
    }
    catch (const Type2Exception& exception)
    {
        std::cout << "Catch Type2Exception" << std::endl;
    }
    catch (const BaseException& exception)
    {
        std::cout << "Catch BaseException" << std::endl;
    }
    catch (const std::exception& exception)
    {
        std::cout << "Catch std::exception" << std::endl;
    }
    catch (...)
    {
        std::cout << "Catch unknown exception" << std::endl;
    }
}

void demo()
{
    return;
    // One exception handling function to be used in several places.
    // It uses dynamic casting to handle the different exceptions.
    const auto exception_handler = [](const std::exception* exception)
    {
        if (const auto* e = dynamic_cast<const Type1Exception*>(exception); e)
            std::cout << "Catch Type1Exception" << std::endl;
        else if (const auto* e = dynamic_cast<const Type2Exception*>(exception); e)
            std::cout << "Catch Type2Exception" << std::endl;
        else if (const auto* e = dynamic_cast<const BaseException*>(exception); e)
            std::cout << "Catch BaseException" << std::endl;
        else if (const auto* e = dynamic_cast<const std::exception*>(exception); e)
            std::cout << "Catch standard exception" << std::endl;
        else
            std::cout << "Catch unknown exception" << std::endl;
    };

    // Throw different exceptions and print the exception caught via the exception handler.
    try
    {
        std::cout << "Throw Type1Exception" << std::endl;
        throw Type1Exception();
    }
    catch (const std::exception& e)
    {
        exception_handler(&e);
    }
    try
    {
        std::cout << "Throw Type2Exception" << std::endl;
        throw Type2Exception();
    }
    catch (const std::exception& e)
    {
        exception_handler(&e);
    }
    try
    {
        std::cout << "Throw BaseException" << std::endl;
        throw BaseException();
    }
    catch (const std::exception& e)
    {
        exception_handler(&e);
    }
    try
    {
        std::cout << "Throw standard exception" << std::endl;
        throw std::runtime_error("standard");
    }
    catch (const std::exception& e)
    {
        exception_handler(&e);
    }

    // Throw different types of exceptions.
    // The template function uses the standard try ... catch idiom.
    std::cout << "Throw Type1Exception" << std::endl;
    demo_exception_catch_hierarchy(Type1Exception());
    std::cout << "Throw Type2Exception" << std::endl;
    demo_exception_catch_hierarchy(Type2Exception());
    std::cout << "Throw BaseException" << std::endl;
    demo_exception_catch_hierarchy(BaseException());
    std::cout << "Throw standard exception" << std::endl;
    demo_exception_catch_hierarchy(std::runtime_error(""));
}
}

namespace lambda_capture_demo {
void demo()
{
    return;
    // Capture by value.
    {
        auto v = 7;
        // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
        // Mark it mutable because it mutates the captured v.
        // Note that it captures v once, and stores it in the lambda function.
        // Hence, the increased v is kept and can be increased once more and so on.
        auto lambda = [v] mutable
        {
            v++;
            std::cout << "Captured v by value = " << v << std::endl;
        };
        lambda();
        lambda();
        std::cout << "Original v = " << v << std::endl;
    }

    // Capture by reference.
    {
        auto v = 7;
        auto lambda = [&v]()
        {
            ++v;
            std::cout << "Captured v by reference = " << v << std::endl;
        };
        lambda();
        lambda();
        std::cout << "Original v = " << v << std::endl;
    }
}
}

namespace assign_two_lambdas_to_same_function_object {
void demo()
{
    return;
    // Create an unassigned std::function object.
    auto func = std::function<void(int)>{};

    // Assign a lambda without capture to the std::function object.
    func = [](int v)
    {
        std::cout << "Assigned lambda without capture: " << v << std::endl;
    };
    func(12); // Prints 12.

    // Assign a lambda with capture to the same std::function object.
    auto v42 = 42;
    func = [v42](int v)
    {
        std::cout << "Assigned lambda with capture: " << (v + v42) << std::endl;
    };
    func(12); // Prints 54.
}
}

namespace stateless_lambda_function {
// A stateless lambda (function) does not retain any data or memory
// from one execution to the next.
const auto stateless = []
{
};
// It is assignable.
const auto stateless2 = stateless;
// Default-constructible (i.e. constructor without parameters, or with default parameters).
static_assert(std::is_default_constructible_v<decltype(stateless)>); // passes
const decltype(stateless) stateless3;
static_assert(std::is_same_v<decltype(stateless), decltype(stateless2)>); // passes
static_assert(std::is_same_v<decltype(stateless), decltype(stateless3)>); // passes
static_assert(std::is_same_v<decltype(stateless2), decltype(stateless3)>); // passes
}


namespace keyword_auto_for_lambdas {
// Using keyword "auto" for lambda functions.
constexpr auto lambda_typename = []<typename T>(T v) { return v + 1; };
constexpr auto lambda_auto = [](auto v) { return v + 1; };
static_assert(lambda_typename('a') == 'b');
static_assert(lambda_auto('a') == 'b');
static_assert(lambda_typename(1) == 2);
static_assert(lambda_auto(1) == 2);
static_assert(lambda_typename(static_cast<std::uint64_t>(41)) == 42);
static_assert(lambda_auto(static_cast<std::uint64_t>(41)) == 42);
}


namespace operator_overloading {
// A struct with overloaded operators.

struct Container
{
    constexpr Container(const int value) : value(value)
    {
    }

    int value;

    // Overload the "+" operator.
    constexpr Container operator+(const Container& other)
    {
        return Container(value + other.value);
    }

    // Overload the "()" operator.
    constexpr int operator()() { return value; }

    // Overload the += and the -= and the %= operators.
    constexpr Container& operator+=(const Container& c) noexcept
    {
        value += c.value;
        return *this;
    }

    constexpr Container& operator-=(const Container& c) noexcept
    {
        value -= c.value;
        return *this;
    }

    constexpr Container& operator%=(const Container& c) noexcept
    {
        value = value % c.value;
        return *this;
    }
};

// Overload the "<<" operator.
std::ostream& operator<<(std::ostream& os, const Container& c) noexcept
{
    os << c.value;
    return os;
}

// Overload the "==" operator.
constexpr inline bool operator==(const Container& l, const Container& r) noexcept
{
    return l.value == r.value;
}

static_assert(Container(10) + Container(20) == 30);
static_assert(Container(15)() == 15);
static_assert(Container(10) != Container(20));
static_assert(Container(10) == 10);

void demo()
{
    Container container(10);
    container += Container(5);
    assert(container == 15);
    container -= Container(1);
    assert(container == 14);
    container %= Container(5);
    assert(container == 4);
}
}

namespace space_ship_operator {
// Demo of the spaceship ( <=> ) operator in C++20.
struct Version
{
    unsigned short major{};
    unsigned short minor{};

    // Setting the spaceship operator to default causes the compiler to generate
    // all comparison operators, like < <= == >= > != .
    // The compiler considers all fields, in this case major and minor.
    // If the first field is smaller than or greater than, the comparison is complete.
    // If the first fields are the same,
    // then it considers the second field, and so on,
    // till it completes the comparison.
    constexpr auto operator<=>(const Version&) const noexcept = default;
};

static_assert(Version(1, 1) != Version(1, 2));
static_assert(Version(1, 1) < Version(1, 2));
static_assert(Version(1, 1) <= Version(1, 2));
static_assert(Version(1, 2) > Version(1, 1));
static_assert(Version(1, 2) >= Version(1, 1));
static_assert(Version(1, 1) == Version(1, 1));
}


namespace template_with_default_type {
// Template with a default type.
template <typename T = int>
constexpr auto sum(T a, T b) -> T
{
    return a + b;
}

// Pass the type:
static_assert(sum<int>(1, 1) == 2);
// Omit the type: it takes int.
static_assert(sum(1, 1) == 2);
}


namespace demo_accumulate {
constexpr std::array<int, 3> integers{1, 2, 3};
// Cpp Core Guidelines
// ES.1: Prefer the standard library to other libraries and to handcrafted code.
constexpr auto sum = std::accumulate(integers.cbegin(), integers.cend(), 0);
static_assert(sum == 6);
constexpr auto product = std::accumulate(integers.cbegin(), integers.cend(), 1, std::multiplies<int>());
static_assert(product == 6);

void demo()
{
    const auto strings = std::vector<std::string>{"a", "b"};
    std::string init{"init"};
    const auto concat = std::accumulate(strings.cbegin(), strings.cend(), init);
    assert(concat == "initab");

    auto dash_fold = [](std::string a, int b)
    {
        return std::move(a) + '-' + std::to_string(b);
    };
    std::string s = std::accumulate(std::next(integers.begin()), integers.end(),
                                    std::to_string(integers.at(0)), // start with first element
                                    dash_fold);
    assert(s == "1-2-3");
}
}


namespace demo_adjacent_find {
constexpr auto increasing = {1, 2, 3};
constexpr auto is_less = [](const auto& l, const auto& r) { return l > r; };
constexpr auto iter1 = std::ranges::adjacent_find(increasing, is_less);
static_assert(iter1 == increasing.end());
constexpr auto decreasing = {3, 2, 1};
constexpr auto iter2 = std::ranges::adjacent_find(increasing, is_less);
static_assert(iter2 != increasing.begin());
}


namespace demo_async_and_future {
void demo()
{
    std::thread::id divide_thread_id;
    const auto divide = [&](int a, int b) -> int
    {
        if (!b) throw std::runtime_error("Cannot divide by zero");
        divide_thread_id = std::this_thread::get_id();
        return a / b;
    };
    std::thread::id main_thread_id = std::this_thread::get_id();
    std::future future = std::async(divide, 45, 5); // Call the function in a thread.
    const int result = future.get(); // Wait till the calculation is ready and get the result.
    assert(result == 9);
    assert(main_thread_id != divide_thread_id);
}
}

namespace demo_at_exit {
void demo()
{
    const auto fn = []() -> void
    {
        assert(true); // To see the effect on exit: Set to false.
    };
    std::atexit(fn); // Register "fn" to run at normal program exit.
}
}


namespace demo_variant {
void demo()
{
    // Initialized with the monostate as that is the first variant.
    std::variant<std::monostate, int, float> variant;

    // Getting the wrong variant throws a bad_variant_access exception.
    try
    {
        std::get<int>(variant);
        assert(false);
    }
    catch (const std::bad_variant_access& e)
    {
        assert(true);
    }

    variant = 10;
    assert(std::get<int>(variant) == 10);
    assert(std::get<1>(variant) == 10); // Same as above.
    assert(std::holds_alternative<int>(variant));
}
}


namespace demo_bind {
// Demo of std::bind, including bind_front and bind_back.
// https://cppreference.com/w/cpp/utility/functional/bind.html
void demo()
{
    constexpr auto minus = [](const int a, const int b) -> int
    {
        return a - b;
    };

    struct Foo
    {
        int minus(const int a, const int b) const noexcept
        {
            return a - b;
        }

        int val10 = 10;
    };

    // Struct instance.
    Foo foo;

    // The function template std::bind generates a forwarding call wrapper for f.
    // Calling this wrapper is equivalent to invoking f with some of its arguments bound to args.

    const auto value1_minus_value2 = std::bind(minus, std::placeholders::_1, std::placeholders::_2);
    assert(value1_minus_value2(1, 2) == -1);

    const auto value_minus_3 = std::bind(minus, std::placeholders::_1, 3);
    assert(value_minus_3(1) == -2);

    // Bind to a pointer to a member function.
    const auto value1_minus_value2_via_member_fn = std::bind(&Foo::minus, &foo, std::placeholders::_1,
                                                             std::placeholders::_2);
    assert(value1_minus_value2_via_member_fn(1, 2) == -1);

    // Bind to a pointer to a data member.
    const auto get_value_from_struct = std::bind(&Foo::val10, std::placeholders::_1);
    assert(get_value_from_struct(foo) == 10);

    // Function templates std::bind_front and std::bind_back
    // generate a perfect forwarding call wrapper
    // which allows to invoke the callable target
    // with its first or last sizeof...(Args) parameters bound to args.

    const auto fifty_minus_value = std::bind_front(minus, 50);
    assert(fifty_minus_value(3) == 47); // equivalent to `minus(50, 3)`: 47.

    const auto value_minus_fifty = std::bind_back(minus, 50);
    assert(value_minus_fifty(5) == -45);
}
}


namespace demo_bit_operations {
// https://en.cppreference.com/w/cpp/header/bit
void demo()
{
    {
        // The system's endian-ness.
        constexpr auto endian{std::endian::native};
        static_assert(endian != std::endian::big);
        static_assert(endian == std::endian::little);
    }
    {
        // Demo of std::bit_cast.
        // Reinterpret the object representation of one type as that of another.
        constexpr uint8_t ui8{255};
        constexpr int8_t i8 = std::bit_cast<int8_t>(ui8);
        static_assert(ui8 == 255);
        static_assert(i8 == -1);
    }

    static_assert(std::bitset<4>(2u) == 0b0010);

    static_assert(std::has_single_bit(2u));

    static_assert(std::bitset<4>(2u).to_string() == "0010");

    {
        // rotl - Rotate bits to the left.
        // rotr - Rotate bits to the right.
        constexpr uint8_t ui2 = 0b0010;
        static_assert(std::rotl(ui2, 1) == 0b0100);
        static_assert(std::rotr(ui2, 1) == 0b0001);
    }

    // bit_ceil
    // Finds the smallest integral power of two not less than the given value.
    // bit_floor
    // Finds the largest integral power of two not greater than the given value.
    // bit_width
    // Finds the smallest number of bits needed to represent the given value.
    // countl_zero
    // Counts the number of consecutive 0 bits, starting from the most significant bit.
    // countl_one
    // Counts the number of consecutive 1 bits, starting from the most significant bit.
    // countr_zero
    // Counts the number of consecutive 0 bits, starting from the least significant bit.
    // countr_one
    // Counts the number of consecutive 1 bits, starting from the least significant bit.
    // popcount - Counts the number of 1 bits in an unsigned integer.

    // The std::byteswap reverses the bytes in the given integer value n.
    {
        // const auto dump = [](auto value)
        // {
        //     std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(value) * 2) << value <<
        //         " : ";
        //     for (std::size_t i{}; i != sizeof(value); i++, value >>= 8)
        //         std::cout << std::setw(2) << static_cast<unsigned>(decltype(value)(0xFF) & value) << ' ';
        //     std::cout << std::dec << std::endl;
        // };
        //
        // static_assert(std::byteswap('a') == 'a');
        //
        // std::cout << "byteswap for U16:" << std::endl;
        // constexpr auto x = std::uint16_t(0xCAFE);
        // dump(x);
        // dump(std::byteswap(x));
        //
        // std::cout << "\nbyteswap for U32:" << std::endl;
        // constexpr auto y = std::uint32_t(0xDEADBEEFu);
        // dump(y);
        // dump(std::byteswap(y));
        //
        // std::cout << "\nbyteswap for U64:" << std::endl;
        // constexpr auto z = std::uint64_t{0x0123456789ABCDEFull};
        // dump(z);
        // dump(std::byteswap(z));
    }
}
}


namespace stream_manipulation {
void demo()
{
    {
        std::ostringstream oss;
        oss << std::boolalpha << false << std::noboolalpha << true;
        assert(oss.str() == "false1");
    }
}
}


namespace common_mathematical_functions {
void demo()
{
    assert(std::ceil(1.2f) == 2);
}
}


namespace demo_chrono_library {
// The chrono library defines five main types:
// clocks
// time points
// durations
// calendar dates
// time zone information
void demo()
{
    // A clock consists of a starting point, or epoch, and a tick rate.
    // E.g. a clock may have an epoch of 1st January 1970 UTC and tick every second.
    // The system_clock is the real-time wall clock.
    // The steady_clock is monotonic and never gets adjusted
    // The high_resolution_clock is a clock with the shortest possible tick period.

    // A time point is a duration of time that has passed since the epoch of a specific clock.

    // A duration consists of a span of time, defined as some number of ticks of some time unit.
    // For example, "42 seconds" could be represented by a duration consisting of 42 ticks of a 1-second time unit.

    {
        const auto now = std::chrono::system_clock::now();
        const auto time_point = std::chrono::time_point_cast<std::chrono::minutes, std::chrono::system_clock>(now);
        const auto duration_minutes = time_point.time_since_epoch();
        const auto minutes = duration_minutes.count();
        assert(minutes >= 29559654);
        const auto next_minute = time_point + std::chrono::minutes{1};
        assert(next_minute.time_since_epoch().count() == minutes + 1);

        const auto duration_hours = std::chrono::duration_cast<std::chrono::hours>(duration_minutes);
        const auto hours = duration_hours.count();
        assert(hours == minutes/60);
    }

    {
        // Parses a timestamp that approximately matches the RFC 3339 standard.
        // Example of a timestamp in Zulu time:    2023-11-19T14:59:37.420Z
        // Example of a timestamp with a timezone: 2023-11-19T15:59:37.420+01:00

        const std::string timestring{"2023-11-19T15:59:37.420+01:00"};
        std::istringstream iss(timestring);

        // Parsing format for a date/time stamp with specified time zone.
        // The time zone may also be indicated with a "Z" at the end: Zulu time = UTC.
        std::string format{"%Y-%m-%dT%H:%M:%S"};
        format += timestring.ends_with('Z') ? "%Z" : "%z";

        // std::chrono::time_point<std::chrono::system_clock> tp;
        // iss >> std::chrono::parse(format, tp);
    }
}
}

namespace demo_copy {
void demo()
{
    constexpr auto values = std::array<int, 3>{1, 2, 3};
    {
        std::vector<int> copied{};
        std::copy(values.begin(), values.end(), std::back_inserter(copied));
        assert(copied.size() == 3);
    }
    {
        std::vector<int> copied{};
        std::ranges::copy(values, std::back_inserter(copied));
        assert(copied.size() == 3);
    }
}
}

namespace reference_wrappers {
void demo()
{
    const auto fn = [](int& n1, int& n2, const int& n3)
    {
        n1++; // Increases the copy of n1 stored in the function object.
        n2++; // Increases the caller's n2.
        // n3++; Compile error: cannot assign to variable 'n3' with const-qualified type 'const int &'
    };

    int n1{1};
    int n2{2};
    int n3{3};

    std::function<void()> bound_fn = std::bind(fn, n1, std::ref(n2), std::cref(n3));

    bound_fn();

    assert(n1 == 1); // This is left unchanged, because it was passed by value to the function.
    assert(n2 == 3); // This was passed by reference, and got increased by the function.
    assert(n3 == 3); // Passed by const reference, could not get increased.
}
}


namespace timer_with_jthread_and_timed_mutex_and_condition_variable {
void demo()
{
    return;
    std::timed_mutex mx;
    std::condition_variable_any cv; // The _any means: Works with any lock, not just a unique_lock.

    // This lambda function takes a stop token as parameters.
    const auto timer = [&](const std::stop_token& stoken)
    {
        std::cout << "Step 3: Set the timer interval to 100 milliseconds" << std::endl;
        constexpr auto interval = std::chrono::milliseconds(100);
        while (!stoken.stop_requested())
        {
            std::cout << "Step 4: The stop token has no stop request: Keep going" << std::endl;
            std::unique_lock ulk(mx);
            std::cout <<
                "Step 5: Enter the condition variable which will wait 100 ms or less in case of a thread stop request"
                << std::endl;
            if (cv.wait_for(ulk, stoken, interval, [&stoken] { return stoken.stop_requested(); }))
            {
                std::cout <<
                    "Step 8: The condition variable got a stop request and so interrupts its waiting state immediately"
                    << std::endl;
                break;
            }
            std::cout << "Step 6: Run one timer cycle" << std::endl;
        }
        std::cout << "Step 9: The thread function quits and the thread automatically joins" << std::endl;
    };

    std::cout << "Step 1: The main thread starts the timer thread" << std::endl;
    std::jthread thread(timer);
    std::cout << "Step 2: The main thread will sleep for 350 milliseconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    std::cout << "Step 7: The jthread will go out of scope, this sends a stop request to the thread function" <<
        std::endl;
}
}


namespace condition_variables {
void demo()
{
    std::condition_variable cv;
    std::cv_status status;

    const auto timer = [&]()
    {
        std::mutex mx;
        std::unique_lock ulk(mx);
        status = cv.wait_for(ulk, std::chrono::microseconds(100));
    };
    {
        std::jthread thread1(timer);
    }
    // The condition variable ran into a timeout.
    assert(status == std::cv_status::timeout);
    // Or else: no_timeout.

}
}

int main()
{
    algorithms_lower_bound_and_upper_bound::demo();
    min_max_clamp_ranges_minmax::demo();
    latches_and_barriers::demo_latch();
    latches_and_barriers::demo_barrier();
    mutual_exclusion::shared_and_unique_locks();
    timed_mutex_demo::timed_mutex();
    containers_library::demo_contains();
    containers_library::demo_vector_vs_list_allocation();
    containers_library::demo_set_with_hash_key_and_comparator();
    demo_piecewise_construct::demo();
    demo_forward_as_tuple::demo();
    class_design_idioms::raii_resource_acquisition_is_initialization();
    class_design_idioms::curiously_recurring_template_pattern();
    class_design_principles::liskov_substitution_principle::demo();
    language_declarations::declarations();
    language_exceptions::demo();
    lambda_capture_demo::demo();
    assign_two_lambdas_to_same_function_object::demo();
    operator_overloading::demo();
    demo_accumulate::demo();
    demo_async_and_future::demo();
    demo_at_exit::demo();
    demo_variant::demo();
    demo_bind::demo();
    demo_bit_operations::demo();
    stream_manipulation::demo();
    common_mathematical_functions::demo();
    demo_chrono_library::demo();
    demo_copy::demo();
    reference_wrappers::demo();
    timer_with_jthread_and_timed_mutex_and_condition_variable::demo();
    condition_variables::demo();
    return EXIT_SUCCESS;
}

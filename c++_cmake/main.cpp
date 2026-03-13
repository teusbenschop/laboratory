#include <algorithm>
#include <array>
#include <barrier>
#include <cassert>
#include <condition_variable>
#include <cstdlib>
#include <functional>
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
constexpr int constexpr_add (int var1, int var2)
{
    return var1 + var2;
}

// consteval : Force evaluation of a function at compile time.
consteval int consteval_add (int var1, int var2)
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
    static_assert(constexpr_add(1,2) == 3);
    static_assert(constexpr_add(i1,i2) == 20);

    // Variable i3 is assigned at runtime.
    [[maybe_unused]]int i3 = 2;
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);

    // The variable is initialized at runtime although the function is constexpr.
    [[maybe_unused]] int i4 = constexpr_add(i3, i3);
    assert(i4 == 4);

    // OK, evaluated at compile-time.
    static_assert(consteval_add(1,1) == 2);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);
    // Can all be evaluated at compile time.
    static_assert(i1 == 10);
    static_assert(i2 == 10);
    static_assert(constexpr_add(1,2) == 3);
    static_assert(constexpr_add(i1,i2) == 20);

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
    static_assert(if_constexpr_add(1,2) == 3);

    {
        constexpr const auto xdigit = [](int n) -> char {
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

class BaseException : public std::exception {
    const std::string m_what;
public:
    explicit BaseException(std::string_view what = "") noexcept : m_what(what) {}
    const char* what() const noexcept final override { return m_what.c_str(); }
};

struct Type1Exception : BaseException {
    explicit Type1Exception(std::string_view what = "") noexcept : BaseException(what) {}
};

struct Type2Exception : BaseException {
    explicit Type2Exception(std::string_view what = "") noexcept : BaseException(what) {}
};

// This template function gets passed an exception, throws it, and catches it.
// The "catch" clauses are put in the correct order:
// 1. Catch the most derived types.
// 2. Catch the base exception.
// 3. Catch the standard exception.
template <typename Exception>
void demo_exception_catch_hierarchy(const Exception& e)
{
    try {
        throw e;
    }
    catch (const Type1Exception& exception) {
        std::cout << "Catch Type1Exception" << std::endl;
    }
    catch (const Type2Exception& exception) {
        std::cout << "Catch Type2Exception" << std::endl;
    }
    catch (const BaseException& exception) {
        std::cout << "Catch BaseException" << std::endl;
    }
    catch (const std::exception& exception) {
        std::cout << "Catch std::exception" << std::endl;
    }
    catch (...) {
        std::cout << "Catch unknown exception" << std::endl;
    }
}

void demo()
{
  // One exception handling function to be used in several places.
  // It uses dynamic casting to handle the different exceptions.
  const auto exception_handler = [] (const std::exception* exception)
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
  try {
    std::cout << "Throw Type1Exception" << std::endl;
    throw Type1Exception();
  } catch (const std::exception& e) {
    exception_handler(&e);
  }
  try {
    std::cout << "Throw Type2Exception" << std::endl;
    throw Type2Exception();
  } catch (const std::exception& e) {
    exception_handler(&e);
  }
  try {
    std::cout << "Throw BaseException" << std::endl;
    throw BaseException();
  } catch (const std::exception& e) {
    exception_handler(&e);
  }
  try {
    std::cout << "Throw standard exception" << std::endl;
    throw std::runtime_error("standard");
  } catch (const std::exception& e) {
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
    // Capture by value.
    {
        auto v = 7;
        // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
        // Mark it mutable because it mutates the captured v.
        // Note that it captures v once, and stores it in the lambda function.
        // Hence, the increased v is kept and can be increased once more and so on.
        auto lambda = [v] mutable {
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
        auto lambda = [&v]() {
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
}

namespace stateless_lambda_function {
// A stateless lambda (function) does not retain any data or memory
// from one execution to the next.
const auto stateless = [] {};
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

struct Container {
    constexpr Container(const int value) : value(value) { }
    int value;

    // Overload the "+" operator.
    constexpr Container operator+(const Container &other)
    {
        return Container(value + other.value);
    }

    // Overload the "()" operator.
    constexpr int operator()() { return value; }

    // Overload the += and the -= and the %= operators.
    constexpr Container& operator+=(const Container& c) noexcept {
        value += c.value; return *this;
    }
    constexpr Container& operator-=(const Container& c) noexcept {
        value -= c.value; return *this;
    }
    constexpr Container& operator%=(const Container& c) noexcept {
        value = value % c.value; return *this;
    }
};

// Overload the "<<" operator.
std::ostream& operator<<(std::ostream& os, const Container& c) noexcept
{
    os << c.value;
    return os;
}

// Overload the "==" operator.
constexpr inline bool operator==(const Container& l, const Container& r) noexcept {
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
struct Version {
    unsigned short major {};
    unsigned short minor {};

    // Setting the spaceship operator to default causes the compiler to generate
    // all comparison operators, like < <= == >= > != .
    // The compiler considers all fields, in this case major and minor.
    // If the first field is smaller than or greater than, the comparison is complete.
    // If the first fields are the same,
    // then it considers the second field, and so on,
    // till it completes the comparison.
    constexpr auto operator<=>(const Version&) const noexcept = default;
};

static_assert(Version(1,1) != Version(1,2));
static_assert(Version(1,1) <  Version(1,2));
static_assert(Version(1,1) <= Version(1,2));
static_assert(Version(1,2) >  Version(1,1));
static_assert(Version(1,2) >= Version(1,1));
static_assert(Version(1,1) == Version(1,1));

}

int main()
{
    algorithms_lower_bound_and_upper_bound::demo();
    min_max_clamp_ranges_minmax::demo();
    // latches_and_barriers::demo_latch();
    // latches_and_barriers::demo_barrier();
    // mutual_exclusion::shared_and_unique_locks();
    // timed_mutex_demo::timed_mutex();
    containers_library::demo_contains();
    containers_library::demo_vector_vs_list_allocation();
    containers_library::demo_set_with_hash_key_and_comparator();
    demo_piecewise_construct::demo();
    demo_forward_as_tuple::demo();
    class_design_idioms::raii_resource_acquisition_is_initialization();
    class_design_idioms::curiously_recurring_template_pattern();
    class_design_principles::liskov_substitution_principle::demo();
    language_declarations::declarations();
    //language_exceptions::demo();
    //lambda_capture_demo::demo();
    //assign_two_lambdas_to_same_function_object::demo();
    operator_overloading::demo();
    return EXIT_SUCCESS;
}

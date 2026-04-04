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
#include <compare>
#include <complex>
#include <condition_variable>
#include <coroutine>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <execution>
#include <expected>
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
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <syncstream>
#include <thread>
#include <unordered_set>
#include <vector>

#include "scoped_timer.h"
#include "binary_search.h"
#include "bounds_limits.h"
#include "any_of_all_of_none_of.h"
#include "counting.h"
#include "latches_barriers.h"
#include "constraints.h"



/// Todo




namespace shared_and_unique_locks {
// This example code features a queue.
// Some code writes to it, and some code reads from it.
// To synchronize the read and write operations, the following locks are used:
// 1. A shared lock to enable one or more processes to read from the queue simultaneously.
// 2. A unique lock so only one bit of code can write to the queue.

std::queue<int> the_queue;
std::shared_mutex the_shared_mutex;
std::condition_variable_any the_cv;

void print_queue(const std::stop_token& stoken)
{
    while (!stoken.stop_requested())
    {
        // Wait for something to be broadcast or for a stop request.
        // This uses a unique lock because it might modify the queue.
        std::unique_lock lock(the_shared_mutex);
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
    std::shared_lock lock(the_shared_mutex);
    return the_queue.empty();
}

void demo()
{
    return;

    std::jthread printer(print_queue);

    for (int i{1}; i <= 5; i++)
    {
        // This uses a unique lock because it modifies the queue.
        std::cout << "Attempt to obtain a unique lock" << std::endl;
        std::unique_lock lock(the_shared_mutex);
        std::cout << "The unique lock was obtained" << std::endl;
        std::cout << "Push " << i << " onto queue" << std::endl;
        the_queue.push(i);
    }

    // Wait till the queue is empty.
    while (!queue_empty())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
}


namespace timed_mutex {
// If a normal mutex cannot be obtained, this leads to a deadlock.
// A timed mutex will assist in such a case.
// If a lock is requested on a timed mutex, a timeout can be passed too.
// If the lock cannot be obtained in time, it falls in a timeout, not in a deadlock.

std::timed_mutex the_timed_mutex;

void demo()
{
    return;
    std::cout << "Obtain first lock on the timed mutex" << std::endl;
    const std::unique_lock lock1(the_timed_mutex, std::chrono::seconds(1));
    std::cout << "Obtained first lock" << std::endl;
    std::cout << "Obtain second lock on the same timed mutex" << std::endl;
    const std::unique_lock lock2(the_timed_mutex, std::chrono::milliseconds(10));
    if (lock2)
        std::cout << "Obtained second lock" << std::endl;
    else
        std::cout << "Failed to obtain second lock within 10 milliseconds" << std::endl;
}
}

namespace containers_library {
void demo_contains()
{
    constexpr auto word{"word"};
    std::multiset<std::string> bag;
    bag.insert(word);
    assert(bag.contains(word));
    assert(not bag.contains(""));
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
    vector.resize(100);
    list.resize(100);
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

    // An unordered set, with hash function to generate the key, and an equal operator.
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

void demo_forward_list()
{
    std::forward_list<int> list = {3, 1, 2, 3, 3};
    // Singly linked list.
    // Supports fast insertion and removal anywhere.
    list.sort();
    list.unique();
    const auto standard = std::forward_list<int>{1, 2, 3};
    assert(list == standard);
}
}


namespace piecewise_construct {

enum Construction { none, from_tuple, from_int_float };

struct Foo
{
    Construction construction {none};

    // Constructor from a tuple.
    constexpr explicit Foo(std::tuple<int, float>)
    {
        construction = from_tuple;
    }

    // Constructor from an int and a float.
    constexpr explicit Foo(int, float)
    {
        construction = from_int_float;
    }
};


void demo()
{
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


namespace forward_as_tuple {
// The helper "forward_as_tuple" takes a variadic pack of arguments
// and creates a tuple out of them.
// It determines the types of the elements of the arguments:
// if it receives an lvalue then it will have an lvalue reference,
// and if it receives an rvalue then it will have an rvalue reference.
void demo()
{
    // Function returns an rvalue.
    const auto rvalue = [] -> std::string
    {
        return "universe";
    };

    // The variable i is a lvalue.
    int lvalue = 42;

    auto tuple = std::forward_as_tuple(lvalue, rvalue());

    // An lvalue reference binds to an lvalue. Marked with one ampersand (&).
    // An rvalue reference binds to an rvalue. Marked with two ampersands (&&).
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
// Existing code: Class save to database.
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
    int i3 = 2;
    // Compile error: the value of 'i3' is not usable in a constant expression
    // static_assert(constexpr_add(i3,i3) == 4);
    assert(i3 == 2);

    // The variable is initialized at runtime although the function is constexpr.
    int i4 = constexpr_add(i3, i3);
    assert(i4 == 4);

    // OK, evaluated at compile-time.
    static_assert(consteval_add(1, 1) == 2);

    // Cannot be evaluated at compile-time.
    // Compile error:
    // call to consteval function 'consteval_add(i3, i3)' is not a constant expression
    // int i5 = consteval_add(i3, i3);

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
            constexpr const char digits[] = "0123456789";
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
    const auto dynamic_exception_handler = [](const std::exception* exception)
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
        dynamic_exception_handler(&e);
    }
    try
    {
        std::cout << "Throw Type2Exception" << std::endl;
        throw Type2Exception();
    }
    catch (const std::exception& e)
    {
        dynamic_exception_handler(&e);
    }
    try
    {
        std::cout << "Throw BaseException" << std::endl;
        throw BaseException();
    }
    catch (const std::exception& e)
    {
        dynamic_exception_handler(&e);
    }
    try
    {
        std::cout << "Throw standard exception" << std::endl;
        throw std::runtime_error("standard");
    }
    catch (const std::exception& e)
    {
        dynamic_exception_handler(&e);
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

namespace lambda_capture {
void demo()
{
    // Capture by value.
    {
        int captured_v_by_value;
        auto v = 7;
        // Note that the following lambda, via the capture, copies the current v = 7 into the lambda.
        // Mark it mutable because it mutates the captured v.
        // Note that it captures v once, and stores it in the lambda function.
        // Hence, the increased v is kept and can be increased once more and so on.
        auto lambda = [v](int& captured_v_by_value) mutable
        {
            v++;
            captured_v_by_value = v;
        };
        lambda(captured_v_by_value);
        assert(captured_v_by_value == v + 1);
        lambda(captured_v_by_value);
        assert(captured_v_by_value == v + 2);
        // The original v remains unchanged.
        assert(v == 7);
    }

    // Capture by reference.
    {
        auto v = 7;
        auto lambda = [&v]
        {
            ++v;
        };
        lambda();
        assert(v == 8);
        lambda();
        assert(v == 9);
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
// A stateless lambda function does not retain any data or memory
// from one execution to the next.
constexpr auto stateless1 = []
{
};
// It is assignable.
constexpr auto stateless2 = stateless1;
// Default-constructible (i.e. constructor without parameters, or with default parameters).
static_assert(std::is_default_constructible_v<decltype(stateless1)>); // passes
constexpr decltype(stateless1) stateless3;
static_assert(std::is_same_v<decltype(stateless1), decltype(stateless2)>); // passes
static_assert(std::is_same_v<decltype(stateless1), decltype(stateless3)>); // passes
static_assert(std::is_same_v<decltype(stateless2), decltype(stateless3)>); // passes
}


namespace keyword_auto_for_lambdas {
// Can use "typename T" or "auto" for lambda functions.
constexpr auto lambda_typename = []<typename T>(T value) -> T {
    T val2 = value;
    decltype(value) val3 = value;
    return value + 1;
};
constexpr auto lambda_auto = [](auto v) -> auto { return v + 1; };
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

constexpr double foo{-0.0f};
constexpr double bar{+0.0f};
constexpr std::partial_ordering result{foo <=> bar};
static_assert(result != std::partial_ordering::less);
static_assert(result != std::partial_ordering::greater);
static_assert(result == std::partial_ordering::equivalent);
static_assert(result != std::partial_ordering::unordered);

}


namespace template_with_default_type {

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
constexpr auto sum = std::accumulate(integers.cbegin(), integers.cend(), 0);
static_assert(sum == 6);
constexpr auto product = std::accumulate(integers.cbegin(), integers.cend(), 1, std::multiplies<int>());
static_assert(product == 6);

void demo()
{
    const auto strings = std::vector<std::string>{"a", "b"};
    const std::string init{"init"};
    const auto concat = std::accumulate(strings.cbegin(), strings.cend(), init);
    assert(concat == "initab");

    const auto dash_fold = [](std::string a, int b)
    {
        return std::move(a) + '-' + std::to_string(b);
    };
    std::string s = std::accumulate(std::next(integers.begin()), integers.end(),
                                    std::to_string(integers.at(0)), // start with first element
                                    dash_fold);
    assert(s == "1-2-3");
}
}


namespace adjacent_find {
constexpr auto increasing = {1, 2, 3};
constexpr auto is_less = [](const auto& l, const auto& r) { return l > r; };
constexpr auto iter1 = std::ranges::adjacent_find(increasing, is_less);
static_assert(iter1 == increasing.end());
constexpr auto decreasing = {3, 2, 1};
constexpr auto iter2 = std::ranges::adjacent_find(decreasing, is_less);
static_assert(iter2 == decreasing.begin());
}


namespace async_and_future {
void demo()
{
    std::thread::id divide_thread_id;
    const auto divide = [&](int a, int b) -> int
    {
        if (!b) throw std::runtime_error("Cannot divide by zero");
        divide_thread_id = std::this_thread::get_id();
        return a / b;
    };
    {
        std::thread::id main_thread_id = std::this_thread::get_id();
        std::future future = std::async(divide, 45, 5); // Call the function in a thread.
        const int result = future.get(); // Wait till the calculation is ready and get the result.
        assert(result == 9);
        assert(main_thread_id != divide_thread_id);
    }
    {
        // Default launch policy (launch policy can be omitted).
        std::future future = std::async(std::launch::async | std::launch::deferred, divide, 1, 2);
        // std::launch::async: Run as soon as possible.
        // std::launch::deferred: Wait till result is requested, then run.
    }
}
}

namespace at_exit {
void demo()
{
    const auto fn = []() -> void
    {
        assert(true); // To see the effect on exit: Set to false.
    };
    std::atexit(fn); // Register "fn" to run at normal program exit.
}
}


namespace variant {
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


namespace bind_and_bind_front_and_bind_back {
// Demo of std::bind, including bind_front and bind_back.
// https://cppreference.com/w/cpp/utility/functional/bind.html
void demo()
{
    constexpr auto minus = [](int a, int b) -> int
    {
        return a - b;
    };

    struct Foo
    {
        constexpr int minus(int a, int b) const noexcept
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


namespace chrono_library {
// https://en.cppreference.com/w/cpp/chrono
// The chrono library defines five main types:
// clocks
// time points
// durations
// calendar dates
// time zone information
void demo()
{
    // A clock consists of a starting point (epoch) and a tick rate.
    // E.g. a clock may have an epoch of 1st January 1970 UTC and tick every second.
    // The system_clock is the real-time wall clock.
    // The steady_clock is monotonic and never gets adjusted.
    // The high_resolution_clock is a clock with the shortest possible tick period.

    // A time point is a duration of time that has passed since the epoch of a specific clock.

    // A duration consists of a span of time, defined as some number of ticks of some time unit.
    // For example, "42 seconds" could be represented by a duration consisting of 42 ticks of a 1-second time unit.

    {
        const auto now = std::chrono::system_clock::now();
        const auto time_point = std::chrono::time_point_cast<std::chrono::minutes, std::chrono::system_clock>(now);
        const auto duration_minutes = time_point.time_since_epoch();
        const long minutes = duration_minutes.count();
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

    {
        constexpr auto year_month_day {std::chrono::year(2026)/3/23};
        static_assert(year_month_day.year() == std::chrono::year(2026));
        static_assert(year_month_day.month() == std::chrono::month(3));
        static_assert(year_month_day.day() == std::chrono::day(23));
    }

    {
        const auto utc = std::chrono::system_clock::now();
        //std::cout << "UTC time: " << utc << std::endl;
        const std::time_t time = std::chrono::system_clock::to_time_t(utc);
        std::string time_str = std::ctime(&time);
        time_str.pop_back(); // Remove the \n.
        //std::cout << "Local time: " << time_str << std::endl;
        //std::cout << std::chrono::current_zone()->to_local(utc) << std::endl;
    }
}
}

namespace copy {
void demo()
{
    const auto values = std::array<int, 3>{1, 2, 3};
    {
        std::vector<int> copy{};
        std::copy(values.begin(), values.end(), std::back_inserter(copy));
        assert(copy.size() == 3);
    }
    {
        std::vector<int> copy{};
        std::ranges::copy(values, std::back_inserter(copy));
        assert(copy.size() == 3);
    }
}
}


namespace reference_wrappers {
void demo()
{
    const auto fn = [](int& n1, int& n2, [[maybe_unused]] const int& n3)
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

namespace meta_programming_automatic_unit_conversion {
// Template programming is about letting the compiler deal with the types.
// Template metaprogramming is about letting the compiler calculate values.

// These are strong types for weights.
// The class automatically converts to the desired unit when it gets passed to a function.
// For example if a function expects a weight in grams,
// and the code passes a weight in kilograms,
// then the compiler automatically converts the passed unit to the expected unit.

// The factors to convert the given weight type to grams.
struct grams
{
    static constexpr float ms_factor = 1.0f;
};

struct kilograms
{
    static constexpr float ms_factor = 1000.0f;
};

// This concept can be used to assure that a type is a weight type.
template <typename T>
concept weight_unit = std::is_same_v<T, grams> or std::is_same_v<T, kilograms>;

template <weight_unit U>
class Weight
{
    float m_value{};

public:
    constexpr Weight() noexcept = default;
    // Constructor taking a float, so the weight is equal to the float passed.
    constexpr explicit Weight(const decltype(m_value) v) noexcept : m_value(v)
    {
    }

    template <weight_unit UU>
    // This constructor is a template, the compiler generates multiple constructors,
    // 1. Create a kilogram from a gram.
    // 2. Create a kilogram from a kilogram.
    // 3. Create a gram from a kilogram.
    // 4. Create a gram from a gram.
    constexpr Weight(const Weight<UU>& s) noexcept
    {
        if constexpr (std::is_same_v<U, UU>)
            m_value = s.value();
        else
            m_value = s.value() * UU::ms_factor / U::ms_factor;
    }

    [[nodiscard]] constexpr decltype(m_value) value() const noexcept { return m_value; }
    explicit constexpr operator decltype(m_value)() const noexcept { return m_value; } // Support static cast.
    constexpr void value(const decltype(m_value) v) noexcept { m_value = v; }
    auto operator<=>(const Weight<U>&) const noexcept = default;

    // Allow for basic arithmetic operations:
    constexpr Weight<U>& operator+=(const Weight<U>& s) noexcept
    {
        m_value += s.value();
        return *this;
    }

    constexpr Weight<U>& operator-=(const Weight<U>& s) noexcept
    {
        m_value -= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const Weight<U>& s) noexcept
    {
        m_value *= s.value();
        return *this;
    }

    constexpr Weight<U>& operator*=(const decltype(m_value) v) noexcept
    {
        m_value *= v;
        return *this;
    }

    constexpr Weight<U>& operator/=(const Weight<U>& s) noexcept
    {
        m_value /= s.value();
        return *this;
    }

    constexpr Weight<U>& operator/=(const decltype(m_value) v) noexcept
    {
        m_value /= v;
        return *this;
    }
};

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator+(Weight<UL> l, const Weight<UR> r) noexcept { return l += r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator-(Weight<UL> l, const Weight<UR> r) noexcept { return l -= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator*(Weight<UL> l, const Weight<UR> r) noexcept { return l *= r; }

template <weight_unit U>
constexpr Weight<U> operator*(Weight<U> l, const float r) noexcept { return l *= r; }

template <weight_unit UL, weight_unit UR>
constexpr Weight<UL> operator/(Weight<UL> l, const Weight<UR> r) noexcept { return l /= r; }

template <weight_unit U>
constexpr Weight<U> operator/(Weight<U> l, const float r) noexcept { return l /= r; }

template <weight_unit U>
inline std::ostream& operator<<(std::ostream& os, const Weight<U>& s) noexcept
{
    os << s.value() << " ";
    if constexpr (std::is_same_v<U, grams>)
        os << "g";
    else if constexpr (std::is_same_v<U, kilograms>)
        os << "kg";
    else
        os << "<unknown weight unit>";
    return os;
}

// Some quality tests.
static_assert(std::is_constructible_v<Weight<grams>, float>);
static_assert(std::is_nothrow_constructible_v<Weight<grams>, float>);

static_assert(std::is_default_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_default_constructible_v<Weight<grams>>);

static_assert(std::is_copy_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_copy_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_copy_constructible_v<Weight<grams>>);

static_assert(std::is_move_constructible_v<Weight<grams>>);
static_assert(std::is_trivially_move_constructible_v<Weight<grams>>);
static_assert(std::is_nothrow_move_constructible_v<Weight<grams>>);


void demo()
{
    constexpr auto weight_100_kg = Weight<kilograms>(100);
    constexpr auto weight_10_g = Weight<grams>(10);

    constexpr Weight<grams> weight_g = weight_100_kg;
    static_assert(weight_g.value() == 100000);

    constexpr Weight<kilograms> weight_kg = weight_10_g;
    static_assert(weight_kg.value() == 0.01f);
}
}


namespace enable_shared_from_this {
// The std::enable_shared_from_this allows an object t
// that is currently managed by a std::shared_ptr pt
// to safely generate additional std::shared_ptr instances pt1, pt2 etc.
// that all share ownership of t with pt.
void demo()
{
    struct Struct : public std::enable_shared_from_this<Struct>
    {
        std::shared_ptr<Struct> getptr()
        {
            return shared_from_this();
        }
    };

    // The original and derived shared pointers share the same object.
    {
        std::shared_ptr<Struct> object1 = std::make_shared<Struct>();
        std::shared_ptr<Struct> object2 = object1->getptr();
        assert(object1 == object2);
        assert(object1.use_count() == 2);
        assert(object2.use_count() == 2);
    }

    // Bad use: shared_from_this is called without having std::shared_ptr owning the caller.
    // It will throw an exception.
    try
    {
        Struct object1;
        std::shared_ptr<Struct> object2 = object1.getptr();
        assert(false); // It should never arrive here.
    }
    catch (std::bad_weak_ptr& e)
    {
        assert(e.what() == std::string("bad_weak_ptr"));
    }
}
}

namespace swapping_and_exchanging {
void demo()
{
    {
        int a = 1, b = 2;
        std::swap(a, b);
        assert(a == 2);
        assert(b == 1);
    }
    {
        int a = 1;
        // Put a new value in a, and return the old value.
        const int b = std::exchange(a, 2);
        assert(a == 2);
        assert(b == 1);
    }
}
}

namespace execution_policies {
void demo()
{
    std::array<int, 5> values = {1, 2, 3, 4, 5};
    [[maybe_unused]] const auto fn = []([[maybe_unused]] const int n)
    {
    };
    // std::for_each(std::execution::par_unseq, values.begin(), values.end(), fn);
    // Clang does not yet support parallel execution.
    std::for_each(values.begin(), values.end(), fn);
}
}


namespace filesystem {
void demo()
{
    {
        std::string url = "/var/log/app";
        std::filesystem::path p(url);
        const auto dirname = p.parent_path().string();
        assert(dirname == "/var/log");
    }

    assert(std::filesystem::path::preferred_separator == '/');

    {
        std::filesystem::path p("log");
        std::filesystem::remove(p);
    }

    {
        std::filesystem::path path("/tmp/hi.txt");
        assert(std::filesystem::exists (path) == false);
    }

    std::filesystem::path path(R"(/tmp)");
    for (const auto& directory_entry : std::filesystem::directory_iterator{path})
    {
        std::filesystem::path path = directory_entry.path();
        assert(!path.empty());
    }
}
}


namespace demo_initializer_list {
// The initializer_list is a light-weight proxy object that gives access to the backing array.
template <class T>
struct S
{
    std::vector<T> v;

    S(std::initializer_list<T> l) : v(l)
    {
    }

    void append(std::initializer_list<T> l)
    {
        v.insert(v.end(), l.begin(), l.end());
    }

    std::pair<const T*, std::size_t> c_arr() const
    {
        return {&v[0], v.size()}; // copy list-initialization in return statement
        // Note this is NOT a use of std::initializer_list
    }
};

template <typename T>
void templated_fn(T)
{
}

void demo()
{
    S<int> s = {1, 2, 3, 4, 5}; // copy list-initialization
    s.append({6, 7, 8}); // list-initialization in function call
    assert(s.c_arr().second == 8); // Assert the backing array size.
    assert(s.v.size() == 8);

    // Range-for over brace-init-list
    for (int x : {-1, -2, -3}) // the rule for auto makes this ranged-for work
        assert(x == -3 or x == -2 or x == -1);

    auto al = {10, 11, 12}; // special rule for auto
    assert(al.size() == 3);

    auto la = al; // a shallow-copy of top-level proxy object
    assert(la.begin() == al.begin()); // guaranteed: backing array is the same

    std::initializer_list<int> il{-3, -2, -1};
    assert(il.begin()[2] == -1); // note the replacement for absent operator[]
    il = al; // shallow-copy
    assert(il.begin() == al.begin()); // guaranteed

    // templated_fn({1, 2, 3}); // compiler error! "{1, 2, 3}" is not an expression,
    // it has no type, and so T cannot be deduced
    templated_fn<std::initializer_list<int>>({1, 2, 3}); // OK
    templated_fn<std::vector<int>>({1, 2, 3}); // also OK
}
}


namespace output_manipulation {
void demo()
{
    {
        // When filling put the value at the left.
        std::stringstream ss;
        ss << std::left << std::setfill('_') << std::setw(10) << -1.23;
        assert(ss.str() == "-1.23_____");
    }
    {
        // When filling up put the value at the right (this is the default normally).
        std::stringstream ss;
        ss << std::right << std::setfill('_') << std::setw(10) << -1.23;
        assert(ss.str() == "_____-1.23");
    }
}
}


namespace inserter {
// The inserter is a convenience function template that constructs a std::insert_iterator
// for the container c and its iterator i with the type deduced from the type of the argument.
void demo()
{
    std::multiset<int> s{1, 2, 3};

    // The std::inserter is commonly used with multi-sets.
    // Add 5 times 2 to the end of the container.
    std::fill_n(std::inserter(s, s.end()), 5, 2);
    {
        std::multiset<int> standard{1, 2, 2, 2, 2, 2, 2, 3};
        assert(s == standard);
    }

    std::vector<int> d{100, 200, 300};
    std::vector<int> v{1, 2, 3, 4, 5};

    // when inserting in a sequence container, insertion point advances
    // because each std::insert_iterator::operator= updates the target iterator
    std::copy(d.begin(), d.end(), std::inserter(v, std::next(v.begin())));
    {
        std::vector<int> standard{1, 100, 200, 300, 2, 3, 4, 5};
        assert(v == standard);
    }
}
}


namespace member_function {
// Function template std::mem_fn generates wrapper objects for pointers to members.
// It can store, copy, and invoke a pointer to member.

struct Foo
{
    std::string display_greeting()
    {
        return "hello";
    }

    int display_number(int i)
    {
        return i;
    }

    int add_xy(int x, int y)
    {
        return data + x + y;
    }

    template <typename... Args>
    int add_many(Args... args)
    {
        return data + (args + ...);
    }

    auto add_them(auto... args) // C++20 required
    {
        return data + (args + ...);
    }

    int data = 7;
};

void demo()
{
    auto f = Foo{};

    constexpr auto greet = std::mem_fn(&Foo::display_greeting);
    assert(greet(f) == "hello");

    const auto print_num = std::mem_fn(&Foo::display_number);
    assert(print_num(f, 42) == 42);

    const auto access_data = std::mem_fn(&Foo::data);
    assert(access_data(f) == 7);

    const auto add_xy = std::mem_fn(&Foo::add_xy);
    assert(add_xy(f, 1, 2) == 10);

    const auto u = std::make_unique<Foo>();
    assert(access_data(u) == 7);
    assert(add_xy(u, 1, 2) == 10);

    const auto add_many = std::mem_fn(&Foo::add_many<short, int, long>);
    assert(add_many(u, 1, 2, 3) == 13);

    const auto add_them = std::mem_fn(&Foo::add_them<short, int, float, double>);
    assert(add_them(u, 5, 7, 10.0f, 13.0) == 42);
}
}


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


namespace lock_multiple_simultaneously {
// This demonstrates how to use std::lock to lock multiple locks at once simultaneously.
// This avoids the risk of having deadlocks in the transfer function.
void demo()
{
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
    assert (account1.m_balance == 80);
    assert (account2.m_balance == 50);
}
}


namespace future_and_promise_and_exception {
// It is possible to set an exception in a promise.
void demo()
{
    return; // Fails on macOS Tahoe
    const auto divide = [] (int a, int b, std::promise<int>& promise) {
        try {
            const auto result = a / b;
            promise.set_value(result);
        } catch(...) {
            try {
                // Store anything thrown in the promise
                promise.set_exception(std::current_exception());
                // or throw a custom exception instead.
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
}


namespace osyncstream {
// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.
void demo()
{
    const auto stream_worker = []([[maybe_unused]] int id) {
        using namespace std::literals::chrono_literals;
        for (int i = 0; i < 2; i++) {
            std::this_thread::sleep_for(1ms);
            // std::osyncstream synced_out(std::cout);
            // synced_out << "worker " << id << std::endl;
        }
    };

    std::jthread threads [4];
    for (int i = 0; i < 4; ++i) {
        threads[i] = std::jthread(stream_worker, i);
    }

}
}

namespace jthread {
// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be cancelled/stopped in certain situations.
void demo()
{
    {
        const auto worker = [] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //std::cout << "Within thread with ID " << std::this_thread::get_id() << std::endl;
        };
        auto jthread = std::jthread{worker};
        // The jthread will join automatically when it goes out of scope.
    }

    {
        using namespace std::literals::chrono_literals;
        std::jthread stoppable_thread { [] (std::stop_token stoptoken) {
            while (!stoptoken.stop_requested()) {
                std::this_thread::sleep_for(1ms);
            }
        }};
        std::this_thread::sleep_for(25ms);
        stoppable_thread.request_stop();
        //  stoppable_thread.join(); // Not needed here.
    }

    {
        // Runs badly on macOS Tahoe: Commented out.
        // using namespace std::literals::chrono_literals;
        //
        // std::mutex mutex{};
        // std::jthread threads [4];
        //
        // const auto print = [](const std::stop_source &source)
        // {
        //     std::cout << std::boolalpha
        //     << "stop_source stop_possible = " << source.stop_possible() << std::endl
        //     << "stop_requested = " << source.stop_requested() << std::endl;
        // };
        //
        // // Common stop source.
        // std::stop_source stop_source;
        // assert(stop_source.stop_requested() == false);
        // assert(stop_source.stop_possible() == true);
        // // print(stop_source);
        //
        // const auto joinable_thread_worker = [&mutex] (const int id, std::stop_source stop_source)
        // {
        //     using namespace std::literals::chrono_literals;
        //     std::stop_token stoken = stop_source.get_token();
        //     while (true) {
        //         std::this_thread::sleep_for(3ms);
        //         std::lock_guard lock (mutex);
        //         if (stoken.stop_requested()) {
        //             std::cout << "worker " << id << " is requested to stop" << std::endl;
        //             return;
        //         }
        //         std::cout << "worker " << id << " goes back to sleep" << std::endl;
        //     }
        // };
        //
        // // Create worker threads.
        // for (int i = 0; i < 4; ++i) {
        //     threads[i] = std::jthread(joinable_thread_worker, i+1, stop_source);
        // }
        //
        // std::this_thread::sleep_for(7ms);
        //
        // std::cout << "request stop" << std::endl;
        // stop_source.request_stop();
        //
        // print(stop_source);
        // // Note: destructor of jthreads will call join so no need for explicit calls
    }
}
}


namespace semaphores {
void demo()
{
    return;
    // The counting_semaphore contains an internal counter initialized by the constructor.
    // This counter is decremented by calls to acquire() and related methods,
    // and is incremented by calls to release().
    // When the counter is zero, acquire() blocks until the counter is incremented.
    // The binary_semaphore is a counting_semaphore with max count = 1.
    std::binary_semaphore signal_main_to_thread_semaphore{0};
    std::binary_semaphore signal_thread_to_main_semaphore{0};

    const auto thread_processor = [&]() {
        // Wait for a signal from the main process by attempting to acquire (decrement) the semaphore.
        // This call blocks until the semaphore is released (its count is increased) from the main process.
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

    // Signal the worker thread to start working by releasing the semaphore (increasing its count).
    std::cout << "Main sends the signal" << std::endl;
    signal_main_to_thread_semaphore.release();

    // Wait until the worker thread is done doing the work
    // by attempting to decrement the semaphore's count.
    signal_thread_to_main_semaphore.acquire();

    std::cout << "Main got the signal" << std::endl;
}
}


namespace demo_packaged_task {
// A std::packaged_task wraps any Callable target so that it can be invoked asynchronously.
// Its return value or exception thrown can be accessed through std::future objects.
void demo()
{
    // No need to pass a promise reference here.
    [[maybe_unused]] auto task_divide = [](int a, int b) {
        if (!b)
            throw std::runtime_error{"Divide by zero exception"};
        return a / b;
    };

    // std::packaged_task<decltype(task_divide)> task(task_divide);
    // auto future = task.get_future();
    // std::thread thread(std::move(task), 45, 5);
    // try {
    //     auto result = future.get();
    //     std::cout << result << std::endl;
    // } catch (const std::exception& exception) {
    //     std::cout << exception.what() << std::endl;
    // }
    // thread.join();
}
}


namespace attribute_no_unique_address {
// https://en.cppreference.com/w/cpp/language/attributes/no_unique_address

struct Empty {}; // The size of any object of empty class type is at least 1.
static_assert(sizeof(Empty) == 1);

struct X
{
    int i;
    Empty e; // At least one more byte is needed to give ‘e’ a unique address
};
static_assert(sizeof(X) == 8);

// struct Y
// {
//     int i;
//     [[no_unique_address]] Empty e; // Empty member optimized out.
// };
// static_assert(sizeof(Y) == 4);

// struct Z
// {
//     char c;
//     // e1 and e2 cannot share the same address because they have the
//     // same type, even though they are marked with [[no_unique_address]].
//     // However, either may share address with ‘c’.
//     [[no_unique_address]] Empty e1, e2;
// };
// static_assert(sizeof(Z) == 2);

// struct W
// {
//     char c[2];
//     // e1 and e2 cannot have the same address, but one of
//     // them can share with c[0] and the other with c[1]:
//     [[no_unique_address]] Empty e1, e2;
// };
// static_assert(sizeof(W) == 3);

void demo()
{

}
}


namespace attribute_likely_unlikely {
// https://en.cppreference.com/w/cpp/language/attributes/likely
// Attribute to hint the compiler for the likely or unlikely path of execution,
// allowing the compiler to optimize the code.
constexpr double power(double x, long long n) noexcept
{
    if (n > 0) [[likely]]
      return x * pow(x, n - 1);
    else [[unlikely]]
      return 1;
}

constexpr long long factorial(long long n) noexcept
{
    if (n > 1) [[likely]]
      return n * factorial(n - 1);
    else [[unlikely]]
      return 1;
}

constexpr double cosine(double x) noexcept
{
    constexpr long long precision{16LL};
    double y{};
    for (auto n{0LL}; n < precision; n += 2LL) [[likely]]
      y += pow(x, n) / (n & 2LL ? -factorial(n) : factorial(n));
    return y;
}

}

namespace attribute_assume {
// Specifies that the given expression is assumed to always evaluate to true
// to allow compiler optimizations based on the information given.
// Since assumptions cause runtime-undefined behavior if they do not hold,
// they should be used sparingly.
// https://en.cppreference.com/w/cpp/language/attributes/assume
// One correct way to use them is to follow assertions with assumptions.

[[maybe_unused]] auto f = [] (auto x) {
    // Compiler may assume x is positive.
    assert(x > 0);
    [[assume(x > 0)]];
};

}

namespace designated_initializers {
// https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers

struct S1 {
    const int x{};
    const int y{};
    const int z{};
};
// Fails because designator order does not match declaration order.
//constexpr S1 s1 {.y = 2, .x = 1};
// Ok, s2.y initialized to 0.
constexpr S1 s2 {.x = 1, .z = 2};
static_assert(s2.x == 1);
static_assert(s2.y == 0);
static_assert(s2.z == 2);

struct S2 {
    const int o {33};
    const int n {42};
    const int m {-1};
};
constexpr S2 s3 {.m = 21};
// Initializes o with 33.
// Then initializes n with = 42.
// Then initializes m with = 21.
static_assert(s3.o == 33);
static_assert(s3.n == 42);
static_assert(s3.m == 21);

void demo()
{
}
}


namespace aggregate_initialization {
// https://en.cppreference.com/w/cpp/language/aggregate_initialization

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

// Using direct-list-initialization syntax.
constexpr S s1 =
{
    1,
    {
        2, 3,
        { 4, 5, 6 },
    }
};
static_assert(s1.x == 1);
static_assert(s1.foo.i == 2);
static_assert(s1.foo.j == 3);
static_assert(s1.foo.a[0] == 4);
static_assert(s1.foo.a[1] == 5);
static_assert(s1.foo.a[2] == 6);

// Same, but with brace elision.
// The compiler suggest braces around subobject initialization, rightly so.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr S s2 = {1, 2, 3, 4, 5, 6};
#pragma GCC diagnostic pop
static_assert(s2.x == 1);
static_assert(s2.foo.i == 2);
static_assert(s2.foo.j == 3);
static_assert(s2.foo.a[0] == 4);
static_assert(s2.foo.a[1] == 5);
static_assert(s2.foo.a[2] == 6);

// Brace elision only allowed with equals sign
constexpr int ar[] = {1, 2, 3}; // ar is int[3]
static_assert(ar[0] == 1);
static_assert(ar[1] == 2);
static_assert(ar[2] == 3);

// Too many initializer clauses
// char cr[3] = {'a', 'b', 'c', 'd'};

// Array initialized as {'a', '\0', '\0'}
constexpr char cr[3] = {'a'};
static_assert(cr[0] == 'a');
static_assert(cr[1] == '\0');
static_assert(cr[2] == '\0');

// Fully-braced 2D array: {1, 2}
constexpr int ar2d1[2][2] = {{1, 2}, {3, 4}};
static_assert(ar2d1[0][0] == 1);
static_assert(ar2d1[0][1] == 2);
static_assert(ar2d1[1][0] == 3);
static_assert(ar2d1[1][1] == 4);

// Compiler suggests braces around subobject.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr int ar2d2[2][2] = {3, 4};
#pragma GCC diagnostic pop
static_assert(ar2d2[0][0] == 3);
static_assert(ar2d2[0][1] == 4);
static_assert(ar2d2[1][0] == 0);
static_assert(ar2d2[1][1] == 0);

// Brace elision.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
constexpr int ar2d3[2][2] = {1, 2, 3, 4}; // brace elision: {1, 2}
#pragma GCC diagnostic pop
static_assert(ar2d3[0][0] == 1);
static_assert(ar2d3[0][1] == 2);
static_assert(ar2d3[1][0] == 3);
static_assert(ar2d3[1][1] == 4);

// Only first column: {1, 0}
constexpr int ar2d4[2][2] = {{1}, {2}};
static_assert(ar2d4[0][0] == 1);
static_assert(ar2d4[0][1] == 0);
static_assert(ar2d4[1][0] == 2);
static_assert(ar2d4[1][1] == 0);

constexpr std::array<int, 3> std_ar2{{1, 2, 3}};  // std::array is an aggregate
static_assert(std_ar2[0] == 1);
static_assert(std_ar2[1] == 2);
static_assert(std_ar2[2] == 3);

constexpr std::array<int, 3> std_ar1 = {1, 2, 3}; // brace-elision okay
static_assert(std_ar1[0] == 1);
static_assert(std_ar1[1] == 2);
static_assert(std_ar1[2] == 3);

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
constexpr U u1 = {1};   // OK, first member of the union
static_assert(u1.a == 1);
// U u2 = {0, "asdf"}; // error: too many initializers for union
// U u3 = {"asdf"};    // error: invalid conversion to int

void demo()
{

}
}


namespace coroutines {
// https://en.cppreference.com/w/cpp/language/coroutines
// Coroutines are designed to be performing as lightweight threads.
// Coroutines provide concurrency but not parallelism.
// Switching between coroutines need not involve any system/blocking calls
// so no need for synchronization primitives such as mutexes, semaphores.
// A coroutine does not have a stack, it stores its variables on the heap.
// A coroutine suspends execution by returning to the caller.
// The data needed to resume operations is on the heap.
// A function is a coroutine if its definition contains any of the following:
// * co_await: suspend execution until resumed.
// * co_yield: suspend execution returning a value.
// * co_return: complete execution returning a value.

struct return_object {
  struct promise_type {
    return_object get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void unhandled_exception() {}
  };
};

struct awaiter {
  std::coroutine_handle<> *m_handle;
  constexpr bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> handle) { *m_handle = handle; }
  constexpr void await_resume() const noexcept {}
};

// This function runs forever. It increases and prints the value.
// The variable i maintains its value even as control switches repeatedly
// between this function and the function that invoked it.
return_object infinite_counter(std::coroutine_handle<>* continuation) {
    awaiter a{continuation};
    int i {0};
    while (true)
    {
        // Suspend the coroutine and returns control to the caller.
        co_await a;
        std::cout << "in coroutine " << ++i << std::endl;
    }
}

void demo()
{
    return;
    std::coroutine_handle<> handle;
    infinite_counter(&handle);
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "in main function" << std::endl;
        handle();
    }
    handle.destroy();
}
}


namespace modules {
// https://en.cppreference.com/w/cpp/language/modules
// Modules are a language feature to share declarations and definitions across translation units.
// They are an alternative to some use cases of headers.
// Modules are orthogonal to namespaces.
// export module hello_world;
// C++20 'module' only available with '-fmodules-ts', which is not yet enabled with '-std=c++20'
void demo()
{
}
}

namespace header_compare {
// https://en.cppreference.com/w/cpp/header/compare
void demo()
{

}
}


namespace formatting_library {
// https://en.cppreference.com/w/cpp/utility/format
void demo()
{
    std::string result = std::format("A={} B={} C={}", "a", std::string("b"), 3);
    assert(result == "A=a B=b C=3");

    // Formats to an output iterator.
    std::string buffer;
    std::format_to
    (
        std::back_inserter(buffer), // the output iterator.
        "Hello, C++{}!", // the format string.
        20 // the argument(s).
    );
    assert(buffer == "Hello, C++20!");
}
}


namespace span {
// https://en.cppreference.com/w/cpp/container/span
void demo()
{
    constexpr int container[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const auto fullspan = std::span{container};
    assert(fullspan.size() == 10);
    const auto subspan = fullspan.subspan(3, 2);
    // Result: 3 4
    assert(subspan.size() == 2);
}
}


namespace starts_with_and_ends_with {
// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/starts_with
// https://en.cppreference.com/w/cpp/algorithm/ranges/ends_with.html
void demo()
{
    constexpr std::string_view hello_world{"hello world"};
    static_assert(hello_world.starts_with("hello"));
    static_assert(!hello_world.starts_with("world"));
    static_assert(hello_world.ends_with('d'));
    static_assert(!hello_world.ends_with("hello"));

    using namespace std::literals;
    static_assert(std::ranges::starts_with("const_cast", "const"sv));
    static_assert(std::ranges::starts_with("constexpr", "const"sv));
    static_assert(!std::ranges::starts_with("volatile", "const"sv));

    constexpr auto ascii_upper = [](char8_t c)
    {
        return u8'a' <= c && c <= u8'z' ? static_cast<char8_t>(c + u8'A' - u8'a') : c;
    };

    constexpr auto cmp_ignore_case = [=](char8_t x, char8_t y)
    {
        return ascii_upper(x) == ascii_upper(y);
    };

    // Projection 1: the projection to apply to the elements of the range to examine.
    // Projection 2: the projection to apply to the elements of the range to be used as the prefix.
    static_assert(std::ranges::starts_with(u8"Constantinopolis", u8"constant"sv,{}, ascii_upper, ascii_upper));
    static_assert(not std::ranges::starts_with(u8"Istanbul", u8"constant"sv,{}, ascii_upper, ascii_upper));
    // The predicate: the binary predicate that compares the projected elements.
    static_assert(std::ranges::starts_with(u8"Metropolis", u8"metro"sv,cmp_ignore_case));
    static_assert(not std::ranges::starts_with(u8"Acropolis", u8"metro"sv,cmp_ignore_case));

    constexpr static auto v = {1, 3, 5, 7, 9};
    constexpr auto odd = [](int x) { return x % 2; };
    assert(std::ranges::starts_with(v, std::views::iota(1) | std::views::filter(odd) | std::views::take(3)));

    static_assert(not std::ranges::ends_with("for", "cast"));
    static_assert(std::ranges::ends_with("dynamic_cast", "cast"));
    static_assert(not std::ranges::ends_with("as_const", "cast"));
    static_assert(std::ranges::ends_with("bit_cast", "cast"));
    static_assert(not std::ranges::ends_with("to_underlying", "cast"));
    static_assert(std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{3, 4}));
    static_assert(not std::ranges::ends_with(std::array{1, 2, 3, 4}, std::array{4, 5}));
}
}


namespace explicit_object_parameter_this {

struct Struct
{
    // OK.
    // Same as void foo(int i) const &;
    void f1(this Struct const& self, int i);

    // Error: already declared.
    // void f1(int i) const &;

    // Also OK for templates.
    // For member function templates,
    // explicit object parameter allows deduction of type and value category,
    // this language feature is called “deducing this”.
    template<typename Self>
    void f2(this Self&& self);

    // Pass object by value: makes a copy of “*this”.
    void f3(this Struct self, int i);

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
    void f4(this Struct object)
    {
        // invalid use of 'this' in a function with an explicit object parameter
        // auto x = this;

        // There's no implicit "this": use of undeclared identifier 'bar'
        // bar();

        object.f3(1);
    }
};

void demo()
{
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
}


namespace multidimensional_subscript_operator {
void demo()
{
    int array3d[4][3][2]{};
    array3d[3][2][1] = 42;
    assert(array3d[3][2][1] == 42);
}
}


namespace static_operators_and_lambdas {

struct Struct
{
    // Static operators: Can call them without the object instance.
    static int operator()(int a, int b) { return a + b; }
};

void demo()
{
    Struct object;

    // This creates an object (and perhaps the optimizer removes it again).
    assert(Struct{}(1, 0) == 1);

    // This does not create an object, just calls the static method.
    assert(object(1, 0) == 1);

    // Lambda's can be made static too.
    int x {0};
    const static auto lambda1 = [x] { return x;};
    const auto lambda2 = [&x] {x+=10; return x;};
    // But see https://godbolt.org/z/3qeqnEsh8 that static lambdas generate much more code.
    // Consider that a static lambda does the capture (of variable x) once.
    // And that may give unexpected output.
    // The rule from R1 of this paper is basically:
    // A static lambda shall have no lambda-capture.
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1169r4.html#lambdas
    assert (lambda1() == 0);
    assert (lambda2() == 10);
    assert (lambda1() == 0);
    assert (lambda2() == 20);
}
}

namespace auto_x_decay_copy {
void demo()
{
    // A decay-copy is a copy of a variables which has lost some properties.
    // How does auto(x) help?
    // It is an easy way to make a copy of a variable.
    // It clearly communicates that it makes a copy of a variable.

    const auto pop_front = [] (auto& container) {
        std::erase(container, auto(container.front())); // <- Make copy through auto(x)
    };

    std::vector vector {1, 2, 3};
    assert (vector.size() == 3);
    pop_front (vector);
    assert (vector.size() == 2);
    pop_front (vector);
    assert (vector.size() == 1);
}
}


namespace extended_floating_point_types {
#ifdef __STDCPP_FLOAT32_T__
std::float32_t f32 = 0.0;
#endif
#ifdef __STDCPP_FLOAT64_T__
std::float64_t f64 = 0.0;
#endif
#ifdef __STDCPP_FLOAT128_T__
std::float128_t f128 = 0.0;
#endif
}


namespace literal_suffix_z {
void demo()
{
    // Avoid this warning:
    // comparison of integers of different signs: 'int' and 'size_type' (aka 'unsigned long') [-Wsign-compare]
    const std::vector<int> v{2, 4, 6, 8};
    for (auto i = 0uz; i < v.size(); ++i) {
        assert((v.at(i)));
    }
}
}

namespace alias_declarations_in_init_statements {
void demo()
{
    std::vector v {1, 2, 3};
    for (using T = int; T& e : v)
        assert((e));
}
}


namespace brackets_are_optional_for_lambdas {
void demo()
{
    std::string s1 = "s1";
    auto with_parenthesis = [s1 = std::move(s1)] () {
        assert(not s1.empty());
    };

    std::string s2 = "s1";
    auto without_parenthesis = [s2 = std::move(s2)]  {
        assert(not s2.empty());
    };
}
}


namespace character_sets_encodings_escape_sequences {
void demo()
{
    // Consistent character literal encoding.
    static_assert('A' == '\x41');
    static_assert('A' == 0x41);

    // Named universal character escapes.
    // https://www.unicode.org/Public/14.0.0/ucd/NamesList.txt
    assert("\N{CAT FACE}" == std::string("🐱"));
    assert("\N{COW FACE}" == std::string("🐮"));
    assert("\N{NATIONAL PARK}" == std::string("🏞"));

    // Delimited escape sequences.
    assert("\o{111}" == std::string("I"));
    assert("\x{A0}" != std::string(""));
    assert("\u{CAFE}" == std::string("쫾"));
}
}


namespace expected_and_unexpected {
// The class template std::expected provides a way to represent either of two values:
// an expected value of type T, or an unexpected value of type E.

enum class parse_error
{
    invalid_input,
    overflow
};

void demo()
{
    const auto parse_number = [] (std::string_view& str) -> std::expected<double, parse_error>
    {
        const char* begin = str.data();
        char* end;
        float number = std::strtof(begin, &end);

        if (begin == end)
            return std::unexpected(parse_error::invalid_input);
        if (std::isinf(number))
            return std::unexpected(parse_error::overflow);

        str.remove_prefix(end - begin);
        return number;
    };

    {
        std::string_view input = "42.2";
        const auto num = parse_number(input);
        assert(num.has_value());
        assert(*num == 42.2f);
    }

    {
        std::string_view input = "42abc";
        const auto num = parse_number(input);
        assert(num.has_value());
        assert(*num == 42);
    }

    {
        std::string_view input = "meow";
        const auto num = parse_number(input);
        assert(not num.has_value());
        // If num does not have a value, dereferencing it causes undefined behavior
        // and accessing num.value() throws std::bad_expected_access (C++23)
        // try
        // {
        //     assert(num.value() == 0);
        // }
        // catch (const std::exception& exception)
        // {
        //     std::cout << "exception: " << exception.what() << std::endl;
        // }
        assert(num.value_or(123) == 123);
        assert(num.error() == parse_error::invalid_input);
    }

    {
        std::string_view input = "inf";
        const auto num = parse_number(input);
        assert(not num.has_value());
        assert(num.error() == parse_error::overflow);
    }
}
}


namespace dangling_placeholder {

void demo()
{
    auto dangling_iter = std::ranges::max_element(std::array{0, 1, 0, 1});
    static_assert(std::is_same_v<std::ranges::dangling, decltype(dangling_iter)>);
    // std::cout << *dangling_iter;
    // compilation error: no match for 'operator*' (operand type is 'std::ranges::dangling')
}
}


namespace move_only_function {
// https://en.cppreference.com/w/cpp/utility/functional/move_only_function.html
// C++23 introduces the std::move_only_function,
// a new utility for handling callable objects that don't need to be copyable.
// It's a streamlined alternative to std::function,
// designed for cases where you're working with move-only types
// like std::unique_ptr or other non-copyable resources.
void demo()
{
    return;

    std::packaged_task<double()> packaged_task([](){ return 3.14159; });

    std::future<double> future = packaged_task.get_future();

    auto lambda = [task = std::move(packaged_task)]() mutable { task(); };

    //  std::function<void()> function = std::move(lambda); // Error

    // std::move_only_function<void()> function = std::move(lambda); // OK does not yet compile on macOS 15.4.1

    //function();

    std::cout << future.get();

}
}


namespace atomic_wait {
// This performs atomic waiting operations.
// An old value is passed to the ::wait.
// It unblocks the thread if the atomic wait gets another value than the old value passed.
void demo()
{
    std::atomic<bool> all_tasks_completed{false};
    std::atomic<unsigned> completion_count{};
    std::future<void> task_futures[16];
    std::atomic<int> outstanding_task_count{16};

    // Spawn several tasks which take different amounts of time,
    // then decrement the outstanding task count.
    for (std::future<void>& task_future : task_futures)
        task_future = std::async([&]
        {
            // This sleep represents doing real work...
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);

            ++completion_count;
            --outstanding_task_count;

            // When the task count falls to zero, notify
            // the waiter (main thread in this case).
            if (outstanding_task_count.load() == 0)
            {
                all_tasks_completed = true;
                all_tasks_completed.notify_one();
            }
        });

    // Wait here till the atomic variable has a value different from false.
    all_tasks_completed.wait(false);

    assert(completion_count.load() == 16);
    // std::cout << "Tasks completed = " << completion_count.load() << std::endl;
}
}

namespace forward_like {
// https://en.cppreference.com/w/cpp/utility/forward_like.html
// Returns a reference to x which has similar properties to T&&.

struct TypeTeller
{
    void operator()(this auto&& self)
    {
        using self_type = decltype(self);
        using unref_self_type = std::remove_reference_t<self_type>;
        if constexpr (std::is_lvalue_reference_v<self_type>)
        {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const lvalue" << std::endl;
            else
                std::cout << "mutable lvalue" << std::endl;
        }
        if constexpr (std::is_rvalue_reference_v<self_type>) {
            if constexpr (std::is_const_v<unref_self_type>)
                std::cout << "const rvalue" << std::endl;
            else
                std::cout << "mutable rvalue" << std::endl;
        }
    }
};


struct FarStates
{
    std::unique_ptr<TypeTeller> pointer;
    std::optional<TypeTeller> optional;
    std::vector<TypeTeller> container;

    auto&& from_opt(this auto&& self)
    {
        return std::forward_like<decltype(self)>(self.optional.value());
        // It is OK to use std::forward<decltype(self)>(self).opt.value(),
        // because std::optional provides suitable accessors.
    }

    auto&& operator[](this auto&& self, std::size_t i)
    {
        return std::forward_like<decltype(self)>(self.container.at(i));
        // It is not so good to use std::forward<decltype(self)>(self)[i], because
        // containers do not provide rvalue subscript access, although they could.
    }

    auto&& from_ptr(this auto&& self)
    {
        if (!self.pointer)
            throw std::bad_optional_access{};
        return std::forward_like<decltype(self)>(*self.pointer);
        // It is not good to use *std::forward<decltype(self)>(self).ptr, because
        // std::unique_ptr<TypeTeller> always dereferences to a non-const lvalue.
    }
};

void demo()
{
    return;

    FarStates my_state
    {
        .pointer{std::make_unique<TypeTeller>()},
        .optional{std::in_place, TypeTeller{}},
        .container{std::vector<TypeTeller>(1)},
      };

    my_state.from_ptr()();
    my_state.from_opt()();
    my_state[0]();

    std::cout << std::endl;

    std::as_const(my_state).from_ptr()();
    std::as_const(my_state).from_opt()();
    std::as_const(my_state)[0]();

    std::cout << std::endl;

    std::move(my_state).from_ptr()();
    std::move(my_state).from_opt()();
    std::move(my_state)[0]();

    std::cout << std::endl;

    std::move(std::as_const(my_state)).from_ptr()();
    std::move(std::as_const(my_state)).from_opt()();
    std::move(std::as_const(my_state))[0]();
}
}


int main()
{
    scoped_timer::demo();
    binary_search::demo();
    lower_bound_and_upper_bound::demo();
    any_of_all_of_none_of::demo();
    counting::demo();
    min_max_clamp_ranges_minmax::demo();
    latches::demo();
    barriers::demo();
    unconstrained_errors::demo();
    constraint_derived_from::demo();
    demonstrate_constraints::demo();
    shared_and_unique_locks::demo();
    timed_mutex::demo();
    containers_library::demo_contains();
    containers_library::demo_vector_vs_list_allocation();
    containers_library::demo_set_with_hash_key_and_comparator();
    containers_library::demo_forward_list();
    piecewise_construct::demo();
    forward_as_tuple::demo();
    class_design_idioms::raii_resource_acquisition_is_initialization();
    class_design_idioms::curiously_recurring_template_pattern();
    class_design_principles::liskov_substitution_principle::demo();
    language_declarations::declarations();
    language_exceptions::demo();
    lambda_capture::demo();
    assign_two_lambdas_to_same_function_object::demo();
    operator_overloading::demo();
    demo_accumulate::demo();
    async_and_future::demo();
    at_exit::demo();
    variant::demo();
    bind_and_bind_front_and_bind_back::demo();
    demo_bit_operations::demo();
    stream_manipulation::demo();
    common_mathematical_functions::demo();
    chrono_library::demo();
    copy::demo();
    reference_wrappers::demo();
    timer_with_jthread_and_timed_mutex_and_condition_variable::demo();
    condition_variables::demo();
    meta_programming_automatic_unit_conversion::demo();
    enable_shared_from_this::demo();
    swapping_and_exchanging::demo();
    execution_policies::demo();
    filesystem::demo();
    demo_initializer_list::demo();
    output_manipulation::demo();
    inserter::demo();
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
    lock_multiple_simultaneously::demo();
    future_and_promise_and_exception::demo();
    //osyncstream::demo();
    // jthread::demo();
    semaphores::demo();
    //demo_packaged_task::demo();
    //attribute_no_unique_address::demo();
    designated_initializers::demo();
    aggregate_initialization::demo();
    coroutines::demo();
    modules::demo();
    header_compare::demo();
    formatting_library::demo();
    span::demo();
    starts_with_and_ends_with::demo();
    explicit_object_parameter_this::demo();
    multidimensional_subscript_operator::demo();
    static_operators_and_lambdas::demo();
    literal_suffix_z::demo();
    alias_declarations_in_init_statements::demo();
    brackets_are_optional_for_lambdas::demo();
    character_sets_encodings_escape_sequences::demo();
    expected_and_unexpected::demo();
    dangling_placeholder::demo();
    move_only_function::demo();
    forward_like::demo();
    atomic_wait::demo();
    return EXIT_SUCCESS;
}


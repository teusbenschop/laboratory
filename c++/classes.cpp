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

#include "classes.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <ostream>

namespace classes {

namespace resource_acquisition_is_initialization_raii {
// 1. RAII (Resource Acquisition Is Initialization)
void demo()
{
    {
        // Problem: Resource might be leaked.
        auto* ptr = new int(10);
        // `Use resource.
        // throw ...
        delete ptr;
    }
    {
        // Solution: Use an object to manage the resource.
        // Can also be a user-defined class to manage the resource.
        std::unique_ptr<int> ptr = std::make_unique<int>(10); // Resource acquired
        // Use resource.
        // Resource released automatically when ptr goes out of scope.
    }
}
}


namespace design_idiom_pimpl {
// 2. Pimpl (Pointer to Implementation)
// Public interface is in header file.
// Private implementation is in another file.
// The public interface has a pointer to the implementation.

class Public
{
public:
    Public();
    ~Public();
    void work() const;

private:
    class Pimpl; // Forward declaration.
    Pimpl* m_pimpl; // Pointer to implementation;
};

class Public::Pimpl
{
public:
    void internal_work() const
    {
        // Implementation details...
    }
};

Public::Public() : m_pimpl(new Pimpl())
{
}

Public::~Public() { delete m_pimpl; }

void Public::work() const
{
    m_pimpl->internal_work();
}

void demo()
{
}
}


namespace design_idiom_crtp {

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

void demo()
{
    DerivedClass derived;
    // Call function from base class -> calls the derived function.
    derived.common_function();
    // Call to the derived function.
    derived.specific_function();
    // Assert it ran "specific_function" in both cases.
    assert(derived.value == 2);
}
}


namespace design_idiom_copy_swap {

// 4. Copy and Swap Idiom
// It swaps the current object with a copy of the object being assigned.

class CopySwapClass
{
public:
    // Constructor.
    CopySwapClass(int size) : m_size(size), m_data(new int(size))
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
    int m_size;
    int* m_data;
};

void demo()
{
}
}


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
    void start_engine() const override
    {
    };
};

class Bicycle : public Bike
{
public:
    void start_engine() const override { throw std::runtime_error("No engine available"); };
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


namespace constructors {

// default constructor: C()
// copy constructor: C(const C&)
// copy assignment operator: C operator=(const C&)
// move constructor: C(C&&)
// move assignment operator: C operator=(C&&)
// destructor: ~C()

// Compiler generates all six, unless one of them is user defined.
// Force generation by declaring = default.

// A constructor is trivial if not user-defined, and no virtual inheritance.

struct S1
{
    int value;
};

static_assert(std::is_constructible_v<S1>);
static_assert(std::is_default_constructible_v<S1>);
static_assert(std::is_nothrow_constructible_v<S1>);
static_assert(std::is_nothrow_default_constructible_v<S1>);
static_assert(std::is_destructible_v<S1>);
static_assert(std::is_nothrow_destructible_v<S1>);
static_assert(std::is_copy_constructible_v<S1>);
static_assert(std::is_trivially_copy_constructible_v<S1>);
static_assert(std::is_nothrow_copy_constructible_v<S1>);
static_assert(std::is_copy_assignable_v<S1>);
static_assert(std::is_nothrow_copy_assignable_v<S1>);
static_assert(std::is_trivially_copy_assignable_v<S1>);
static_assert(std::is_move_constructible_v<S1>);
static_assert(std::is_nothrow_move_constructible_v<S1>);
static_assert(std::is_trivially_move_constructible_v<S1>);
static_assert(std::is_move_assignable_v<S1>);
static_assert(std::is_nothrow_move_assignable_v<S1>);
static_assert(std::is_trivially_move_assignable_v<S1>);



void demo()
{
    // Policy: Declare constructors explicit.

    {
        // Default constructor.
        struct S
        {
            int value{};
        };
        constexpr S s;
        static_assert(s.value == 0);
    }
    {
        // User-defined default constructor.
        struct S
        {
            explicit S() : value(1) {}
            int value{};
        };
        const S s;
        assert(s.value == 1);
    }
    {
        // Deleted default constructor.
        struct S
        {
            S() = delete;
        };
        static_assert(not std::is_constructible_v<S>);
    }
    {
        // Deleted destructor.
        struct S
        {
            ~S() = delete;
        };
        static_assert(not std::is_destructible_v<S>);
    }
    {
        struct S {
            ~S() noexcept(false) {
                throw std::runtime_error("destructor should never throw although C++ allows it");
            }
        };
    }
    // Copy constructor is called when the object is passed by value to a function.
    {
        // Default copy constructor.
        struct S
        {
            int value{};
        };
        S s1;
        s1.value = 2;
        S s2 = s1;
        assert(s2.value == 2);
    }
    {
        // Deleted copy constructor.
        struct S
        {
            S(const S&) = delete;
        };
        static_assert(not std::is_copy_constructible_v<S>);
    }
    {
        // User defined copy constructor.
        // Constructs new object, copies values from existing object.
        struct S
        {
            S() = default;
            S(const S& s)
            {
                this->value = s.value + 1; // Some non-default construction.
            }
            int value{};
        };
        static_assert(std::is_copy_constructible_v<S>);
        static_assert(not std::is_trivially_copy_constructible_v<S>);
        S s1; // <-- call default constructor
        s1.value = 1;
        S s2 = s1; // <- call copy constructor
        assert(s2.value == 2);
    }
    {
        // User-defined copy assignment operator.
        // Copy values from other object into existing object.
        // Does not construct a new object.
        struct S
        {
            S& operator=(const S& other)
            {
                // Good custom is to handle assignment to self: Avoids data loss in case of pointers (delete / new).
                if (this != &other) {
                    value = other.value + 1;
                }
                // Good custom to enable chaining: Assignment operators return reference to *this.
                return *this;
            }
            int value{};
        };
        S s1, s2;
        s1.value = 1;
        s2 = s1;
        assert(s2.value == 2);
        assert(s1.value == 1);
        // Chaining.
        S s3 = s2 = s1;
    }
    {
        // Deleted move constructor.
        struct S
        {
            S(S&&) = delete;
        };
        static_assert(not std::is_move_constructible_v<S>);
    }
    {
        // User-defined move constructor.
        struct S
        {
            S() = default;
            S(S&& other) noexcept {
                value = other.value + 1;
            }
            int value{};
        };
        S s1;
        S s2 = std::move(s1);
        assert(s2.value == 1);
    }
    {
        // User-defined move assignment operator
        struct S
        {
            S& operator=(S&& other) noexcept
            {
                value = other.value + 1;
                return *this;
            }
            int value{};
        };
        S s1, s2;
        s2 = std::move(s1);
        assert(s2.value == 1);
    }
    {
        // The delegating constructor: The class appears in the member initializer list.
        struct S
        {
            S (char c, int i) {}
            S (int i) : S('c', i) {}
            // S(int) delegates to S(char, int).
        };
    }
    {
        // Inheriting constructors.
        struct Base
        {
            Base(int, ...) { }
        };
        struct Derived : Base
        {
            using Base::Base; // Inherits Base(int, ...)
        };
        Derived d1 (1, 2, 3); // Calls the Base constructor.
    }
}
}


namespace inheritance {

struct Base {
    explicit Base() = default;

    // User-defined copy constructors and copy assignment operators should copy all members.
    Base& operator=(const Base& rhs) {
        base_value = rhs.base_value;
        return *this;
    }
    char base_value {'b'};

    // Rule of thumb: If a base class has virtual functions, declare the destructor virtual to prevent leaks.
    virtual ~Base() = default;

    virtual void base() { }
    virtual void base_pure_virtual_function() = 0;
};

struct Derived : public Base {
    explicit Derived() = default;

    // Derived copy constructors and copy assignment operators should copy all members so call base copy equivalent.
    Derived& operator=(const Derived& rhs) {
        Base::operator=(rhs);
        derived_value = rhs.derived_value;
        return *this;
    }
    char derived_value {'d'};

    ~Derived() override = default;
    void base() override { }
    // Pure virtual function must be implemented in derived class.
    void base_pure_virtual_function() override { }
};

// Don't call virtual functions during construction or destruction because
// such calls will not go to a more derived class than the currently running ctor / dtor.

void demo()
{
    {
        auto* d = new Derived;
        d->base();
        d->base_pure_virtual_function();
        delete d;
    }
    {
        Derived d1;
        d1.base_value = 'z';
        Derived d2;
        assert(d2.base_value == 'b');
        d2 = d1;
        // Check Base copy assignment operator was called.
        assert(d2.base_value == 'z');
    }
}
}


void demo()
{
    resource_acquisition_is_initialization_raii::demo();
    design_idiom_pimpl::demo();
    design_idiom_crtp::demo();
    design_idiom_copy_swap::demo();
    liskov_substitution_principle::demo();
    constructors::demo();
    inheritance::demo();
}
}

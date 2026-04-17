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

namespace design_idiom_raii {
// 1. RAII (Resource Acquisition Is Initialization)
void demo()
{
    std::unique_ptr<int> ptr = std::make_unique<int>(10); // Resource acquired
    // Use ptr
    // Resource released automatically when ptr goes out of scope
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


void demo()
{
    design_idiom_raii::demo();
    design_idiom_pimpl::demo();
    design_idiom_crtp::demo();
    design_idiom_copy_swap::demo();
    liskov_substitution_principle::demo();
}
}

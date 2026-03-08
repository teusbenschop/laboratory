#include <iostream>
#include <memory>
#include "../functions.h"

namespace language::classes {
// 1. RAI (Resource Acquisition is Initialization)
void rai_resource_acquisition_is_initialization()
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
        std::cout << "specific function" << std::endl;
    }
};

void curiously_recurring_template_pattern()
{
    DerivedClass derived;
    derived.common_function(); // Call function from base class -> calls the derived function.
    derived.specific_function(); // Call to the derived function.
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


void design_idioms()
{
    rai_resource_acquisition_is_initialization();
    curiously_recurring_template_pattern();
}
}

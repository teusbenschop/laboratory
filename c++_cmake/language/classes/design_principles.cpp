#include <iostream>
#include "../functions.h"

namespace language::classes {
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
class BaseSaveInvoice
{
public:
    virtual void save(GoodInvoice invoice) = 0;
    virtual ~BaseSaveInvoice() = default;
};

class DatabaseSave : public BaseSaveInvoice
{
public:
    void save([[maybe_unused]] GoodInvoice invoice) override
    {
    };
};

class FileSave : public BaseSaveInvoice
{
public:
    void save([[maybe_unused]] GoodInvoice invoice) override
    {
    };
};


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

void liskov_substitution_principle()
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
        std::cout << exception.what() << std::endl;
    }
}


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


void design_principles()
{
    liskov_substitution_principle();
}
}

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

#include "exceptions.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>


namespace exceptions {


// Exception safety
// * Leak no resources.
// * No data corruption.
// * Strong guarantee via copy and swap.
// * Weak guarantee.
// * No-throw guarantee.


namespace exception_hierarchy_and_constructor_parameter_pack {

// It is good practice to have a hierarchy of exception types.

// Example: The base exception is derived from the standard exception.
// The type 1 and type 2 exceptions are derived from the base exception.

template <typename Arg>
concept one_parameter_stream_writable = requires (std::ostream& os, const Arg& arg)
{
    { os << arg } -> std::convertible_to<std::ostream&>;
};

template <typename ...Args>
concept stream_writable = (one_parameter_stream_writable<Args>&& ...);

class Base : public std::exception
{
    std::string m_what;
public:
    template <stream_writable ...Args>
    explicit Base(Args&&...args) noexcept
    {
        std::ostringstream oss;
        (void(oss << std::forward<Args>(args) << ' '), ...);
        m_what.assign(std::move(oss).str());
    }
    [[nodiscard]] const char* what() const noexcept override { return m_what.c_str(); }
};

struct Derived1 : Base { using Base::Base; };
struct Derived2 : Base { using Base::Base; };


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
    catch (const Derived1& exception)
    {
        std::cout << "Catch Type1Exception" << std::endl;
    }
    catch (const Derived2& exception)
    {
        std::cout << "Catch Type2Exception" << std::endl;
    }
    catch (const Base& exception)
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

static void demo()
{
    return;
    // One exception handling function to be used in several places.
    // It uses dynamic casting to handle the different exceptions.
    const auto dynamic_exception_handler = [](const std::exception* exception)
    {
        if (const auto* e = dynamic_cast<const Derived1*>(exception); e)
            std::cout << "Catch Type1Exception" << std::endl;
        else if (const auto* e = dynamic_cast<const Derived2*>(exception); e)
            std::cout << "Catch Type2Exception" << std::endl;
        else if (const auto* e = dynamic_cast<const Base*>(exception); e)
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
        throw Derived1(1, 2);
    }
    catch (const std::exception& e)
    {
        dynamic_exception_handler(&e);
    }
    try
    {
        std::cout << "Throw Type2Exception" << std::endl;
        throw Derived2("");
    }
    catch (const std::exception& e)
    {
        dynamic_exception_handler(&e);
    }
    try
    {
        std::cout << "Throw BaseException" << std::endl;
        throw Base(std::string("a"));
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
    demo_exception_catch_hierarchy(Derived1());
    std::cout << "Throw Type2Exception" << std::endl;
    demo_exception_catch_hierarchy(Derived2());
    std::cout << "Throw BaseException" << std::endl;
    demo_exception_catch_hierarchy(Base());
    std::cout << "Throw standard exception" << std::endl;
    demo_exception_catch_hierarchy(std::runtime_error(""));
}

}


namespace uncaught_exceptions {

// The std::uncaught_exceptions returns how many exceptions have been thrown in the current thread,
// and have not yet entered their matching catch clauses.

struct Struct
{
    char id = '?';
    int ue = std::uncaught_exceptions();

    ~Struct()
    {
        ue = std::uncaught_exceptions();
        std::cout << "Uncaught exceptions in id " << id << " : " << ue << std::endl;
        // Possible usage: If the destructor is called due to a thrown exception,
        // then the number of uncaught exceptions is larger than 0.
        // If so the destructor can roll something back that is not to be committed.
    }
};

static void demo()
{
    return;
    Struct s1('1');
    try
    {
        Struct s2('2');
        throw std::runtime_error("");
        // Destructor of s2 called here, with one uncaught exception.
    }
    catch (...)
    {
        std::cout << "thrown exception" << std::endl;
    }
    // Destructor of s1 called here: zero uncaught exceptions.
}
}


namespace throw_during_construction_or_destruction {
// Throw in constructor: Check which dependent resources should be released.
// Never throw during destruction. If done, program terminates immediately.
static void demo()
{
}
}

void demo ()
{
    exception_hierarchy_and_constructor_parameter_pack::demo();
    uncaught_exceptions::demo();
    throw_during_construction_or_destruction::demo();
}

}

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
#include <string>


namespace exceptions {

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

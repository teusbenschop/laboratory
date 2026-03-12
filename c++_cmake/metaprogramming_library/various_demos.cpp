#include <cassert>
#include <concepts>
#include <iostream>
#include <ostream>
#include <type_traits>

#include "functions.h"

namespace metaprogramming_library {
void various_demos()
{
    // Demo of removing const, volatile, and reference.
    {
        static_assert(std::is_same_v<std::remove_cvref_t<int>, int>);
        static_assert(std::is_same_v<std::remove_cvref_t<int&>, int>);
        static_assert(std::is_same_v<std::remove_cvref_t<int&&>, int>);
        static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
        static_assert(std::is_same_v<std::remove_cvref_t<const int[2]>, int[2]>);
        static_assert(std::is_same_v<std::remove_cvref_t<const int(&)[2]>, int[2]>);
        static_assert(std::is_same_v<std::remove_cvref_t<int(int)>, int(int)>);

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

    // Demo of some simple type traits.
    {
        {
            static_assert(std::is_same_v<uint8_t, unsigned char>);
            static_assert(std::is_floating_point_v<decltype(3.f)>);
            static_assert(std::is_unsigned_v<unsigned int>);
            static_assert(not std::is_unsigned_v<int>);
        }

        {
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
        }

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

        {
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
        }

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

    }
}
}

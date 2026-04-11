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

#include "containers.h"

#include <cassert>
#include <exception>
#include <forward_list>
#include <iostream>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

namespace containers {


// The sequence containers:

// array
// fixed-sized inplace contiguous array

// vector
// Dynamic contiguous array
// Resize is expensive.
// Random access in constant time O(1) through the index.

// inplace_vector (C++26)
// dynamically-resizable, fixed capacity, inplace contiguous array

// deque
// double-ended queue

// forward_list
// singly-linked list

// list
// doubly-linked list




void demo()
{
    // Vector constuctors.
    {
        // Vector constuctor with size and one value.
        std::vector <int>v1 (3, 4);
        const auto standard = std::vector{4, 4, 4};
        assert(v1 == standard);
        // Vector constructor based on iterators.
        std::vector<int> v2 (v1.begin(), v1.end());
        assert(v2 == standard);
        // Vector copy constructor.
        std::vector<int> v3 = v1;
        assert(v3 == standard);
    }

    // Implicit class construction.
    {
        class A
        {
        public:
            A (int number):m_number(number) { }
        private:
            int m_number;
        };
        std::vector<A> v;
        // This implicitly creates an instance of A.
        v.push_back(1);
        // Mark the constructor of A "explicit", and the above won't compile.
    }

    // Need to have sufficient class constructors when storing instances of the class in a std::vector.
    // Often the default constructors suffice.
    {
        class A
        {
        public:
            A (int _number):m_number1 (_number), m_number2 (0)
            {
                std::cout << "normal constructor ";
            }

            A ()
            {
                std::cout << "default constructor ";
            }

            A (const A & source)
            {
                m_number1 = source.m_number1;
                m_number2 = source.m_number2;
                std::cout << "copy constructor ";
            }

            A& operator= (const A & source)
            {
                m_number1 = source.m_number1;
                m_number2 = source.m_number2;
                std::cout << "assignment operator ";
                return *this;
            }

        private:
            int m_number1;
            int m_number2;
        };

        const auto test_constructors = []
        {
            std::vector <A> v1 (1); // calls default constructor.
            std::cout << std::endl;

            v1.push_back (1); // calls normal constructor, then copy constructor, then copy constructor.
            std::cout << std::endl;

            v1[0] = 10; // calls normal constructor, then assignment operator.
            std::cout << std::endl;

            v1.resize(1);
            std::vector <A> v2 (v1); // v1 has one element, calls copy constructor once.
            std::cout << std::endl;

            // assignment operator - empty target
            std::vector< A > v3;
            v3 = v1; // v1 has one element, calls copy constructor once.
            std::cout << std::endl;

            //assignment - not empty target
            std::vector<A> v4 (2); // calls default constructor twice.
            v4 = v1; // calls assignment operator once.
        };
        // test_constructors();
    }

    // If a std::vector gets resized, it moves all of its data to another memory location.
    // The std::list does not do that.
    // This demo demonstrates that behavior.
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



    // Demo of .contains.
    {
        constexpr auto word{"word"};
        std::multiset<std::string> bag;
        bag.insert(word);
        assert(bag.contains(word));
        assert(not bag.contains(""));
    }

    // Demonstrate a std::set with a hash key and a comparator.
    {
        struct Display
        {
            int id;
            std::string message;
        };

        // A hash function object to provide a fingerprint of the visual characteristics of a Display.
        struct DisplayHash
        {
            size_t operator()(const Display& display) const noexcept
            {
                return std::hash<std::string>{}(display.message);
            }
        };

        // An Equal-to object to satisfy the visual characteristics' comparison.
        struct DisplayEquals
        {
            bool operator()(const Display& dl, const Display& dr) const noexcept
            {
                return dl.id == dr.id and dl.message == dr.message;
            }
        };

        // An unordered set, with hash function to generate the key, and an equal operator.
        std::unordered_set<Display, DisplayHash, DisplayEquals> display_set{};

        // Insert three Displays into the set.
        // Check that the size is 3.
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

    // Singly linked list.
    // Supports fast insertion and removal anywhere.
    {
        std::forward_list<int> list = {3, 1, 2, 3, 3};
        list.sort();
        list.unique();
        const auto standard = std::forward_list<int>{1, 2, 3};
        assert(list == standard);
    }
}
}

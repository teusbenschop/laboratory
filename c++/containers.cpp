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
#include <deque>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <unordered_set>
#include <vector>

#include "clocking.h"

namespace containers {


// The sequence containers:

// array
// fixed-sized inplace contiguous array

// vector
// Dynamic contiguous array
// Resize is expensive.
// Random access in constant time O(1) through the index.

// deque
// Double-ended queue
// Not in contiguous memory.
// Fast insertion at both ends, no need to reallocate data.
// Random access is O(1), insertion at ends too, random insertion is O(n) (linear).

// list
// Doubly-linked list
// Not in contiguous memory.
// Insertion and removal is O(1).
// No random access.

// forward_list
// Singly-linked list
// Supports fast insertion and removal anywhere.
// Operator == (searching) has linear complexity O(n)

// The container adaptors:

// stack
// queue
// priority_queue


namespace vectors {
void demo()
{
    // Vector constructors.
    {
        // Vector constructor with size and one value.
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
        class Class
        {
        public:
            Class (const int number):m_number(number) { }
        private:
            int m_number;
        };
        std::vector<Class> v;
        // This implicitly creates an instance of A.
        v.push_back(1);
        // Mark the constructor of A "explicit", and the above won't compile.
    }

    // Need to have sufficient class constructors when storing instances of the class in a std::vector.
    // Often the default constructors suffice.
    {
        class Class
        {
        public:
            Class (int number):m_number1 (number), m_number2 (0)
            {
                std::cout << "normal-constructor ";
            }

            Class ()
            {
                std::cout << "default-constructor ";
            }

            Class (const Class & source)
            {
                m_number1 = source.m_number1;
                m_number2 = source.m_number2;
                std::cout << "copy-constructor ";
            }

            Class& operator= (const Class & source)
            {
                m_number1 = source.m_number1;
                m_number2 = source.m_number2;
                std::cout << "copy-assignment-operator ";
                return *this;
            }

            ~Class()
            {
                std::cout << "destructor " << std::endl;
            }

        private:
            int m_number1;
            int m_number2;
        };

        const auto test_constructors = []
        {
            std::vector <Class> v1 (1); // calls default constructor.
            std::cout << std::endl;

            v1.push_back (1); // calls normal constructor, then copy constructor, then copy constructor.
            std::cout << std::endl;

            v1[0] = 10; // calls normal constructor, then assignment operator.
            std::cout << std::endl;

            v1.resize(1);
            std::vector <Class> v2 (v1); // v1 has one element, calls copy constructor once.
            std::cout << std::endl;

            // assignment operator - empty target
            std::vector< Class > v3;
            v3 = v1; // v1 has one element, calls copy constructor once.
            std::cout << std::endl;

            //assignment - not empty target
            std::vector<Class> v4 (2); // calls default constructor twice.
            v4 = v1; // calls assignment operator once.
        };
        //test_constructors();

        const auto test_destructors = []
        {
            {
                std::vector <Class> v1;
                v1.emplace_back(1); // Calls normal constructor.
                // Calls the class A destructor.
            }
            {
                std::vector <Class*> v1;
                v1.emplace_back(new Class(1)); // Calls normal constructor.
                // Does not call the destructor -> memory leak.
            }
        };
        // test_destructors();
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

    // The std::vector::erase with two iterators removes the range between the two.
    // Note that the value of the last iter is not removed.
    {
        std::vector<int> v{ 1, 2, 3, 4};
        assert(v.size() == 4);
        v.erase(v.begin() + 1, v.begin() + 2);
        assert (v.size() == 3);
    }
}
}


namespace deques {
void demo()
{
    // The deque constructors.
    {
        // Size constructor: fill the container with n values v.
        std::deque<int> d1 (5, 3);
        assert(d1.size() == 5);
        assert(d1.front() == 3);
        assert(d1.back() == 3);

        // Iterator constructor: From an array or from other collections.
        int a1[] = {1, 2, 3};
        std::deque<int> d2 (a1 + 1, a1 + 3); // Contains: 2, 3
        assert(d2.size() == 2);
        assert(d2.front() == 2);
        assert(d2.back() == 3);
    }
}
}


namespace forward_lists {
void demo()
{
    {
        std::forward_list<int> list = {3, 1, 2, 3, 3};
        list.sort();
        list.unique();
        const auto standard = std::forward_list<int>{1, 2, 3};
        assert(list == standard);
    }
}
}


namespace lists {
void demo()
{
    std::list<int> elements1({ 1, 2, 3 });
    std::list<int> elements2({5, 8, 6 });
    elements1.remove(2);
    elements2.remove(8);
    elements1.pop_back();
    elements2.pop_back();
    elements2.insert(elements2.begin(), 7);
    elements2.pop_back();
}
}



namespace iterators {
void demo()
{
    //containers
    std::vector<int> v (10, 1);
    std::deque<int> d (10, 1);
    std::list<int> l (10, 1);

    //iterators
    std::vector<int>::iterator it1;
    std::vector<int>::const_iterator it2;
    std::vector<int>::reverse_iterator it3;
    std::vector<int>::const_reverse_iterator it4;

    std::deque<int>::iterator it5;
    std::deque<int>::const_iterator it6;
    std::deque<int>::reverse_iterator it7;
    std::deque<int>::const_reverse_iterator it8;

    std::list<int>::iterator it9;
    std::list<int>::const_iterator it10;
    std::list<int>::reverse_iterator it11;
    std::list<int>::const_reverse_iterator it12;

    // .begin()  : points to first element.
    // .end()    : points one past last element.
    // .rbegin() : points to last element.
    // .rend()   : points one before first element.

}
}

namespace sizes {
void demo()
{
    std::vector<int> v (10, 1);
    std::deque<int> d (10, 1);
    std::list<int> l (10, 1);
    assert(v.size() == 10);
    assert(d.size() == 10);
    assert(l.size() == 10);
    static_assert(v.max_size() == 4611686018427387903);
    assert(v.capacity() == 10); // Capacity: includes filled and empty slots.
    v.resize(20, 20); // When growing, it inserts value = 20 into the new slots.
    assert(v.capacity() == 20); // When inserting values the capacity increases in bigger jumps.
    v.reserve(100); // Sets the max_size to n. This call may speed up performance.
    assert(v.capacity() == 100);
}
}


namespace assigning {
void demo()
{
    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<int> v;
    v.assign(a, a + 5); // Copies first five elements into v.
    assert(v.size() == 5);
    v.assign(9, 3); // Copies 9 times value 3.
    assert(v.size() == 9);
    assert(v.at(5) == 3);
}
}


namespace inserting {
void demo()
{
    // Insert value at position.
    // Insert n values at position.
    // Insert range of iterators at position.
    // vector – when an increase in size causes it to reallocate,
    // all iterators, references and pointers will be invalidated;
    // deque – all iterators will be invalidated, references also,
    // unless insertion at the beginning or end takes place;
    // list – the iterators and the references remain valid.
}
}


namespace swapping {
void demo()
{
    // Can swap contents of containers, also if sizes differ.
    std::list<int> l1 {1, 2, 3};
    std::list<int> l2 {4, 5};
    l1.swap(l2);
    assert(l1.size() == 2);
    assert(l2.size() == 3);
}
}


namespace splicing {
void demo()
{
    // Splicing removes part from the container argument and inserts it into the calling container.
    // Works on lists only.

    std::list<int>l1 { 1, 2, 3 };
    std::list<int>l2 { 4, 5, 6 };
    std::list<int>l3 { 7, 8, 9 };

    l2.splice (l2.end (), l3);
    assert(l2.size() == 6); // 1 2 3 4 5 6
    assert(l3.size() == 0);

    // moving one element - 6
    std::list<int>::iterator it = l2.begin();
    std::advance (it, 2);
    l1.splice (l1.end (), l2, it);
    // l1: 1 2 3 6
    // l2: 4 5 7 8 9
    assert(l1.size() == 4);
    assert(l2.size() == 5);

    //moving range of elements
    it = l1.end ();
    advance (it, -1);
    l1.splice (it, l2, l2.begin (), l2.end ());
    // l1: 1 2 3 4 5 7 8 9 6
    assert(l2.size() == 0);
}
}


namespace removing {
void demo()
{
    {
        struct DeleteOdd
        {
            bool operator() (const int value) const
            {
                if (value % 2)
                    return true;
                return false;
            }
        };
        std::list<int> l {1, 2, 3};
        l.remove_if (DeleteOdd ());
        std::list<int> standard {2};
        assert(l == standard);
    }
    {
        auto delete_even = [] (int value) {
            if (value % 2 == 0)
                return true;
            return false;
        };
        std::list<int> l {1, 2, 3};
        l.remove_if (delete_even);
        std::list<int> standard {1, 3};
        assert(l == standard);
    }
}
}

namespace uniquifying {
void demo()
{
    {
        std::list<int> l{1, 1, 2, 2, 3, 2, 2, 1, 1};
        l.unique ();
        // Note it only makes adjacent values unique, not necessarily the whole list.
        assert(l.size() == 5); // 1 2 3 2 1
    }
    {
        std::list<int> l{1, 1, 2, 2, 3, 2, 2, 1, 1};
        l.sort();
        l.unique();
        assert(l.size() == 3); // 1 2 3
    }
}
}

namespace merging {
void demo()
{
    std::list l1 { 2, 3, 4 };
    std::list l2 { 2, 3 };
    // Merging requires the list to be sorted.
    l1.merge (l2);
    const std::list standard {2, 2, 3, 3, 4};
    assert(l1 == standard);
    // Can also merge with a comparison predicate.
}
}

namespace containing {
void demo()
{
    constexpr auto word{"word"};
    std::multiset<std::string> bag;
    bag.insert(word);
    assert(bag.contains(word));
    assert(not bag.contains(""));
}
}


namespace set_hash_comparators {
// Demonstrate a std::set with a hash key and a comparator.
void demo()
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
}


namespace stacks {
void demo()
{
    // Initializing and constructing a stack.
    {
        const int a1[] = { 1, 2, 3, 4, 5 };

        // Uses the std::deque as underlying container by default.
        std::stack<int> s1;

        // Copy constructor.
        std::stack s2 (s1);

        // Initialization using predefined container.
        std::deque <int>d1 (a1, a1 + 5);
        std::stack s3 (d1);

        // Using non-default storage.
        std::stack <int, std::list<int>> s4;
        std::stack <int, std::vector<int>>s5;

        // Not allowed - iterator constructor
        // Not allowed - copy constructor source and target stack object
        // using different storage containers
        // Not allowed - initialization using predefined container –
        // using different storage object than declared

        // Assignment operator.
        // Both stacks must be of the same type.
        std::stack<int> s6 = s1;

        // Methods push / top / pop.
        s6.push(1);
        s6.push(2);
        assert(s6.size() == 2);
        assert(s6.top() == 2);
        s6.pop();
        assert(s6.top() == 1);
    }
}
}


namespace queues {
void demo()
{
    // The underlying container is standard a deque.

    // Default constructor.
    std::queue<int> q1;

    // Copy constructor.
    std::queue<int> q2 (q1);

    // Initialization using predefined container.
    int a1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::deque<int> d1 (a1, a1 + 10);
    std::queue<int>q3 (d1);

    // Using non-default storage.
    std::queue<int, std::list<int>> q4;
    std::queue<int, std::vector<int>> q5;

    // Non-allowed constructors:
    // Iterator constructor
    // Copy constructor source and target stack object using different storage containers
    // Initialization using predefined container - using different storage object than declared

    assert(q1.size() == 0);
    assert(q1.empty() == true);

    // Methods front / back / push / pop.
    q1.push(1);
    assert(q1.front() == 1);
    q1.push(2);
    assert(q1.front() == 1);
    assert(q1.back() == 2);
    q1.pop();
    assert(q1.front() == 2);
}
}


namespace priority_queues {
void demo()
{
    // Default constructor
    std::priority_queue <int> p1;

    // Copy constructor
    std::priority_queue<int> p2 (p1);

    // Initialization using iterators
    int a1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector <int> v1 (a1, a1 + 10);
    std::priority_queue<int> q3 (v1.begin (), v1.end ());

    // Using non-default storage (standard it uses the vector)
    std::priority_queue <int, std::deque<int>> p4;

    // Providing different comparator type
    std::priority_queue <int, std::vector<int>, std::greater<int>> p5;

    // The methods push / top / pop
    // The highest values is at the top, regardless of the order of pushing the values.
    p1.push(1);
    p1.push(2);
    p1.push(1);
    assert(p1.top() == 2);
    p1.pop();
    assert(p1.top() == 1);
}
}


namespace span {
// https://en.cppreference.com/w/cpp/container/span

constexpr int container[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr auto fullspan = std::span{container};
static_assert(fullspan.size() == 10);
constexpr auto subspan = fullspan.subspan(3, 2);
// Result: 3 4
static_assert(subspan.size() == 2);

void demo()
{
}
}


namespace performance {
void demo()
{
    return;
    {
        std::vector<int> c;
        {
            scoped_timer::scoped_timer<std::chrono::milliseconds> timer;
            for (int i = 0; i < 1000'000; ++i)
                c.push_back(i);
        }
        {

        }
    }
    {
        std::deque<int> c;
        {
            scoped_timer::scoped_timer<std::chrono::milliseconds> timer;
            for (int i = 0; i < 1000'000; ++i)
                c.push_back(i);
        }
    }
    {
        std::list<int> c;
        {
            scoped_timer::scoped_timer<std::chrono::milliseconds> timer;
            for (int i = 0; i < 1000'000; ++i)
                c.push_back(i);
        }
    }
    // vector: 46 ms
    // deque:  34 ms
    // list:   79 ms.
}
}


namespace inserter {
// The inserter is a convenience function template that constructs a std::insert_iterator
// for the container c and its iterator i with the type deduced from the type of the argument.
void demo()
{
    std::multiset<int> ms{1, 2, 3};

    // The std::inserter is commonly used with multi-sets.
    // Add 5 times 2 to the end of the container.
    std::fill_n(std::inserter(ms, ms.end()), 5, 2);
    {
        std::multiset<int> standard{1, 2, 2, 2, 2, 2, 2, 3};
        assert(ms == standard);
    }

    std::vector<int> d{100, 200, 300};
    std::vector<int> v{1, 2, 3, 4, 5};

    // when inserting in a sequence container, insertion point advances
    // because each std::insert_iterator::operator= updates the target iterator
    std::ranges::copy(d, std::inserter(v, std::next(v.begin())));
    {
        std::vector<int> standard{1, 100, 200, 300, 2, 3, 4, 5};
        assert(v == standard);
    }
}
}



void demo()
{
    vectors::demo();
    deques::demo();
    forward_lists::demo();
    lists::demo();
    iterators::demo();
    sizes::demo();
    assigning::demo();
    inserting::demo();
    swapping::demo();
    splicing::demo();
    removing::demo();
    uniquifying::demo();
    merging::demo();
    containing::demo();
    set_hash_comparators::demo();
    stacks::demo();
    queues::demo();
    priority_queues::demo();
    span::demo();
    performance::demo();
    inserter::demo();
}
}

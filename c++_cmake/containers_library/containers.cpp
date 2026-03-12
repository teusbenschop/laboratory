#include <cassert>
#include <iostream>
#include <list>
#include <set>
#include <unordered_set>
#include <string>
#include <vector>

#include "functions.h"

namespace containers_library {
void demo_contains()
{
    constexpr auto word{"word"};
    auto bag = std::multiset<std::string>{};
    bag.insert(word);
    assert(bag.contains(word));
    assert(!bag.contains(""));
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
    for (auto i{0}; i < 100; ++i)
    {
        vector.push_back(1);
        list.push_back(1);
    }
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

    // An unordered set with hash function to generate the key, and an equal operator.
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

void demo_containers()
{
    demo_contains();
    demo_vector_vs_list_allocation();
    demo_set_with_hash_key_and_comparator();
}
}

#include <cassert>
#include <cstdlib>
#include <memory>
#include <string>

#include "functions.h"


namespace language_support_library::basic_memory_management {
void demo()
{
    struct User {
        User(const std::string& name) : m_name(name) { }
        std::string m_name;
    };
    // Established way.
    {
        // Allocate sufficient memory for the User object.
        auto* memory = std::malloc(sizeof(User));
        // Construct new object in existing memory.
        auto* user = new (memory) User("John");
        assert(user->m_name == "John");
        // Call destructor: This does not yet free the memory.
        user->~User();
        // Free memory on heap.
        std::free(memory);
    }
    // Modern way in C++20.
    {
        auto* memory = std::malloc(sizeof(User));
        auto* user_ptr = reinterpret_cast<User*>(memory);
        std::uninitialized_fill_n(user_ptr, 1, User{"John"});
        std::construct_at (user_ptr, User{"John"});
        assert(user_ptr->m_name == "John");
        std::destroy_at(user_ptr);
        std::free(memory);
    }



}
}

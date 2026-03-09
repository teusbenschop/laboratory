#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "functions.h"

namespace containers_library {

void demo_containers()
{
    {
        std::string word{"word"};
        auto bag = std::multiset<std::string>{};
        bag.insert(word);
        assert(bag.contains(word));
    }
}
}

#include <chrono>
#include <iostream>

namespace snippets {
template <typename D>
class scoped_timer
{
    std::chrono::time_point<std::chrono::system_clock, D> m_started_at;
public:
    scoped_timer() noexcept : m_started_at(std::chrono::time_point_cast<D>(std::chrono::system_clock::now())) { }
    ~scoped_timer() noexcept {
        const auto stopped_at = std::chrono::time_point_cast<D>(std::chrono::system_clock::now());
        std::cout << "elapsed " << stopped_at - m_started_at << std::endl;
    }
};

}



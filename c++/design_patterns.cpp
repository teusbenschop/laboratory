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

#include "design_patterns.h"
#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>
#include <vector>

namespace design_patterns {


namespace command {
// Design pattern "Command".
// Take input action, configure which command to run on it.
// Example: Configure keyboard shortcuts result in which actions.

struct BaseCommand
{
    virtual ~BaseCommand() = default;
    virtual int execute() = 0;
};

struct ForwardCommand : BaseCommand
{
    int execute() final { return 1; }
};

struct BackwardCommand : BaseCommand
{
    int execute() final { return -1; }
};

class KeyboardHandler
{
public:
    KeyboardHandler(BaseCommand* forward, BaseCommand* backward) : positive(forward), negative(backward) { }
    [[nodiscard]] int handle_keystroke(const int input) const
    {
        if (input > 0)
            return positive->execute();
        if (input < 0)
            return negative->execute();
        return 0;
    }
private:
    BaseCommand* positive;
    BaseCommand* negative;
};

void demo()
{
    ForwardCommand forward;
    BackwardCommand backward;
    const KeyboardHandler handler(&forward, &backward);
    assert(handler.handle_keystroke(+1) == 1);
    assert(handler.handle_keystroke(-1) == -1);
}
}


namespace flyweight {
// Many objects. Common traits. Have one common part, shared by all objects.
// Result: Improved efficiency in memory and speed.

struct Common
{
    int bricks {2000};
    int roof_tiles {1000};
};
class House
{
    Common& m_common;
public:
    explicit House(Common& common) : m_common(common) {}
    [[nodiscard]] Common& get_common() const { return m_common; }
    int windows {6};
    int rooms {4};
};

void demo()
{
    Common common;
    const House house1(common);
    const House house2(common);
    const House house3(common);
    assert(house1.get_common().bricks == 2000);
    assert(house2.get_common().roof_tiles == 1000);
    assert(house3.get_common().bricks == 2000);
}
}


namespace observer {
// The "Subject" generates notifications.
// The "Observer" registers with the Subject and receives notifications.
// Both are decoupled.

class Observer
{
public:
    virtual ~Observer() = default;
    virtual void on_notify(int id) = 0;
};

class ConcreteObserver : public Observer
{
public:
    ~ConcreteObserver() override = default;
    void on_notify(const int id) override { count += id; }
private:
    int count {0};
};

class Subject
{
public:
    virtual ~Subject() = default;
    void register_observer(Observer* observer) {observers.push_back(observer);};
    void notify(const int id)
    {
        std::ranges::for_each(observers, [id](const auto& observer)
        {
           observer->on_notify(id);
        });
    }
private:
    std::vector<Observer*> observers;
};

class ConcreteSubject : public Subject
{
public:
    ~ConcreteSubject() override = default;
};

void demo()
{
    // Have some "observers", objects to receive the notifications.
    ConcreteObserver observer1, observer2;

    // Have a "subject", that is, a part that generates notifications.
    // Add observers, as many as needed.
    ConcreteSubject concrete_subject;
    concrete_subject.register_observer(&observer1);
    concrete_subject.register_observer(&observer2);

    // The subject sends notifications to the registered observers.
    // It has no idea what the observers do with it.
    concrete_subject.notify(10);

    // The Observer pattern can also be created with functional programming.
}
}


namespace prototype {
// The original idea is that a derived class can clone itself to deliver a new object of itself.
// A "spawner" will generate clones from the prototype.
// An easy implementation with less code is through a template.

struct Struct1 { };
struct Struct2 { };

template<typename T>
std::unique_ptr<T> spawn()
{
    return std::make_unique<T>();
}

void demo()
{
    spawn<Struct1>();
    spawn<Struct2>();
}
}


namespace singleton {
// Singleton:
// 1. A class has only one instance.
// 2. Global access point.

class Singleton
{
public:
    static Singleton& instance()
    {
        static auto* instance = new Singleton();
        return *instance;
    }
private:
    explicit Singleton() = default; // No external entity can instantiate.
};

void demo()
{
    const auto& singleton = Singleton::instance();
}
}

namespace state {

// The "state" something can be in.
enum class state { lying, sitting, standing };

// The "state machine": Something to change state.
state state_machine(const state state, const int input)
{
    switch (state) {
    case state::lying:
        if (input > 0)
            return state::sitting;
        break;
    case state::sitting:
        if (input < 0)
            return state::lying;
        if (input > 0)
            return state::standing;
        break;
    case state::standing:
        if (input < 0)
            return state::sitting;
        break;
    }
    return state;
}

void demo()
{
    assert(state_machine(state::sitting, 1) == state::standing);
}
}


namespace double_buffer {
// Create a series of sequential operations to appear simultaneous.
// 1. Rendering engine writes to one framebuffer.
// 2. Display reads from another framebuffer.
// 3. Rendering ready: Switch buffers.

class DoubleBuffer
{
    std::vector<int> write_buffer;
    std::vector<int> read_buffer {1};
public:
    void write_to_buffer(const int i) { write_buffer.push_back(i); }
    int read_from_buffer(int& v) const { return read_buffer.at(v); }
    void switch_buffer () { std::swap(write_buffer, read_buffer); } // Or switch pointers or references.
};
void demo()
{
    // Engine writes to buffer.
    DoubleBuffer double_buffer;
    double_buffer.write_to_buffer(1);
    // Display reads from buffer.
    int pixel = 0;
    [[maybe_unused]] const int i = double_buffer.read_from_buffer(pixel);
    // On next frame: Switch the buffers.
    double_buffer.switch_buffer();
    // Start over again.
}
}


namespace game_loop {
// Have independent running loop to process things, not depending on input like batch programs.
// Employs a timer with a jthread and timed mutex and condition variable.
void demo()
{
    std::timed_mutex mx;
    std::condition_variable_any cv; // The _any means: Works with any lock, not just a unique_lock.

    // This lambda function takes a stop token as parameter.
    const auto timer = [&](const std::stop_token& stoken)
    {
        // Step 3: Set the timer interval to 100 milliseconds.
        constexpr auto interval = std::chrono::milliseconds(100);
        while (!stoken.stop_requested())
        {
            // Step 4: The stop token has no stop request: Keep going.
            std::unique_lock ulk(mx);
            // Step 5: Enter the condition variable which will wait 100 ms or less in case of a thread stop request.
            if (cv.wait_for(ulk, stoken, interval, [&stoken] { return stoken.stop_requested(); }))
            {
                // Step 8: The condition variable got a stop request and so interrupts its waiting state immediately.
                break;
            }
            // Step 6: Run one timer cycle.
        }
        // Step 9: The thread function quits and the thread automatically joins.
    };

    // Step 1: The main thread starts the timer thread.
    std::jthread thread(timer);
    // Step 2: The main thread will sleep for 350 milliseconds.
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    // Step 7: The jthread will go out of scope, this sends a stop request to the thread function.
}
}


namespace update_method {
// Several encapsulated entities get updated once each cycle of the loop.
struct Entity
{
    void update() {++state;}
    int state{};
};
void demo()
{
    Entity entity;
    // The loop
    {
        entity.update();
    }
}
}


namespace bytecode {
// Make an interpreter, and bytecode that runs in it.
// Slower than native code. Easier to update by changing a blob of data.
void demo()
{
}
}


namespace subclass_sandbox {
// Define behavior in a subclass using a set of operations provided by the base class.
// Sandbox:
// 1. Base class has protected methods that only the subclasses can access.
// 2. Subclasses don't have random #includes to nose into other code.
void demo()
{
}
}


namespace type_object {
// Allow flexible creation of new classes
// by creating a single class,
// each instance of which represents a different type of object.

// This object defines the "type" of another object.
struct Type
{
    int value{};
};

// This object derives its characteristics from the type object it has a reference to.
class Typed
{
    Type& m_type;
public:
    explicit Typed(Type& type) : m_type(type) {}
};

void demo()
{
    Type type1(1);
    Type type2(2);
    Typed typed1 (type1);
    Typed typed2 (type2);
    // Read the above from a configuration file: No need to hard-code and recompile etc.
}
}


namespace component {
// Allow an umbrella entity to span multiple domains.
// The domains are decoupled from one another.

struct InputComponent
{
    int get_key() const {return key;}
    int key{};
};

struct OutputComponent
{
    void output(const int v) { value = v; }
    int value{};
};
class Umbrella
{
    const InputComponent& m_input_component;
    OutputComponent& m_output_component;
public:
    Umbrella(const InputComponent& input_component, OutputComponent& output_component)
        : m_input_component(input_component), m_output_component(output_component)
    {}
};

void demo()
{
    const InputComponent input_component;
    OutputComponent output_component;
    Umbrella umbrella (input_component, output_component);
}
}


namespace event_queue {
// Decouple message sending and processing moments.
std::queue<int> message_queue;
// 1. Single-cast queue.
// 2. Broadcast queue (multiple listeners).
// 3. Work queue (multiple listeners, one takes the job).
void demo()
{
    message_queue.push(1);
    int m = message_queue.front();
    message_queue.pop();
}
}


namespace service_locator {
// Provide a global access point to a service.
// No coupling between clients and implementation.

struct Service { /* implementation */ };

static Service service{}; // The live service object.

static const Service& locate_service() { return service; };  // The service locator.

void demo()
{
    const auto& service = locate_service();
}
}


namespace dirty_flag {
// Set a flag in an object to defer work until the result is needed.
struct Work
{
    bool dirty{false};
    void do_heavy_work(){}
};
void demo()
{
    Work work;
    if (work.dirty)
        work.do_heavy_work();
    work.dirty = true;
    if (work.dirty)
        work.do_heavy_work();
}
}


namespace object_pool {
// Improve performance and memory by reusing objects from a fixed pool
// instead of allocating and freeing them individually.
// Avoids heap fragmentation.
struct ReusableObject
{
    bool in_use {false};
    int data{};
};
void demo()
{
    // The pool with very many reusable objects, all not in use yet.
    std::vector<ReusableObject> reusable_objects(10'000);
    // Start using them.
    for (int i = 0; i < 100; i++)
        reusable_objects.at(i).in_use = true;
    // Release them.
    for (int i = 0; i < 100; i++)
        reusable_objects.at(i).in_use = false;
}
}


namespace spatial_partition {
// Place objects in arrays which indicate the distance from one object to another.
// This makes it faster to locate objects relative to each other depending on distance.
void demo()
{
}
}


void demo()
{
    command::demo();
    flyweight::demo();
    observer::demo();
    prototype::demo();
    singleton::demo();
    state::demo();
    double_buffer::demo();
    game_loop::demo();
    update_method::demo();
    type_object::demo();
    subclass_sandbox::demo();
    bytecode::demo();
    type_object::demo();
    component::demo();
    event_queue::demo();
    service_locator::demo();
    dirty_flag::demo();
    object_pool::demo();
    spatial_partition::demo();
}
}

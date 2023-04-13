#include <cstdlib>
#include <cstddef>
#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <random>
#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <latch>
#include <barrier>
#include <sstream>
#include <shared_mutex>
#include "main.h"
#include "stuck.h"


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    //threads1();
    //threads2();
    //condition_variable1();
    //async1();
    //jthreads1 ();
    //future1 ();
    //latch1 ();
    //barrier1 ();
    //coroutines1 ();
    //timed_mutex1 ();
    //shared_mutex1();
    //mutex_types();
    stuck_threads();
    return EXIT_SUCCESS;
}


void print (int n, const std::string &str)
{
    std::cout << "Printing integer: " << n << std::endl;
    std::cout << "Printing string: " << str << std::endl;
}


void threads1 ()
{
    std::thread t1(print, 10, "Educative.blog");
    t1.join();
}


void threads2 ()
{
    std::vector <std::string> s { "Educative.blog", "Educative", "courses", "are great" };
    std::vector<std::thread> threads {};
    for (size_t i = 0; i < s.size(); i++) {
        threads.push_back(std::thread(print, i, s[i]));
    }
    for (auto& thread : threads) {
        thread.join();
    }
}


void condition_variable1()
{
    std::condition_variable condition_variable {};
    std::mutex mutex {};
    std::queue<int> queue {};
    bool stopped {false};
    
    std::thread producer{[&]() {
        // Prepare a random number generator.
        // It will simply push random numbers to intq.
        std::default_random_engine generator{};
        std::uniform_int_distribution<int> distribution{};
        
        std::size_t count {10000};
        while (count--)
        {
            {
                // Always lock before changing state guarded by a mutex and condition_variable.
                std::lock_guard<std::mutex> lock{mutex};
                
                // Push a random int into the queue
                int val = distribution (generator);
                queue.push (val);
                std::cout << "Producer pushed " << val << std::endl;
                
            }

            // Tell the consumer it has an int.
            // Best practise is to release the lock before signalling the condition variable.
            condition_variable.notify_one();

            // Wait shortly.
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
        
        // All done.
        // Acquire the lock, set the stopped flag, then inform the consumer.
        std::lock_guard<std::mutex> lock {mutex};
        
        std::cout << "Producer is ready" << std::endl;
        
        stopped = true;
        condition_variable.notify_one();
    }};
    
    std::thread consumer{[&]() {
        do {
            std::unique_lock<std::mutex> lock {mutex};
            condition_variable.wait_for (lock, std::chrono::seconds(1), [&] () {
            // condition_variable.wait (lock, [&] () {
                // Acquire the lock only if we've stopped or the queue isn't empty.
                return stopped || !queue.empty();
            });
            
            // We own the mutex here; pop the queue until it empties out.
            while (!queue.empty())
            {
                const auto& val = queue.front();
                queue.pop();
                std::cout << "Consumer popped: " << val << std::endl;
            }
            
            if (stopped) {
                // Producer has signaled a stop.
                std::cout << "Consumer is done!" << std::endl;
                break;
            }
            
        } while (true);
    }};
    
    consumer.join();
    producer.join();
    
    std::cout << "Example completed" << std::endl;
}


void async1 ()
{
    // Get the start time.
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    // Fetch data from the database.
    std::cout << "Start fetching data from the database" << std::endl;
    std::future <std::string> result_from_db = std::async (std::launch::async, fetch_data_from_db, "data");
    // Fetch data from file.
    std::cout << "Start fetching data from the file" << std::endl;
    std::future <std::string> result_from_file = std::async (std::launch::async, fetch_data_from_file, "data");
    // Fetching data from the database and file will block
    // till the data is available in the two future objects.
    std::string db_data = result_from_db.get();
    std::string file_data = result_from_file.get();
    // Combine the data and print it.
    std::string data = db_data + " :: " + file_data;
    std::cout << "Data = " << data << std::endl;
    // Get the end time.
    auto end = std::chrono::system_clock::now();
    int diff = std::chrono::duration_cast <std::chrono::seconds> (end - start).count();
    std::cout << "Total time taken = " << diff << " seconds" << std::endl;
}


std::string fetch_data_from_db (std::string received_data)
{
    // Make sure that the function takes a short while to complete.
    std::this_thread::sleep_for (std::chrono::seconds(2));
    // Do stuff like creating a database connection and fetching data.
    return "database_" + received_data;
}


std::string fetch_data_from_file (std::string received_data)
{
    // Make sure that the function takes a short while to complete.
    std::this_thread::sleep_for (std::chrono::seconds(2));
    // Do stuff like fetching data from file.
    return "file_" + received_data;
}


void jthreads1 ()
{
    std::cout << std::endl;
    std::cout << std::boolalpha;
    
    std::thread thr{[]{ std::cout << "Joinable std::thread" << std::endl; }};
    
    std::cout << "thr.joinable(): " << thr.joinable() << std::endl;
    
    std::cout << std::endl;
    
    std::cout << std::endl;
    
//    std::jthread nonInterruptable([]{                                   // (1)
//        int counter{0};
//        while (counter < 10){
//            std::this_thread::sleep_for(0.2s);
//            std::cerr << "nonInterruptable: " << counter << std::endl;
//            ++counter;
//        }
//    });
    
//    std::jthread interruptable([](std::interrupt_token itoken){         // (2)
//        int counter{0};
//        while (counter < 10){
//            std::this_thread::sleep_for(0.2s);
//            if (itoken.is_interrupted()) return;                        // (3)
//            std::cerr << "interruptable: " << counter << std::endl;
//            ++counter;
//        }
//    });
    
//    std::this_thread::sleep_for(1s);
    
    std::cerr << std::endl;
    std::cerr << "Main thread interrupts both jthreads" << std:: endl;
//    nonInterruptable.interrupt();
//    interruptable.interrupt();                                          // (4)
    
    std::cout << std::endl;
}


void future1 ()
{
    std::future <int> f1 = std::async([]() { return 123; });
    std::cout << f1.get() << std::endl;
//    std::future <std::string> f2 = f1.then ([](std::future<int> f) {
//        return std::to_string(f.get());      // here .get() won’t block
//    });
//
//    auto myResult= f2.get();
    
    // main.cpp:222:39: error: no member named 'then' in 'std::future<int>'

}


void latch1 ()
{
    struct job {
        const std::string name {};
        std::string product{"not worked"};
        std::thread action{};
    } jobs[] = {{"annika"}, {"buru"}, {"chuck"}};
    
    // The latch class is a downward counter of type std::ptrdiff_t
    // which can be used to synchronize threads.
    // The value of the counter is initialized on creation.
    // Threads may block on the latch until the counter is decremented to zero.
    // There is no possibility to increase or reset the counter,
    // which makes the latch a single-use barrier.
    std::latch work_done {std::size(jobs)};
    std::latch start_clean_up {1};
    
    auto work = [&](job& my_job) {
        my_job.product = my_job.name + " worked";
        work_done.count_down();
        start_clean_up.wait();
        my_job.product = my_job.name + " cleaned";
    };
    
    std::cout << "Starting all the work jobs" << std::endl;
    for (auto& job : jobs) {
        job.action = std::thread{work, std::ref(job)};
    }
    work_done.wait();
    std::cout << "All work has been done" << std::endl;
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }
    
    std::cout << "Workers cleaning up" << std::endl;
    start_clean_up.count_down();
    for (auto& job : jobs) {
        job.action.join();
    }
    std::cout << "Ready cleaning up" << std::endl;
    for (auto const& job : jobs) {
        std::cout << "  " << job.product << '\n';
    }
}


void barrier1 ()
{
    const auto workers = { "anil", "busara", "carl" };
    
    // A function that is run om completion of a barrier.
    auto on_completion = []() noexcept {
        // locking not needed here
        static auto phase = "... done\n" "Cleaning up...\n";
        std::cout << phase;
        phase = "... done\n";
    };

    // Define the barrier to use with the completion callback.
    std::barrier sync_point(std::ssize(workers), on_completion);
    
    // The worker function.
    auto work = [&](std::string name) {
        std::string product = "  " + name + " worked\n";
        std::cout << product;  // ok, op<< call is atomic
        sync_point.arrive_and_wait();
        product = "  " + name + " cleaned\n";
        std::cout << product;
        sync_point.arrive_and_wait();
    };
    
    std::cout << "Starting...\n";
    std::vector<std::thread> threads;
    for (auto const& worker : workers) {
        threads.emplace_back(work, worker);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}


void coroutines1 ()
{
    // https://en.cppreference.com/w/cpp/language/coroutines
    // A coroutine is a function that can suspend execution to be resumed later.
    // Coroutines are stackless.
    // They suspend execution by returning to the caller
    // and the data that is required to resume execution is stored separately from the stack.
    // This allows for sequential code that executes asynchronously
    // (e.g. to handle non-blocking I/O without explicit callbacks),
    // and also supports algorithms on lazy-computed infinite sequences and other uses.
}


void timed_mutex1 ()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back (timed_mutex_job, i);
    }
    for (auto& thread : threads) {
        thread.join();
    }
}


void timed_mutex_job (int id)
{
    std::ostringstream stream {};
    using namespace std::chrono_literals;
    for (int i = 0; i < 3; ++i) {
        if (timed_mutex.try_lock_for (50ms)) {
            std::lock_guard<std::timed_mutex> lock_guard (timed_mutex, std::adopt_lock);
            stream << " success";
            std::this_thread::sleep_for (50ms);
            //timed_mutex.unlock();
        } else {
            stream << " failed";
            i--;
        }
        std::this_thread::sleep_for (50ms);
    }
    std::lock_guard<std::mutex> lock {mutex};
    std::cout << "[" << id << "]" << stream.str() << std::endl;
}


class ThreadSafeCounter {
public:
    ThreadSafeCounter() = default;
    
    // Multiple threads/readers can read the counter's value at the same time.
    unsigned int get() const {
        std::shared_lock lock (mutex);
        return value;
    }
    
    // Only one thread/writer can increment/write the counter's value.
    void increment() {
        std::unique_lock lock (mutex);
        ++value;
    }
    
    // Only one thread/writer can reset/write the counter's value.
    void reset() {
        std::unique_lock lock (mutex);
        value = 0;
    }
    
private:
    mutable std::shared_mutex mutex;
    unsigned int value {0};
};


void shared_mutex1()
{
    ThreadSafeCounter counter {};
    
    std::mutex mutex {};
    
    auto increment_and_print = [&counter, &mutex]() {
        for (int i = 0; i < 3; i++) {
            counter.increment();
            std::unique_lock lock (mutex);
            std::cout << std::this_thread::get_id() << ' ' << counter.get() << std::endl;
        }
    };
    
    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);
    std::thread thread3(increment_and_print);

    thread1.join();
    thread2.join();
    thread3.join();
}


void mutex_types()
{
    std::mutex m_cout;
    std::timed_mutex m_a;
    std::thread lock_first([&m_a, &m_cout]() {
        std::unique_lock ul(m_a);
        m_cout.lock();      // Bad
        std::cout << "Thread " << std::this_thread::get_id() << " locked the mutex.\nPress any key to release the lock.\n";
        m_cout.unlock();     // Bad
        std::getchar();
    });
    
    std::thread try_the_lock([&m_a, &m_cout]() {
        std::unique_lock ul(m_a, std::defer_lock);
        constexpr int max_wait {2};
        m_cout.lock();     // Bad
        std::cout << "Thread " << std::this_thread::get_id() << " waiting at most " << max_wait << " seconds to obtain the lock.\n";
        m_cout.unlock();     // Bad
        if (std::lock_guard cout_lk(m_cout); ul.try_lock_for(std::chrono::seconds(max_wait)))
            std::cout << "Thread " << std::this_thread::get_id() << " successfully obtained the lock.\n";
        else
            std::cout << "Thread " << std::this_thread::get_id() << " failed to obtain the lock in time!\n";
    });
    
    try {
        std::unique_lock cul(m_cout);
        std::cout << std::thread::hardware_concurrency() << " threads supported by the system.\n";
        cul.unlock();
        if (lock_first.joinable())   lock_first.join();
        if (try_the_lock.joinable()) try_the_lock.join();
        
        cul.lock();
        std::cout << "Program complete.\n";
    }
    catch (const std::system_error& e) {
        std::cout << "Whoopsy Daisies: " << e.what() << "\n";
    }

}


void stuck_threads()
{
    std::cout << "Monitoring thread" << std::endl;
    std::vector <stuck *> stuck_threads {};
    stuck * stuck_pointer {nullptr};
    for (int i = 0; i < 3; i++) {
        if (stuck_pointer) stuck_threads.push_back (stuck_pointer);
        stuck_pointer = new stuck(i);
        std::this_thread::sleep_for (std::chrono::seconds(10));
    }
    if (stuck_pointer) stuck_threads.push_back (stuck_pointer);
    for (int i = 0; i < 100; i++) {
        for (auto& stuck_pointer : stuck_threads) {
            if (stuck_pointer) {
                if (stuck_pointer->m_ready) {
                    delete stuck_pointer;
                    stuck_pointer = nullptr;
                }
            }
        }
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
}

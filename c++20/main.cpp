
void source_location()
{
  return;
}


// https://en.cppreference.com/w/cpp/container/span

template<class T, std::size_t N>
[[nodiscard]]
constexpr auto slice(std::span<T,N> s, std::size_t offset, std::size_t width)
{
  return s.subspan(offset, offset + width <= s.size() ? width : 0);
}

void span()
{
  return;
  constexpr int a[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  for (std::size_t offset{}; ; ++offset) {
    constexpr std::size_t width {5};
    const auto subspan = slice(std::span{a}, offset, width);
    if (subspan.empty())
      break;
    for (const auto& elem : subspan)
      std::cout << elem << ' ';
    std::cout << std::endl;
  }
}


void type_traits()
{
  return;
  struct foo
  {
    void m() { std::cout << "non-const non-volatile" << std::endl; }
    void m() const { std::cout << "const" << std::endl; }
    void m() volatile { std::cout << "volatile" << std::endl; }
    void m() const volatile { std::cout << "const volatile" << std::endl; }
  };
  foo{}.m();
  std::add_const<foo>::type{}.m();
  std::add_volatile<foo>::type{}.m();
  std::add_cv<foo>::type{}.m();
  
  int bar {10};
  std::cout << bar << std::endl;
}


// https://en.cppreference.com/w/cpp/thread/barrier
void barrier ()
{
  return;
  const auto persons = { "Person A", "Person B", "Person C", "Person D" };
  
  // A function to run on completion of a barrier.
  auto on_barrier_completion = []() noexcept {
    // Locking not needed here.
    static auto phase {"Ready working"};
    std::cout << phase << std::endl;
    phase = "Ready cleaning up";
  };
  
  // Define the barrier to use with the completion callback.
  std::barrier barrier(std::ssize(persons), on_barrier_completion);
  
  
  // The worker function.
  std::mutex mutex {};
  auto worker = [&](std::string name)
  {
    {
      std::lock_guard<std::mutex> lock (mutex);
      std::cout << " " << name << " worked" << std::endl;
    }
    barrier.arrive_and_wait();
    {
      std::lock_guard<std::mutex> lock (mutex);
      std::cout << " " << name << " cleaned" << std::endl;
    }
    barrier.arrive_and_wait();
  };
  
  std::cout << "Starting" << std::endl;
  std::vector<std::thread> threads{};
  for (auto const& person : persons) {
    threads.emplace_back(worker, person);
  }
  for (auto& thread : threads) {
    thread.join();
  }
}


// https://en.cppreference.com/w/cpp/thread/latch
void latch()
{
  return;
  
  struct job {
    const std::string name;
    std::string product{};
    std::thread action{};
  };
  
  job jobs[] = {{"Person A"}, {"Person B"}, {"Person C"}};
  
  std::latch work_done {std::size(jobs)};
  std::latch start_clean_up {1};
  
  const auto work = [&](job& my_job) {
    my_job.product = my_job.name + " worked";
    work_done.count_down();
    start_clean_up.wait();
    my_job.product = my_job.name + " cleaned";
  };
  
  std::cout << "Starting jobs...";
  for (auto& job : jobs) {
    job.action = std::thread{work, std::ref(job)};
  }
  work_done.wait();
  std::cout << "ready\n";
  for (auto const& job : jobs) {
    std::cout << job.product << '\n';
  }
  
  std::cout << "Workers cleaning up...";
  start_clean_up.count_down();
  for (auto& job : jobs) {
    job.action.join();
  }
  std::cout << "ready\n";
  for (auto const& job : jobs) {
    std::cout << job.product << '\n';
  }
}


// https://en.cppreference.com/w/cpp/thread/counting_semaphore
void semaphore()
{
  return;
  // Object counts are set to zero.
  // Objects are in non-signaled state.
  // The counting_semaphore contains an internal counter initialized by the constructor.
  // This counter is decremented by calls to acquire() and related methods,
  // and is incremented by calls to release().
  // When the counter is zero, acquire() blocks until the counter is incremented.
  // The binary_semaphore is a counting_semaphore with max count = 1.
  std::binary_semaphore signal_main_to_thread_semaphore{0};
  std::binary_semaphore signal_thread_to_main_semaphore{0};

  const auto thread_processor = [&]()
  {
    // Wait for a signal from the main process by attempting to decrement the semaphore.
    signal_main_to_thread_semaphore.acquire();
    
    // This call blocks until the semaphore's count is increased from the main process.
    std::cout << "Thread got the signal" << std::endl;
    
    // Wait shortly to imitate some work being done by the thread.
    using namespace std::literals;
    std::this_thread::sleep_for(1s);
    
    std::cout << "Thread sends the signal" << std::endl;
    
    // Signal the main process back.
    signal_thread_to_main_semaphore.release();
  };
  
  // Create a worker thread
  std::thread worker_thread(thread_processor);

  // Signal the worker thread to start working by increasing the semaphore's count.
  std::cout << "Main sends the signal" << std::endl;
  signal_main_to_thread_semaphore.release();
  
  // Wait until the worker thread is done doing the work
  // by attempting to decrement the semaphore's count.
  signal_thread_to_main_semaphore.acquire();
  
  std::cout << "Main got the signal" << std::endl;
  worker_thread.join();
}


// https://en.cppreference.com/w/cpp/thread/jthread
// It has the same general behavior as std::thread,
// except that jthread automatically joins on destruction,
// and can be cancelled/stopped in certain situations.

void joinable_thread_basic()
{
  return;
  
  using namespace std::literals::chrono_literals;
  std::cout << "Joining thread" << std::endl;
  
  std::jthread joining_thread { [] (std::stop_token stoptoken) {
    while (!stoptoken.stop_requested()) {
      std::cout << "." << std::flush;
      std::this_thread::sleep_for(100ms);
    }
    std::cout << std::endl;
  }};
  
  std::this_thread::sleep_for(2500ms);
  std::cout << std::endl << "Request stop" << std::endl;
  joining_thread.request_stop();
//  joining_thread.join(); // Not needed here.
}


void joinable_thread_advanced()
{
  return;
  
  using namespace std::literals::chrono_literals;

  std::mutex mutex{};
  std::jthread threads [4];

  const auto print = [](const std::stop_source &source)
  {
    std::cout << std::boolalpha << "stop_source stop_possible = " << source.stop_possible() << " stop_requested = " << source.stop_requested() << std::endl;
  };
  
  // Common stop source.
  std::stop_source stop_source;
  print(stop_source);
  
  const auto joinable_thread_worker = [&mutex] (const int id, std::stop_source stop_source)
  {
    using namespace std::literals::chrono_literals;
    std::stop_token stoken = stop_source.get_token();
    while (true) {
      std::this_thread::sleep_for(300ms);
      std::lock_guard lock (mutex);
      if (stoken.stop_requested()) {
        std::cout << "worker " << id << " is requested to stop" << std::endl;
        return;
      }
      std::cout << "worker " << id << " goes back to sleep" << std::endl;
    }
  };
  
  // Create worker threads.
  for (int i = 0; i < 4; ++i) {
    threads[i] = std::jthread(joinable_thread_worker, i+1, stop_source);
  }
  
  std::this_thread::sleep_for(700ms);
  
  std::cout << "request stop" << std::endl;
  stop_source.request_stop();
  
  print(stop_source);
  
  // Note: destructor of jthreads will call join so no need for explicit calls
}


// Use a helper class for atomic std::cout streaming
class atomic_writer
{
  std::ostringstream buffer;
public:
  ~atomic_writer() {
    std::cout << buffer.str() << std::endl;
  }
  atomic_writer& operator<<(auto input) {
    buffer << input;
    return *this;
  }
};


void stop_call_back ()
{
  /*
  using namespace std::literals::chrono_literals;

  // A worker thread.
  // It will wait until it is requested to stop.
  std::jthread worker([] (std::stop_token stoptoken) {
    atomic_writer() << "Worker thread id: " << std::this_thread::get_id();
    std::mutex mutex;
    std::unique_lock lock(mutex);
    std::condition_variable_any().wait(lock, stoptoken, [&stoptoken] {
      return stoptoken.stop_requested();
    });
  });
  
  // Register a stop callback on the worker thread.
  std::stop_callback callback(worker.get_stop_token(), [] {
    atomic_writer() << "Stop callback executed by thread: "  << std::this_thread::get_id();
  });
  
  // The stop_callback objects can be destroyed prematurely to prevent execution.
  {
    std::stop_callback scoped_callback(worker.get_stop_token(), [] {
      // This will not be executed.
      atomic_writer() << "Scoped stop callback executed by thread: " << std::this_thread::get_id();
    });
  }
  
  // Demonstrate which thread executes the stop_callback and when.
  // Define a stopper function
  auto stopper_func = [&worker] {
    if(worker.request_stop())
      atomic_writer () << "Stop request executed by thread: " << std::this_thread::get_id();
    else
      atomic_writer() << "Stop request not executed by thread: " << std::this_thread::get_id();
  };
  
  // Let multiple threads compete for stopping the worker thread
  std::jthread stopper1(stopper_func);
  std::jthread stopper2(stopper_func);
  stopper1.join();
  stopper2.join();
  
  // After a stop has already been requested, a new stop_callback executes immediately.
  atomic_writer() << "Main thread: " << std::this_thread::get_id();
  std::stop_callback callback_after_stop(worker.get_stop_token(), [] {
    atomic_writer() << "Stop callback executed by thread: " << std::this_thread::get_id();
  });
   */
}


// https://en.cppreference.com/w/cpp/io/basic_osyncstream
// The class template std::basic_osyncstream is a convenience wrapper for std::basic_syncbuf.
// It provides a mechanism to synchronize threads writing to the same stream.

static void stream_worker (int id)
{
  using namespace std::literals::chrono_literals;
  for (int i = 0; i < 10; i++) {
    std::this_thread::sleep_for(300ms);
    std::osyncstream synced_out(std::cout);
    synced_out << "worker " << id << std::endl;
  }
}

void sync_stream()
{
  return;
  using namespace std::literals::chrono_literals;
  std::jthread threads [4];
  for (int i = 0; i < 4; ++i) {
    threads[i] = std::jthread(stream_worker, i);
  }
}


// https://en.cppreference.com/w/cpp/string/basic_string/starts_with
// https://en.cppreference.com/w/cpp/string/basic_string/ends_with

void starts_ends_with()
{
  return;
  const std::string hello_world {"hello world"};
  
  const auto test_starts_width = [&hello_world](const auto prefix)
  {
    std::cout << std::quoted(hello_world) << " starts with " << prefix << ": ";
    std::cout << std::boolalpha << hello_world.starts_with(prefix) << std::endl;
  };

  const auto test_ends_width = [&hello_world] (const auto prefix)
  {
    std::cout << std::quoted(hello_world) << " ends with " << prefix << ": ";
    std::cout << std::boolalpha << hello_world.ends_with(prefix) << std::endl;
  };
  
  using namespace std::literals;
  
  test_starts_width(std::string_view("hello"));
  test_starts_width(std::string_view("goodbye"));
  test_starts_width('h');
  test_starts_width('x');
  test_starts_width("h");
  test_starts_width("x");
  test_ends_width("ld"sv);
  test_ends_width("ld"s);
  test_ends_width('d');
  test_ends_width('e');
  test_ends_width("d");
}


// https://en.cppreference.com/w/cpp/utility/functional/bind_front

// Function templates std::bind_front and std::bind_back generate
// a perfect forwarding call wrapper which allows to invoke the callable target
// with its first or last sizeof...(Args) parameters bound to args.
void bind_front_back()
{
  return;
  
  const auto minus = [] (int a, int b) -> int {
    return a - b;
  };

  constexpr auto fifty_minus_value = std::bind_front(minus, 50);
  std::cout << fifty_minus_value(3) << std::endl; // equivalent to `minus(50, 3)`

  struct S
  {
    int val;
    int minus(int arg) const noexcept { return val - arg; }
  };

  constexpr auto member_minus = std::bind_front(&S::minus, S{50});
  std::cout << member_minus(3) << std::endl; // equivalent to `S tmp{50}; tmp.minus(3)`
  
  // The noexcept specification is preserved.
  static_assert(!noexcept(fifty_minus_value(3)));
  static_assert(noexcept(member_minus(3)));
  
  // Binding of a lambda.
  constexpr auto plus = [](int a, int b) { return a + b; };
  constexpr auto forty_plus = std::bind_front(plus, 40);
  std::cout << forty_plus(7) << '\n'; // equivalent to `plus(40, 7)`
  
  auto madd = [](int a, int b, int c) { return a * b + c; };
  auto mul_plus_seven = std::__bind_back(madd, 7);
  std::cout << mul_plus_seven(4, 10) << '\n'; // equivalent to `madd(4, 10, 7)`
}


void distribution()
{
  return;
  
  const int nrolls = 10000; // number of experiments
  const int nstars = 100;   // maximum number of stars to distribute

  // Define a Mersenne-Twister pseudo random number generator, only once in the application:
  std::mt19937 generator1;

  std::default_random_engine generator;

  std::poisson_distribution<int> distribution(4.1);
  
  int p[10]={};
  
  for (int i=0; i<nrolls; ++i) {
    int number = distribution(generator);
    if (number<10) ++p[number];
  }
  
  for (int i = 0; i < 10; ++i)
    std::cout << i << ": " << std::string(p[i]*nstars/nrolls,'*') << std::endl;

  {
    // Define the Poisson distribution engine with the desired mean value:
    std::poisson_distribution<int> distrib(1);
    // Generate a 50-piece sample of integers and print on stdout:
    for (int i=0 ; i<50; i++)
      std::cout << distrib(generator1) << " ";
    std::cout << "\n\n";
  }
}


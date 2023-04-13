#include <string>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]);
void print (int n, const std::string &str);

void threads1 ();
void threads2 ();

void condition_variable1();

void async1 ();
std::string fetch_data_from_db (std::string received_data);
std::string fetch_data_from_file (std::string received_data);

void jthreads1 ();

void future1 ();

void latch1 ();

void barrier1 ();

void coroutines1 ();

void timed_mutex1 ();
std::mutex mutex;
std::timed_mutex timed_mutex;
void timed_mutex_job (int id);

void shared_mutex1();
void mutex_types();

void stuck_threads();


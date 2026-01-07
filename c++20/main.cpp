





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


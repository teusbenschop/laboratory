#include <ncurses.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]);

class NcursesWindow
{
public:
  NcursesWindow();
  ~NcursesWindow();
  void run ();
private:
  WINDOW *w;
};

int main1([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]);
void printing();
void moving_and_sleeping();
void colouring();


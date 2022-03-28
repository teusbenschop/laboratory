#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  std::cout << "ncurses lab - press ctrl-c to quit" << std::endl;

  // Initialize Ncurses
  initscr();

  {
    NcursesWindow ncurses_window;
    ncurses_window.run ();
  }

  // End it.
  endwin();

  return EXIT_SUCCESS;
}


int main1([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  std::cout << "ncurses lab - press ctrl-c to quit" << std::endl;
  
  initscr();

  addstr("-----------------\n| codedrome.com |\n| ncurses Demo  |\n-----------------\n\n");
  refresh();

  //printing();

  //moving_and_sleeping();

  //colouring();

  addstr("\npress any key to exit...");
  refresh();

  getch();

  // Wait for any signal.
  //pause();

  endwin();

  return EXIT_SUCCESS;
}


void printing()
{
    addstr("This was printed using addstr\n\n");
    refresh();

    addstr("The following letter was printed using addch:- ");
    addch('a');
    refresh();

    printw("\n\nThese numbers were printed using printw\n%d\n%f\n", 123, 456.789);
    refresh();
}

void moving_and_sleeping()
{
    int row = 5;
    int col = 0;

    curs_set(0);

    for(char c = 65; c <= 90; c++)
    {
        move(row++, col++);
        addch(c);
        refresh();
        napms(100);
    }

    row = 5;
    col = 3;

    for(char c = 97; c <= 122; c++)
    {
        mvaddch(row++, col++, c);
        refresh();
        napms(100);
    }

    curs_set(1);

    addch('\n');
}

void colouring()
{
    if(has_colors())
    {
        if(start_color() == OK)
        {
            init_pair(1, COLOR_YELLOW, COLOR_RED);
            init_pair(2, COLOR_GREEN, COLOR_GREEN);
            init_pair(3, COLOR_MAGENTA, COLOR_CYAN);

            attrset(COLOR_PAIR(1));
            addstr("Yellow and red\n\n");
            refresh();
            attroff(COLOR_PAIR(1));

            attrset(COLOR_PAIR(2) | A_BOLD);
            addstr("Green and green A_BOLD\n\n");
            refresh();
            attroff(COLOR_PAIR(2));
            attroff(A_BOLD);

            attrset(COLOR_PAIR(3));
            addstr("Magenta and cyan\n");
            refresh();
            attroff(COLOR_PAIR(3));
        }
        else
        {
            addstr("Cannot start colours\n");
            refresh();
        }
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }
}


NcursesWindow::NcursesWindow()
{
  // Create a new window.
  w = newwin( 10, 12, 1, 1 );
  // Sets default borders for the window.
  box( w, 0, 0 );
}


NcursesWindow::~NcursesWindow()
{
  delwin (w);

}


void NcursesWindow::run ()
{
  std::vector <std::string> items = { "One", "Two", "Three", "Four", "Five" };
  // now print all the menu items and highlight the first one
  for (int i = 0; i < 5; i++) {
    if (i == 0) wattron (w, A_STANDOUT); // highlights the first item.
    else wattroff (w, A_STANDOUT);
    mvwprintw( w, i+1, 2, "%s", items[i].c_str() );
  }
  wrefresh( w ); // update the terminal screen

  noecho(); // disable echoing of characters on the screen
  keypad( w, TRUE ); // enable keyboard input for the window.
  curs_set( 0 ); // hide the default screen cursor.

  // get the input
  int i = 0;
  int ch = 0;
  while ((ch = wgetch(w)) != 'q'){
    // right pad with spaces to make the items appear with even width.
    mvwprintw( w, i+1, 2, "%s", items[i].c_str() );
    // use a variable to increment or decrement the value based on the input.
    switch( ch ) {
      case KEY_UP:
        i--;
        i = ( i<0 ) ? 4 : i;
        break;
      case KEY_DOWN:
        i++;
        i = ( i>4 ) ? 0 : i;
        break;
    }
    // now highlight the next item in the list.
    wattron( w, A_STANDOUT );
    mvwprintw( w, i+1, 2, "%s", items[i].c_str());
    wattroff( w, A_STANDOUT );
  }

}

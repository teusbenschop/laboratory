#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include "main.h"

// https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/menus.html#MENUUSERPTR

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD   4

const char *choices[] =
{
  "Choice 1",
  "Choice 2",
  "Choice 3",
  "Choice 4",
  "Choice 5",
  "Choice 6",
  "Choice 7",
  "Exit",
};


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  std::cout << "ncurses lab - press ctrl-c to quit" << std::endl;

  ITEM **my_items;
  int c;
  MENU *my_menu;
        int n_choices, i;
  //ITEM *cur_item;
  
  /* Initialize curses */
  initscr();
  start_color();
        cbreak();
        noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

  /* Initialize items */
  n_choices = ARRAY_SIZE(choices);
  my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
  for(i = 0; i < n_choices; ++i)
  {       my_items[i] = new_item(choices[i], choices[i]);
    /* Set the user pointer */
    set_item_userptr(my_items[i], (void *) func);
  }
  my_items[n_choices] = (ITEM *)NULL;

  /* Create menu */
  my_menu = new_menu((ITEM **)my_items);

  /* Post the menu */
  mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
  mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
  post_menu(my_menu);
  refresh();

  while((c = getch()) != KEY_F(1)) {
    switch(c) {
      case KEY_DOWN:
        menu_driver(my_menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver(my_menu, REQ_UP_ITEM);
        break;
      case 10: /* Enter */
      {
        ITEM *cur;
        void (*p)(char *);
        cur = current_item(my_menu);
        //void * p2 = item_userptr(cur);
        p = (void (*)(char *))item_userptr(cur);
        p((char *)item_name(cur));
        pos_menu_cursor(my_menu);
        break;
      }
      break;
    }
  }
  unpost_menu(my_menu);
  for(i = 0; i < n_choices; ++i)
    free_item(my_items[i]);
  free_menu(my_menu);
  endwin();

  pause ();
  
  return EXIT_SUCCESS;
}


void func(char *name)
{
  move(20, 0);
  clrtoeol();
  mvprintw(20, 0, "Item selected is : %s", name);
}


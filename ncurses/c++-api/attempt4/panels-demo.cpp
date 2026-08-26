

#include "panels-demo.h"
#include <cursesapp.h>
#include <cursesf.h>
#include <cursesm.h>
#include <memory>
#include <thread>

void panels_demo()
{
	const auto wait_shortly = [] {
		using namespace std::literals;
		std::this_thread::sleep_for(1s);
	};

	NCursesPanel standard_panel{};
	standard_panel.box();
	standard_panel.move(standard_panel.height() / 2, 1);
	standard_panel.hline(standard_panel.width() - 2);
	standard_panel.move(1, standard_panel.width() / 2);
	standard_panel.vline(standard_panel.height() - 2);
	standard_panel.addch(0, standard_panel.width() / 2, ACS_TTEE);
	standard_panel.addch(standard_panel.height() - 1, standard_panel.width() / 2, ACS_BTEE);
	standard_panel.addch(standard_panel.height() / 2, 0, ACS_LTEE);
	standard_panel.addch(standard_panel.height() / 2, standard_panel.width() - 1, ACS_RTEE);
	standard_panel.addch(standard_panel.height() / 2, standard_panel.width() / 2, ACS_PLUS);
	standard_panel.refresh();
	wait_shortly();

	{
		NCursesPanel panel1(8, 20, 12, 4);
		panel1.boldframe("Panel 1");
		panel1.bkgd(' ' | COLOR_PAIR(1));
		panel1.refresh();
		wait_shortly();

		{
			NCursesPanel panel2(8, 20, 10, 6);
			panel2.frame("Panel 2");
			panel2.bkgd(' ' | COLOR_PAIR(2));
			panel2.refresh();
			wait_shortly();

			if (NCursesApplication::getApplication()->useColors()) {
			}
		}
		standard_panel.refresh();
		wait_shortly();
	}
	standard_panel.refresh();
	wait_shortly();

	//  Don't forget to clean up the main screen.
	//  Since this is the last thing using NCursesWindow,
	//  this has the effect of shutting down ncurses and restoring the terminal state.
	standard_panel.clear();
	standard_panel.refresh();
}

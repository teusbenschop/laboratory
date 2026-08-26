

#include "item-scan.h"
#include <cursesm.h>

ScanItem::ScanItem(const char* name) : NCursesMenuItem(name) {}

bool ScanItem::action()
{
	{
		const NCursesPanel my_std;

		NCursesPanel panel(my_std.lines() - 2, my_std.cols() - 2, 1, 1);
		panel.box();
		panel.refresh();

		NCursesPanel entry(panel.lines() - 6, panel.cols() - 6, 3, 3);
		entry.scrollok(TRUE);
		::echo();

		entry.printw("Enter decimal integers.  The running total will be shown\n");
		int nvalue = -1;
		int result = 0;
		while (nvalue != 0) {
			nvalue = 0;
			entry.scanw("%d", &nvalue);
			if (nvalue != 0)
				entry.printw("%d: ", result += nvalue);
			entry.refresh();
		}
		entry.printw("\nPress any key to continue...");
		entry.getch();
	}
	::noecho();
	return FALSE;
}


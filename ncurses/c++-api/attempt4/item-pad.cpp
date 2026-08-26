

#include "item-pad.h"
#include <cursesm.h>

constexpr int grid_size{3};
constexpr int pad_size{200};

PadItem::PadItem(const char* name) : NCursesMenuItem(name) {    }

bool PadItem::action()
{
	unsigned grid_count {0};

	const NCursesPanel my_std;
	NCursesPanel panel(my_std.lines() - 2, my_std.cols() - 2, 1, 1);
	NCursesFramedPad framed_pad(panel, pad_size, pad_size);

	for (int i = 0; i < pad_size; ++i) {
		for (int j = 0; j < pad_size; ++j) {
			if (i % grid_size == 0 && j % grid_size == 0) {
				if (i == 0 || j == 0)
					framed_pad.addch('+');
				else
					framed_pad.addch('A' + grid_count++ % 26);
			}
			else if (i % grid_size == 0)
				framed_pad.addch('-');
			else if (j % grid_size == 0)
				framed_pad.addch('|');
			else
				framed_pad.addch(' ');
		}
	}

	panel.label("Pad Demo", nullptr);
	framed_pad();
	panel.clear();
	return FALSE;
}

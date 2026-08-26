

#include "item-quit.h"
#include <cursesm.h>

QuitItem::QuitItem() : NCursesMenuItem("Quit") {}

bool QuitItem::action()
{
	return true;
}

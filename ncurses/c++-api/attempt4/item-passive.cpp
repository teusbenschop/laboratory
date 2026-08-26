

#include "item-passive.h"
#include <cursesm.h>
#include <form.h>

PassiveItem::PassiveItem(const char* text) : NCursesMenuItem(text)
{
	options_off(O_SELECTABLE);
	options_on(O_VISIBLE);
}



#include "form-label.h"
#include <cursesf.h>

FormLabel::FormLabel(const char* title, int row, int col)
	: NCursesFormField(1, static_cast<int>(::strlen(title)), row, col)
{
	set_value(title);
	options_off(O_EDIT | O_ACTIVE);
}

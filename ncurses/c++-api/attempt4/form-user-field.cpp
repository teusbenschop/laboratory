

#include "form-user-field.h"
#include <cursesf.h>

bool MyFieldType::field_check([[maybe_unused]] NCursesFormField& f)
{
	return true;
}

bool MyFieldType::char_check(const int c)
{
	return (c == chk ? true : false);
}

MyFieldType::MyFieldType(const int x) : chk(x) { }

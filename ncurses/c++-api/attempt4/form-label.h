

#pragma once

#include <cursesf.h>

class FormLabel : public NCursesFormField {
  public:
	FormLabel(const char* title, int row, int col);
};

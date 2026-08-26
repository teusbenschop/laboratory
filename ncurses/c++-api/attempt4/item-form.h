

#pragma once

#include <cursesm.h>

class FormItem : public NCursesMenuItem {
  public:
	explicit FormItem(const char* name);
	bool action() override;
};


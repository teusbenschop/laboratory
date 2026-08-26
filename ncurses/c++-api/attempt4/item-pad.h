

#pragma once

#include <cursesm.h>

class PadItem : public NCursesMenuItem {
  public:
	explicit PadItem(const char* name);
	bool action() override;
};


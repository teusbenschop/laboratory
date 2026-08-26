

#pragma once
#include <cursesm.h>

class PassiveItem : public NCursesMenuItem {
  public:
	explicit PassiveItem(const char* text);
};



#pragma once
#include <cursesm.h>

class ScanItem : public NCursesMenuItem {
  public:
	explicit ScanItem(const char* name);
	bool action() override;
};

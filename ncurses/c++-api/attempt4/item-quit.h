

#pragma once

#include <cursesm.h>

class QuitItem : public NCursesMenuItem {
  public:
	QuitItem();
	bool action() override;
};

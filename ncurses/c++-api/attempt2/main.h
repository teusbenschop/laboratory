
#pragma once

#include <cursesapp.h>
#include <cursesm.h>
#include <vector>


class MenuItems
{
protected:
	std::vector<NCursesMenuItem *> m_item_list{};
	~MenuItems();
};



class UmsConfigureApp : public NCursesApplication {
  public:
	explicit UmsConfigureApp(bool enable_colors);
	void handleArgs(int, char* []) override;
  protected:
	int run() override;
	[[nodiscard]] int titlesize() const override;
	void title() override;
  private:
};

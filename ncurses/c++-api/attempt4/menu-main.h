

#pragma once

#include <cursesm.h>
#include "lib/userdata.h"

class MainMenu : public NCursesMenu {
  public:
	MainMenu();
	MainMenu(const MainMenu& rhs);
	~MainMenu() override;
	MainMenu& operator=(const MainMenu& rhs);
	void On_Menu_Init() override;
	void On_Menu_Termination() override;
	void On_Item_Init(NCursesMenuItem& item) override;
	void On_Item_Termination(NCursesMenuItem& item) override;
  private:
	NCursesPanel* m_panel {nullptr};
	NCursesMenuItem** m_items {nullptr};
	trv::ums::configure::shared::UserData* m_user_data {nullptr};
	static constexpr auto n_items {7};
};


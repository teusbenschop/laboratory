

#include "menu-main.h"
#include <cursesm.h>
#include "item-form.h"
#include "item-pad.h"
#include "item-passive.h"
#include "item-quit.h"
#include "item-scan.h"
#include "panel-item.hpp"
#include "lib/userdata.h"


MainMenu::MainMenu() : NCursesMenu(n_items + 2, 9, (lines() - 10) / 2, (cols() - 10) / 2)
{
	m_user_data = new trv::ums::configure::shared::UserData(1);
	m_items = new NCursesMenuItem *[1 + n_items];
	m_items[0] = new PassiveItem("One");
	m_items[1] = new PassiveItem("Two");
	m_items[2] = new PanelItem("Window", m_user_data);
	m_items[3] = new FormItem("Form");
	m_items[4] = new PadItem("Pad");
	m_items[5] = new ScanItem("Scan");
	m_items[6] = new QuitItem();
	m_items[7] = new NCursesMenuItem(); // Terminating empty item.

	InitMenu(m_items, TRUE, TRUE);

	m_panel = new NCursesPanel(1, n_items, LINES - 1, 1);
	NCursesMenu::boldframe("Demo", "Window");
	m_panel->show();
}

MainMenu& MainMenu::operator=(const MainMenu& rhs)
{
	if (this != &rhs)
		*this = rhs;
	return *this;
}

MainMenu::MainMenu(const MainMenu& rhs)
	: NCursesMenu(rhs)
{
}

MainMenu::~MainMenu()
{
	m_panel->hide();
	delete m_panel;
	delete m_user_data;
}

void MainMenu::On_Menu_Init()
{
	const NCursesWindow standard_window(stdscr);
	m_panel->move(0, 0);
	m_panel->clrtoeol();
	for (int i = 1; i <= count(); i++)
		m_panel->addch('0' + i);
	m_panel->bkgd(standard_window.getbkgd());
	refresh();
}

void MainMenu::On_Menu_Termination()
{
	m_panel->move(0, 0);
	m_panel->clrtoeol();
	refresh();
}

void MainMenu::On_Item_Init(NCursesMenuItem& item)
{
	m_panel->move(0, item.index());
	m_panel->attron(A_REVERSE);
	m_panel->printw("%1d", 1 + item.index());
	m_panel->attroff(A_REVERSE);
	refresh();
}

void MainMenu::On_Item_Termination(NCursesMenuItem& item)
{
	m_panel->move(0, item.index());
	m_panel->attroff(A_REVERSE);
	m_panel->printw("%1d", 1 + item.index());
	refresh();
}

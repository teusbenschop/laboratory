

#pragma once

#include <cursesm.h>
#include "panels-demo.h"

namespace trv::ums::configure::shared {
class UserData;
}

template<typename T>
class PanelItem : public NCursesUserItem<trv::ums::configure::shared::UserData> {
  public:
	PanelItem(const char* p_name, const T* p_UserData);
	~PanelItem() override = default;
	bool action() override;
};

template<typename T> PanelItem<T>::PanelItem(const char* p_name, const T* p_UserData)
	: NCursesUserItem<T>(p_name, static_cast<const char*>(nullptr), p_UserData) { }

template<typename T>
bool PanelItem<T>::action()
{
	[[maybe_unused]] const int dummy = NCursesUserItem<T>::UserData()->value();
	panels_demo();
	return FALSE;
}

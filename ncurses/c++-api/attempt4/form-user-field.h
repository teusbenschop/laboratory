

#pragma once

#include <cursesf.h>

class MyFieldType : public UserDefinedFieldType {
  public:
	explicit MyFieldType(int x);
  protected:
	bool field_check(NCursesFormField& f) override;
	bool char_check(int c) override;
  private:
	int chk{};
};




#pragma once

#include <cursesf.h>
#include "form-user-field.h"


class FormDemo : public NCursesForm {
  public:
	FormDemo();
	explicit FormDemo(const FormDemo& rhs);
	~FormDemo() override;
	FormDemo& operator=(const FormDemo& rhs);

  private:
	NCursesFormField** m_form_field;
	MyFieldType* m_my_field;
	Integer_Field* m_integer_field;
	Enumeration_Field* m_enumeration_field;
};


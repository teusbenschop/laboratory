

#include "form-demo.h"
#include <cursesapp.h>
#include <cursesf.h>
#include <cursesm.h>
#include "form-label.h"


static const char* weekdays[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday", nullptr
};


FormDemo::FormDemo()
	: NCursesForm(13, 51, (lines() - 15) / 2, (cols() - 53) / 2),
	  m_form_field(nullptr),
	  m_my_field(nullptr),
	  m_integer_field(nullptr),
	  m_enumeration_field(nullptr)
{
	m_form_field = new NCursesFormField *[10];
	m_my_field = new MyFieldType('X');
	m_integer_field = new Integer_Field(0, 1, 10);
	m_enumeration_field = new Enumeration_Field(weekdays);

	m_form_field[0] = new FormLabel("Demo Entry Form", 0, 16);
	m_form_field[1] = new FormLabel("Weekday Enum", 2, 1);
	m_form_field[2] = new FormLabel("Number(1-10)", 2, 21);
	m_form_field[3] = new FormLabel("Only 'X'", 2, 35);
	m_form_field[4] = new FormLabel("Multiline Field (Dynamic and Scrollable)", 5, 1);
	m_form_field[5] = new NCursesFormField(1, 18, 3, 1);
	m_form_field[6] = new NCursesFormField(1, 12, 3, 21);
	m_form_field[7] = new NCursesFormField(1, 12, 3, 35);
	m_form_field[8] = new NCursesFormField(4, 46, 6, 1, 2);
	m_form_field[9] = new NCursesFormField();

	InitForm(m_form_field, true, true);
	NCursesForm::boldframe();

	m_form_field[5]->set_fieldtype(*m_enumeration_field);
	m_form_field[6]->set_fieldtype(*m_integer_field);

	m_form_field[7]->set_fieldtype(*m_my_field);
	m_form_field[7]->set_maximum_growth(20); // max. 20 characters
	m_form_field[7]->options_off(O_STATIC); // make field dynamic

	m_form_field[8]->set_maximum_growth(10); // max. 10 lines
	m_form_field[8]->options_off(O_STATIC); // make field dynamic
}

FormDemo::FormDemo(const FormDemo& rhs)
	: NCursesForm(rhs), m_form_field(nullptr), m_my_field(nullptr), m_integer_field(nullptr), m_enumeration_field(nullptr)
{
}

FormDemo::~FormDemo()
{
	delete m_my_field;
	delete m_integer_field;
	delete m_enumeration_field;
}

FormDemo& FormDemo::operator=(const FormDemo& rhs)
{
	if (this != &rhs)
		*this = rhs;
	return *this;
}

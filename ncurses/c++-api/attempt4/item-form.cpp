

#include "item-form.h"
#include <cursesm.h>
#include <cursslk.h>
#include <cursesapp.h>
#include "form-demo.h"

FormItem::FormItem(const char* name) : NCursesMenuItem(name) { }

bool FormItem::action()
{
	// Demo of the soft label key set for function keys at the screen bottom.
	Soft_Label_Key_Set function_keys_labels;
	for (int i = 1; i <= function_keys_labels.labels(); ++i) {
		function_keys_labels[i] = "Fn"; // Text
		function_keys_labels[i] = Soft_Label_Key_Set::Soft_Label_Key::Center; // Justification
	}
	NCursesApplication::getApplication()->push(function_keys_labels);

	FormDemo form_demo;
	form_demo();

	NCursesApplication::getApplication()->pop();
	return FALSE;
}


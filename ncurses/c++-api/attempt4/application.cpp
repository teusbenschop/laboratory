

#include "application.h"
#include <cursesapp.h>
#include <cursesf.h>
#include <cursesm.h>

#include "menu-main.h"

// Pass true to enable color support, false to disable it
Application::Application(const bool enable_colors) : NCursesApplication(enable_colors)
{
}



// Parse the commandline arguments.
void Application::handleArgs([[maybe_unused]] const int argc, [[maybe_unused]] char* argv[])
{

}


void Application::init_labels(Soft_Label_Key_Set& soft_label_key_set) const
{
	for (int i = 1; i <= soft_label_key_set.labels(); i++) {
		char buf[8];
		assert(i < 100);
		soft_label_key_set[i] = buf; // Text
		soft_label_key_set[i] = Soft_Label_Key_Set::Soft_Label_Key::Left; // Justification
	}
}

// The number of lines for the title window.
// A positive number causes ncurses++ to initialize the titleWindow pointer.
int Application::titlesize() const
{
	return 1;
}


// This overriding function sets the title for the app.
void Application::title()
{
	constexpr auto title_text = "UMS Configure";
	const auto title_len = static_cast<int>(strlen(title_text));
	getTitleWindow()->bkgd(screen_titles());
	getTitleWindow()->addstr(0, (getTitleWindow()->cols() - title_len) / 2, title_text);
	getTitleWindow()->noutrefresh();
}


Soft_Label_Key_Set::Label_Layout Application::useSLKs() const
{
	return Soft_Label_Key_Set::PC_Style_With_Index;
}


// Main business logic execution loop.
int Application::run()
{
	try {
		MainMenu my_menu;
		my_menu();
		return EXIT_SUCCESS;
	}
	catch (const NCursesException& e) {
		std::cerr << "error " << e.errorno << " " << e.message << std::endl;
	}
	// The ncurses++ wrapper may throw pointers.
	catch (const NCursesException* const e) { // NOLINT(*-throw-by-value-catch-by-reference)
		std::cerr << "error " << e->errorno << " " << e->message << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "error " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "general error" << std::endl;
	}
	return EXIT_FAILURE;
}



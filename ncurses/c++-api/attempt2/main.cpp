#include "main.h"
#include <vector>
#include <cstring>
#include <cursesapp.h>
#include <cursesm.h>
#include <iostream>


MenuItems::~MenuItems() {
	using cit = std::vector<NCursesMenuItem*>::const_iterator;
	for (cit i = m_item_list.cbegin(); i != m_item_list.cend(); ++i) {
		delete *i;
	}
}



// Using Base-from-Member idiom to destroy items after menu.
class Menu : private MenuItems, public NCursesMenu
{
public:
    explicit Menu(const std::vector<std::string> &in)
        : NCursesMenu(lines() - 6, cols() - 6, 3, 3), accepted(false), items(in)
    {

    	// 1. First build the item pointer array
    	m_item_list.reserve(items.size() + 1U);
    	for (const auto& str : items) {
    		m_item_list.push_back(new NCursesMenuItem(str.c_str()));
    	}
    	m_item_list.push_back(new NCursesMenuItem()); // Null terminator required by ncurses

    	// 2. Initialize the underlying C MENU* structure FIRST
    	InitMenu(&m_item_list[0], false, false);

    	// 3. Configure menu properties AFTER InitMenu
    	set_format(lines() - 4, 1);  // Ensure format fits within window bounds
    	set_mark("* ");
    	options_off(O_ONEVALUE);     // Enable multi-selection
    }

    std::vector<std::string> getSelection()
    {
        std::vector<std::string> selection;

        // Empty selection if choice wasn't confirmed.
        if (!accepted) {
            return selection;
        }

        // Use selected items.
    	for (const auto* item : m_item_list) {
    		if (item && item->name() && item->value()) {
    			selection.push_back(item->name());
    		}
    	}


	        // Or just the current item.
	        // if (selection.empty()) {
	        //     selection.push_back(current_item()->name());
	        // }

	        return selection;
	    }

	    int virtualize(int c) override
	    {
	        switch (c) {
	            // Down.
	            case 'j':  return REQ_DOWN_ITEM;
	            // Up.
	            case 'k':  return REQ_UP_ITEM;
	            // Toggle item and don't move cursor.
	            case ' ':  return REQ_TOGGLE_ITEM;
	            case 't':  return REQ_TOGGLE_ITEM;
	            // Toggle item and move cursor down.
	            case 'T':  current_item()->set_value(!current_item()->value());
	                       return REQ_DOWN_ITEM;

	            // Quit accepting the selection.
	            case '\n': accepted = true;
	                       return MAX_COMMAND + 1;
	            // Cancel.
	            case 'q':  return MAX_COMMAND + 1;

	            // Pass the rest of the keys to predefined handler.
	            default: return NCursesMenu::virtualize(c);
	        }
	    }

	private:
	    bool accepted;
	    const std::vector<std::string> items;
	};





	// Pass true to enable color support, false to disable it
	UmsConfigureApp::UmsConfigureApp(const bool enable_colors)
		: NCursesApplication(enable_colors)
	{
	}


	// Parse arguments. The virtual base class method does nothing: Override it for custom parsing.
	void UmsConfigureApp::handleArgs([[maybe_unused]] const int argc, [[maybe_unused]] char* argv[]) {}


	// Main business logic execution loop.
	int UmsConfigureApp::run()
	{
		// Remarks about the root window:
		// * is a member of the NCursesApplication
		// * is a raw pointer to a live NCursesWindow object
		// * is owned by the NCursesApplication
		// * ownership is by convention, not visible in code (through e.g. some smart pointer)
		// * encapsulates the C-API stdscr (the standard screen).
		assert(Root_Window && "Error: Undefined Root_Window");

		Root_Window->box();

		// Refresh the terminal surface to show changes,
		Root_Window->refresh();

		const std::vector<std::string> in {"aaaaaaaa", "bbbbbbbb", "cccccccc"};
		Menu m(in);

		// const auto print_at_center = [this](const char* message, const uint8_t line_number) {
		// 	const auto start_y = Root_Window->height() / 2 + line_number;
		// 	const auto start_x = (Root_Window->width() - static_cast<int>(strlen(message))) / 2;
		// 	Root_Window->addstr(start_y, start_x, message);
		// };
		// print_at_center("Hello world", 0);
		// print_at_center("Press key to quit", 1);

	// Execute the main menu input handler loop until virtualize() returns MAX_COMMAND + 1
	m();

	// Wait for input. Change it to a loop to handle continual input.
//	Root_Window->getch();

	return EXIT_SUCCESS;
}

int UmsConfigureApp::titlesize() const
{
	// The number of lines for the title window.
	// A positive number causes the wrapper to initialize the titleWindow pointer.
	return 1;
}

// Write the title for the window. It writes the title above the border, rather than in the border.
void UmsConfigureApp::title()
{
	// Take some standard color.
	titleWindow->bkgd(screen_titles());
	// Remove any previous title (which is not there on startup).
	titleWindow->erase();
	// Write title in center of window.
	constexpr auto title {"Hello world app title"};
	constexpr int start_y {0};
	const auto start_x = static_cast<int>((titleWindow->width() - strlen(title)) / 2);
	titleWindow->addstr(start_y, start_x, title);
	// Queue refresh operation - queueing this gives no flickering.
	titleWindow->noutrefresh();
}


int main(const int argc, char* argv[])
{
	// Before initializing ncurses, set the locale.
	// The library defaults to the "C" locale, which supports 7-bit ASCII characters.
	// Enable UTF-8 for better character display instead of garbled text or question marks.
	std::setlocale(LC_ALL, "");

	try {
		constexpr bool want_colors {true};
		UmsConfigureApp app(want_colors);
		app.handleArgs(argc, argv);
		// The call operator initializes stuff including the root window and then executes NCursesApplication::run().
		// Quirk: Calling app.run() therefore gives segmentation fault.
		// Work-around: Do all initialization in the overriding ::run method.
		const int result = app();
		return result;
	}
	catch (const NCursesException& e) {
		std::cerr << "error " << e.errorno << " " << e.message << std::endl;
	}
	catch (const NCursesException* e) { // NOLINT(*-throw-by-value-catch-by-reference)
		std::cerr << "error " << e->errorno << " " << e->message << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "error " << e.what() << std::endl;
	}
	return EXIT_FAILURE;
}

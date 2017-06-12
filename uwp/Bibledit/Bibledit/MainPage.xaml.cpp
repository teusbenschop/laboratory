//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "utilities.h"


using namespace Bibledit;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
	auto uri = ref new Uri (L"http://bibledit.org:8080");
	webview->Navigate (uri);
}

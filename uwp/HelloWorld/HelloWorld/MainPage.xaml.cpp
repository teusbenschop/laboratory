//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace HelloWorld;

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
}


void HelloWorld::MainPage::TextBlock_SelectionChanged (Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void HelloWorld::MainPage::nameInput_TextChanged (Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{

}


void HelloWorld::MainPage::inputButton_Click (Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	greetingOutput->Text = "Hello, " + nameInput->Text + "!";
}

//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace HelloWorld
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void TextBlock_SelectionChanged (Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void nameInput_TextChanged (Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void inputButton_Click (Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}

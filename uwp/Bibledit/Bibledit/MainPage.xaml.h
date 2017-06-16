//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;

namespace Bibledit
{
	public ref class MainPage sealed
	{
	public:
		MainPage();
	private:
		~MainPage ();
		MainPage^ rootPage;
	};

	public ref class ListenerContext sealed
	{
	public:
		ListenerContext (MainPage^ rootPage, StreamSocketListener^ listener);
		ListenerContext (StreamSocketListener^ listener);
		ListenerContext ();
		void OnConnection (StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object);

	private:
		MainPage^ rootPage;
		StreamSocketListener^ listener;

		~ListenerContext ();
		void ReceiveStringLoop (DataReader^ reader, StreamSocket^ socket);
	};
}

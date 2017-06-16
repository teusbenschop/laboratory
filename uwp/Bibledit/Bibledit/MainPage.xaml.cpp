//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "utilities.h"
#include <ppltasks.h>


using namespace Bibledit;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace std;


MainPage::MainPage()
{
	InitializeComponent();
	auto uri = ref new Uri (L"http://bibledit.org:2000");
	webview->Navigate (uri);
	rootPage = this;
	new thread (listener_thread);
}


MainPage::~MainPage ()
{
	UtilityLogMessage ("This will never be called: Destroy MainPage object");
}


ListenerContext::ListenerContext (MainPage^ rootPage, StreamSocketListener^ listener)
{
	this->rootPage = rootPage;
	this->listener = listener;
}


ListenerContext::ListenerContext (StreamSocketListener^ listener)
{
	this->listener = listener;
}


ListenerContext::ListenerContext ()
{
}


ListenerContext::~ListenerContext ()
{
	// The listener can be closed in two ways:
	//  - explicit: by using delete operator (the listener is closed even if there are outstanding references to it).
	//  - implicit: by removing last reference to it (i.e. falling out-of-scope).
	// In this case this is the last reference to the listener so both will yield the same result.
	//delete listener;
	//listener = nullptr;
	UtilityLogMessage ("Destroy ListenerContext.");
}


void ListenerContext::OnConnection (StreamSocketListener^ listener,	StreamSocketListenerConnectionReceivedEventArgs^ object)
{
	UtilityLogMessage ("ListenerContext::OnConnection started");

	DataReader^ reader = ref new DataReader (object->Socket->InputStream);

	// Start a receive loop.
	ReceiveStringLoop (reader, object->Socket);
}


void ListenerContext::ReceiveStringLoop (DataReader^ reader, StreamSocket^ socket)
{
	// Read first 4 bytes (length of the subsequent string).
	create_task (reader->LoadAsync (sizeof (UINT32))).then ([this, reader, socket](unsigned int size)
	{
		if (size < sizeof (UINT32))
		{
			// The underlying socket was closed before we were able to read the whole data.
			cancel_current_task ();
		}

		unsigned int stringLength = reader->ReadUInt32 ();
		return create_task (reader->LoadAsync (stringLength)).then (
			[this, reader, stringLength](unsigned int actualStringLength)
		{
			if (actualStringLength != stringLength)
			{
				// The underlying socket was closed before we were able to read the whole data.
				cancel_current_task ();
			}

			// Display the string on the screen. This thread is invoked on non-UI thread, so we need to marshal the 
			// call back to the UI thread.
			UtilityLogMessage ("Received data: \"" + reader->ReadString (actualStringLength) + "\"");
		});
	}).then ([this, reader, socket](task<void> previousTask)
	{
		try
		{
			// Try getting all exceptions from the continuation chain above this point.
			previousTask.get ();

			// Everything went ok, so try to receive another string. The receive will continue until the stream is
			// broken (i.e. peer closed the socket).
			ReceiveStringLoop (reader, socket);
		}
		catch (Exception^ exception)
		{
			UtilityLogMessage ("Read stream failed with error: " + exception->Message);

			// Explicitly close the socket.
			delete socket;
		}
		catch (task_canceled&)
		{
			// Do not print anything here - this will usually happen because user closed the client socket.

			// Explicitly close the socket.
			delete socket;
		}
	});
}

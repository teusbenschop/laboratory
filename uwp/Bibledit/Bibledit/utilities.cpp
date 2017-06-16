#include "utilities.h"
#include <string>
#include <windows.h>
#include <ppltasks.h>


#include "App.xaml.h"
#include "MainPage.xaml.h"
#include <ppltasks.h>


using namespace Bibledit;


using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace std;


void UtilityLogMessage (Object^ parameter)
{
	auto parameterString = parameter->ToString ();
	auto formattedText = L"UtilityLogMessage: " +  wstring (parameterString->Data ()) + L"\r\n";
	OutputDebugString (formattedText.c_str ());
}


ListenerContext^ listenerContext;
StreamSocketListener^ listener;


void listener_thread ()
{
	UtilityLogMessage ("Listener thread started");

	HostName^ localhost = ref new HostName ("localhost");

	listener = ref new StreamSocketListener ();

	UtilityLogMessage (localhost->CanonicalName);

	listenerContext = ref new ListenerContext (listener);

	listener->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^> (listenerContext, &ListenerContext::OnConnection);

	// If necessary, tweak the listener's control options before carrying out the bind operation.
	// These options will be automatically applied to the connected StreamSockets resulting from
	// incoming connections (i.e., those passed as arguments to the ConnectionReceived event handler).
	// Refer to the StreamSocketListenerControl class' MSDN documentation for the full list of control options.
	listener->Control->KeepAlive = false;

	auto bindTask = create_task (listener->BindEndpointAsync (localhost, "8080"));

	bindTask.then ([localhost](task<void> previousTask) {
		try {
			// Try getting an exception.
			previousTask.get ();
			UtilityLogMessage ("Listening on " + localhost->CanonicalName);
		}
		catch (Exception^ exception) {
			UtilityLogMessage ("Start listening failed with error: " + exception->Message);
		}
	});


	UtilityLogMessage ("Listener thread stopped");
}


/*
void listenerOnConnection (StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object)
{

	//DataReader^ reader = ref new DataReader (object->Socket->InputStream);

	// Start a receive loop.
	//ReceiveStringLoop (reader, object->Socket);
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
			NotifyUserFromAsyncThread ("Received data: \"" + reader->ReadString (actualStringLength) + "\"", NotifyType::StatusMessage);
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
			NotifyUserFromAsyncThread (
				"Read stream failed with error: " + exception->Message,
				NotifyType::ErrorMessage);

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
*/

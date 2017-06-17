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
	HostName^ localhost = ref new HostName ("localhost");

	listener = ref new StreamSocketListener ();

	listenerContext = ref new ListenerContext (listener);

	listener->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^> (listenerContext, &ListenerContext::OnConnection);

	// If necessary, tweak the listener's control options before carrying out the bind operation.
	// These options will be automatically applied to the connected StreamSockets resulting from
	// incoming connections (i.e., those passed as arguments to the ConnectionReceived event handler).
	// Refer to the StreamSocketListenerControl class' MSDN documentation for the full list of control options.
	listener->Control->KeepAlive = false;

	auto bindTask = create_task (listener->BindEndpointAsync (localhost, "9876"));

	bindTask.then ([localhost](task<void> previousTask) {
		try {
			// Try getting an exception.
			previousTask.get ();
			UtilityLogMessage ("Listening on " + localhost->CanonicalName);
			new thread (connector_thread);
		}
		catch (Exception^ exception) {
			UtilityLogMessage ("Start listening failed with error: " + exception->Message);
		}
	});
}


void connector_thread ()
{
	HostName^ hostName;
	try
	{
		hostName = ref new HostName ("localhost");
	}
	catch (InvalidArgumentException^ e)
	{
		UtilityLogMessage ("Failure connecting to invalid host name : " + e->Message);
		return;
	}

	StreamSocket^ socket = ref new StreamSocket ();

	// If necessary, tweak the socket's control options before carrying out the connect operation.
	// Refer to the StreamSocketControl class' MSDN documentation for the full list of control options.
	socket->Control->KeepAlive = false;

	// Keep the socket alive, so subsequent steps can use it.

	UtilityLogMessage ("Connecting to localhost.");

	// Connect to the server at the localhost.
	auto connectTask = create_task (socket->ConnectAsync (hostName, "9876", SocketProtectionLevel::PlainSocket));
	connectTask.then ([socket](task<void> previousTask)
	{
		try
		{
			// Try getting all exceptions from the continuation chain above this point.
			previousTask.get ();
			UtilityLogMessage ("Connected to localhost");

			DataWriter^ writer = ref new DataWriter (socket->OutputStream);

			// Write first the length of the string a UINT32 value followed up by the string. The operation will just store the data locally.
			String^ stringToSend ("Hello: Jesus Saves. Trust Him.");
			writer->WriteUInt32 (writer->MeasureString (stringToSend));
			writer->WriteString (stringToSend);

			// Write the locally buffered data to the network.
			auto writeTask = create_task (writer->StoreAsync ());
			writeTask.then ([socket, stringToSend](task<unsigned int> writeTask)
			{
				try
				{
					// Try getting an exception.
					writeTask.get ();
					UtilityLogMessage ("Sent successfully: " + stringToSend);

					// To reuse the socket with another data writer, 
					// the application must detach the stream from the current writer before deleting it. 
					// This is added for completeness, as this closes the socket in the very next block.
					//writer->DetachStream ();
					//delete writer;
					//writer = nullptr;

					// StreamSocket.Close() is exposed through the 'delete' keyword in C++/CX.
					// The line below explicitly closes the socket.
					delete socket;
					//socket = nullptr;

					// StreamSocketListener.Close() is exposed through the 'delete' keyword in C++/CX.
					// The line below explicitly closes the listener.
					delete listener;
					//listener = nullptr;


				}
				catch (Exception^ exception)
				{
					UtilityLogMessage ("Send failed with error: " + exception->Message);
					return;
				}
			});


		}
		catch (Exception^ exception)
		{
			UtilityLogMessage ("Connect failed with error: " + exception->Message);
			return;
		}
	});



}



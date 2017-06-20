/*
Copyright (©) 2003-2017 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "utilities.h"
#include <string>
#include <windows.h>
#include <ppltasks.h>
#include <sstream>


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
			new thread (connector_thread);
		}
		catch (Exception^ exception) {
			UtilityLogMessage ("Start listening failed with error: " + exception->Message);
		}
	});
}


void connector_thread ()
{
	return;
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


ListenerContext::ListenerContext (StreamSocketListener^ listener)
{
	this->listener = listener;
}


void ListenerContext::OnConnection (StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object)
{
	UtilityLogMessage ("OnConnection");

	StreamSocket^ socket = object->Socket;
	DataReader^ reader = ref new DataReader (socket->InputStream);

	ReceiveLine (reader, socket);


	DataWriter^ writer = ref new DataWriter (socket->OutputStream);
	// Write data. The operation will just store the data locally.
	String^ stringToSend ("<!DOCTYPE html><html><head><meta charset = \"utf-8\"></head><body><h1>Bibledit</h3></body></html>");
	writer->WriteString (stringToSend);
	// Write the locally buffered data to the network.
	auto writeTask = create_task (writer->StoreAsync ());
	writeTask.then ([socket] (task<unsigned int> writeTask)	{
		try	{
			// Try getting an exception.
			writeTask.get ();
			UtilityLogMessage ("Response sent successfully");
			delete socket;
		}
		catch (Exception^ exception) {
			UtilityLogMessage ("Send failed with error: " + exception->Message);
		}
	});


	
	// Explicitly close the socket.
	//delete socket;
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

		unsigned int stringLength = reader->ReadUInt32 (); // Todo fix this, it does the wrong things, reading an int.
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


void ListenerContext::ReceiveStringLoop2 (DataReader^ reader, StreamSocket^ socket)
{
}


void ListenerContext::ReceiveLine (DataReader^ reader, StreamSocket^ socket)
{
	// The line to read.
	string line;
	//  Keep reading till the end of the line.
	bool endline = false;
	while (!endline) {
		// Read one byte at a time.
		auto oneByteTask = create_task (reader->LoadAsync (1));
		oneByteTask.then ([reader, &line, &endline](unsigned int size)
		{
			byte byte = reader->ReadByte ();
			// The WebView sends a carriage return and then a new line in the request headers.
			if (byte == '\r');
			else if (byte == '\n') endline = true;
			else {
				// Append this character to the line.
				line += byte;
			}
		});
	}

	wstring ws = wstring (line.begin (), line.end ());
	String ^ S = ref new String (ws.c_str ());
	UtilityLogMessage (S);

}

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

#pragma once


#include <thread>
#include "MainPage.xaml.h"


using namespace Bibledit;
using namespace Platform;
using namespace Windows::Networking;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking::Sockets;
using namespace std;


void UtilityLogMessage (Object^ parameter);
void listener_thread ();
void connector_thread ();


namespace Bibledit
{
	public ref class ListenerContext sealed
	{
	public:
		ListenerContext (StreamSocketListener^ listener);
		void OnConnection (StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object);
	private:
		StreamSocketListener^ listener;
		void ReceiveStringLoop (DataReader^ reader, StreamSocket^ socket);
		void ReceiveStringLoop2 (DataReader^ reader, StreamSocket^ socket);
		void ReceiveLine (DataReader^ reader, StreamSocket^ socket);
	};
}

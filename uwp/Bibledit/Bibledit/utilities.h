#pragma once


#include <thread>


using namespace Platform;
using namespace Windows::Networking::Sockets;


void UtilityLogMessage (Object^ parameter);
void listener_thread ();
void connector_thread ();



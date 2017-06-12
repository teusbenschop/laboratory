#include "utilities.h"
#include <string>
#include <windows.h>


using namespace Platform;
using namespace std;


void UtilityLogMessage (Object^ parameter)
{
	auto parameterString = parameter->ToString ();
	auto formattedText = L"UtilityLogMessage: " +  wstring (parameterString->Data ()) + L"\r\n";
	OutputDebugString (formattedText.c_str ());
}


// BSPDebugger.cpp : Defines the exported functions for the DLL application.
//

#include "./stdafx.h"
#include "BSPDebugger.h"
#include "BSPDebuggerMainWindow.h"

BEGIN_SYSTEM_MODULE()
SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBSPDebuggerMainWindow,"CBSPDebuggerMainWindow")
END_SYSTEM_MODULE()

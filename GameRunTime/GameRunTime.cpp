// GameRunTime.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "GameRunTime.h"
#include "GameRunTimeLib.h"

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, unsigned int ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif


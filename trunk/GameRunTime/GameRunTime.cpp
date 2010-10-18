// GameRunTime.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "GameRunTime.h"
#include "GameRunTimeLib.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
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


void Test()
{/*
    CConfigFile         configFile;
    CSystemModuleList   rootModules;
    CSystemObjectList   rootObjects;

    SSystemModuleInfo info;
    ISystemManager *piSystemManager=ISystemManager::GetInstance();
    ISystem *piSystem=piSystemManager->CreateSystem("Engine");

    configFile.Load("GameEngine.cfg");
    if(rootModules.Load(&configFile,"MainModules"))
    {
        rootModules.LoadModules(piSystem);
    }
    if(rootObjects.Load(&configFile,"MainObjects"))
    {
        rootObjects.LoadObjects(piSystem);
    }

    ISystemObject *piObject=NULL;
    if(piSystem->GetObject("GameManager",&piObject))
    {
        IGameManager *piManager=dynamic_cast<IGameManager*>(piObject);
        piManager->Run();
    }

    ISystemManager::DestroySystem(piSystem);*/
}

//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


// GameEngine.cpp : Defines the class behaviors for the application.
//

#include "./stdafx.h"
#include "VectorLib.h"
#include "GameEngine.h"
#include "GameGUI.h"
#include "InterfaceLeakAPI.h"

#ifdef ANDROID
#include "android_native_app_glue.h"
#include <android/sensor.h>
#include <android/log.h>
#include <android/window.h>
android_app *g_pAndroidApp=NULL;
#endif

CGameEngineApp theApp;

DECLARE_CUSTOM_WRAPPER1(CGameGUIManagerWrapper,IGameGUIManager,m_piInterface)
DECLARE_CUSTOM_WRAPPER1(CGameWindowWrapper,IGameWindow,m_piWindow)

string g_sRootFolder;
string g_sExecutableFolder;
string g_sInitialConfigFile;

CGameEngineApp::CGameEngineApp()
{
}

void CGameEngineApp::Run()
{
	CConfigFile             configFile;
	CSystemLoaderHelper     systemLoader;
	
	ISystemManager *piSystemManager=GetSystemManager();
	ISystemManagerPathControl *piPathControl=QI(ISystemManagerPathControl,piSystemManager);
	if(piPathControl)
	{
		// Add executable folder as library path
		piPathControl->AddLibraryPath(AppendPathSeparator(g_sExecutableFolder));
		
		// Add working folder as library path
		std::string sCurrentPath=AppendPathSeparator(GetWorkingFolder());
		piPathControl->AddLibraryPath(sCurrentPath);
	}

	if(g_sRootFolder!="")
	{
		RTTRACE("CGameEngineApp::Run -> Setting root folder to %s",g_sRootFolder.c_str());
		if(!SetWorkingFolder(g_sRootFolder))
		{
			RTTRACE("CGameEngineApp::Run -> Error setting root folder to %s",g_sRootFolder.c_str());
		}
	}
	if(configFile.Open(g_sInitialConfigFile))
	{
		CSystemLoaderHelper guiSystem;

		ISystem *piGameSystem=guiSystem.LoadSystem(configFile.GetRoot(),"GameGUI");
		if(piGameSystem)
		{
			CGameGUIManagerWrapper   guiManager;
			guiManager.Attach("GameGUI","GUIManager");
			if(guiManager.m_piInterface){guiManager.m_piInterface->EnterGUILoop();}
			guiManager.Detach();
			
			RTTRACE("CGameEngineApp::Run -> Exiting, destroying game system ...");

			piGameSystem->Destroy();
		
			RTTRACE("CGameEngineApp::Run -> Exiting, Game system destroyed ...");
		}
		else
		{
			RTTRACE("CGameEngineApp::Run -> Failed to create GameGUI system");
		}

		REL(piGameSystem);
	}
	else
	{
		RTTRACE("CGameEngineApp::Run -> Failed to open configuration file %s",g_sInitialConfigFile.c_str());
	}
	REL(piPathControl);


	if(piSystemManager->DebugGetRegisteredObjectCount())
	{
		RTTRACE("CGameEngineApp::Run() -> %d leaked objects!!!!",piSystemManager->DebugGetRegisteredObjectCount());
		std::map<ISystemObject *,std::string>  mObjects;
		std::map<ISystemObject *,std::string>::iterator i;
		piSystemManager->DebugGetRegisteredObjects(&mObjects);
		for(i=mObjects.begin();i!=mObjects.end();i++)
		{
			RTTRACE("CGameEngineApp::Run() -> Leaked object: %s",i->second.c_str());
		}
	} 

	REL(piSystemManager);
	
	RTTRACE("CGameEngineApp::Run -> Exit");
}

void CGameEngineApp::InterpretCommandLine(std::string sExecutableFolder,std::vector<std::string> &vParams)
{
#ifdef ANDROID
	g_sRootFolder="./";
#else
	g_sRootFolder=AppendPathSeparator(sExecutableFolder)+".." PATH_SEPARATOR ".." PATH_SEPARATOR "Resources";
#endif
	g_sInitialConfigFile="Scripts" PATH_SEPARATOR "GameGUI.cfg";
	g_sExecutableFolder=sExecutableFolder;
	
	// Si se ejecuta con un solo parametro y sin opciones presuponemos que es la forma abreviada
	// es decir GameEngine <Modulo>, por ejemplo GameEngine ScenarioEditor
	if(vParams.size()==1 && vParams[0].at(0)!='-')
	{
		g_sInitialConfigFile=vParams[0]+PATH_SEPARATOR+vParams[0]+".cfg";
	}
	else
	{
		for(unsigned int x=0;x<vParams.size();x++)
		{
			const char *pParam=vParams[x].c_str();
			if(pParam[0]=='-')
			{
				if(strcmp(pParam,"-folder")==0 && (x+1)<vParams.size())
				{
					const char *pFolder=vParams[x+1].c_str();
					g_sRootFolder=pFolder;
					x++;
				}
				else if(strcmp(pParam,"-config")==0 && (x+1)<vParams.size())
				{
					const char *pFileName=vParams[x+1].c_str();
					g_sInitialConfigFile=pFileName;
					x++;
				}
			}
		}
	}
}

#ifdef WIN32
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
 // InitMonitorization();
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
  
	char szExecutableFullPath[MAX_PATH]={0};
	GetModuleFileName(NULL,szExecutableFullPath,sizeof(szExecutableFullPath));
	std::string sFolder=GetFileFolder(szExecutableFullPath);
	
	wchar_t *pWCommand=GetCommandLineW();

	int nArgs=0;
	wchar_t **ppArgs=NULL;
	ppArgs=CommandLineToArgvW(pWCommand,&nArgs);
	std::vector<std::string> vParams;
	for(int x=1;x<nArgs;x++)
	{
		char pTemp[1024]={0};
		WideCharToMultiByte(CP_ACP,0,ppArgs[x],wcslen(ppArgs[x]),pTemp,1024,NULL,NULL);
		vParams.push_back(pTemp);
	}
	theApp.InterpretCommandLine(sFolder,vParams);
	theApp.Run();
 // DumpMonitorizationLeaks();
 // EndMonitorization();
  	return 0;
}
#elif defined(LINUX)
int main(int argc, char *argv[])
{
	std::string sFolder=GetFileFolder(argv[0]);
	std::vector<std::string> vParams;
	for(int x=1;x<argc;x++)
	{
		vParams.push_back(argv[x]);
	}
	theApp.InterpretCommandLine(sFolder,vParams);
	theApp.Run();
	return 0;
}
#elif defined(ANDROID)
void android_main(struct android_app* state) 
{
	// Make sure glue isn't stripped.
	app_dummy();
	g_pAndroidApp=state;
	if(g_pAndroidApp->activity){ANativeActivity_setWindowFlags(g_pAndroidApp->activity,AWINDOW_FLAG_FULLSCREEN,0);}
	std::string sFolder="";
	std::vector<std::string> vParams;
	theApp.InterpretCommandLine(sFolder,vParams);
	theApp.Run();
}
#endif

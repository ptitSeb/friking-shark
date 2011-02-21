// GameEngine.cpp : Defines the class behaviors for the application.
//

#include "./stdafx.h"
#include "VectorLib.h"
#include "GameEngine.h"
#include "GameGUI.h"
#include "InterfaceLeakAPI.h"

CGameEngineApp theApp;

DECLARE_CUSTOM_WRAPPER1(CGameGUIManagerWrapper,IGameGUIManager,m_piInterface)
DECLARE_CUSTOM_WRAPPER1(CGameWindowWrapper,IGameWindow,m_piWindow)

string g_sRootFolder;
string g_sExecutableFolder;
string g_sInitialConfigFile="Scripts" PATH_SEPARATOR "GameGUI.cfg";

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
		piPathControl->AddLibraryPath(g_sExecutableFolder);
		
		// Add working folder as library path
		char sCurrentPath[MAX_PATH]={0};
#ifdef WIN32
		GetCurrentDirectory(MAX_PATH,sCurrentPath);
#else
		if(!getcwd(sCurrentPath,MAX_PATH))
		{
		  sCurrentPath[0]=0;
		}
#endif
		int len=strlen(sCurrentPath);
		if(len && sCurrentPath[len-1]!=PATH_SEPARATOR_CHAR)
		{
			sCurrentPath[len]=PATH_SEPARATOR_CHAR;
			sCurrentPath[len+1]=0;
		}
		piPathControl->AddLibraryPath(sCurrentPath);
	}

	if(g_sRootFolder!="")
	{
		RTTRACE("CGameEngineApp::Run -> Setting root folder to %s",g_sRootFolder.c_str());
#ifdef WIN32
		SetCurrentDirectory(g_sRootFolder.c_str());
#else
		if(chdir(g_sRootFolder.c_str())!=0)
		{
		  RTTRACE("CGameEngineApp::Run -> Error setting root folder to %s",g_sRootFolder.c_str());
		}
#endif
	}
	if(configFile.Open(g_sInitialConfigFile))
	{
		CSystemLoaderHelper guiSystem;

		ISystem *piGameSystem=guiSystem.LoadSystem(configFile.GetRoot(),"GameGUI");
		if(piGameSystem)
		{
			CGameWindowWrapper  mainWindow;
			CGameGUIManagerWrapper   guiManager;
			guiManager.Attach("GameGUI","GUIManager");
			guiManager.m_piInterface->EnterGUILoop();
			mainWindow.Detach();
			guiManager.Detach();

			piGameSystem->Destroy();
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
}

void CGameEngineApp::InterpretCommandLine(std::string sExecutableFolder,std::vector<std::string> &vParams)
{
	g_sRootFolder=sExecutableFolder;
	g_sExecutableFolder=sExecutableFolder;

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

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
 // InitMonitorization();
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
  
	char szExecutableFullPath[MAX_PATH]={0};
	char szExecutableFolder[MAX_PATH]={0};
	char szExecutableDrive[MAX_PATH]={0};
	GetModuleFileName(NULL,szExecutableFullPath,sizeof(szExecutableFullPath));
	_splitpath(szExecutableFullPath,szExecutableDrive,szExecutableFolder,NULL,NULL);
	
	std::string sFolder;
	sFolder=szExecutableDrive;
	sFolder+=szExecutableFolder;

	wchar_t *pWCommand=GetCommandLineW();

	int nArgs=0;
	wchar_t **ppArgs=NULL;
	ppArgs=CommandLineToArgvW(pWCommand,&nArgs);
	std::vector<std::string> vParams;
	for(int x=0;x<nArgs;x++)
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
#else
int main(int argc, char *argv[])
{
	char *pTempExecutable=strdup(argv[0]);
	char *pTempFolder=strtok(pTempExecutable,"/");
	std::string sFolder=pTempFolder?pTempFolder:pTempExecutable;
	free(pTempExecutable);

	std::vector<std::string> vParams;
	for(int x=1;x<argc;x++)
	{
		vParams.push_back(argv[x]);
	}
	theApp.InterpretCommandLine(sFolder,vParams);
	theApp.Run();
 // DumpMonitorizationLeaks();
 // EndMonitorization();
  return 0;
}
#endif

#include "StdAfx.h"
#include "GameRunTimeLib.h"
#include ".\systemmodule.h"

CSystemModule::CSystemModule(void)
{
    m_hModule=NULL;
    m_piSystem=NULL;
    m_bRegistered=false;
}

CSystemModule::~CSystemModule(void)
{
}

bool CSystemModule::Init(std::string sPath,ISystem *piSystem)
{
    bool bOk=false;
    m_sPath=sPath;
    m_hModule=LoadLibrary(sPath.c_str());
    if(m_hModule==NULL)
    {
      char fileName[MAX_PATH]={0},fileExt[MAX_PATH]={0};
      _splitpath(sPath.c_str(),NULL,NULL,fileName,fileExt);
      std::set<std::string> sPaths;
      std::set<std::string>::iterator iPaths;
      ISystemManager *piSystemManager=GetSystemManager();
      ISystemManagerPathControl *piPathControl=QI(ISystemManagerPathControl,piSystemManager);
      if(piPathControl)
      {
        piPathControl->GetLibraryPaths(&sPaths);

        for(iPaths=sPaths.begin();iPaths!=sPaths.end();iPaths++)
        {
          std::string sAlternatePath=*iPaths;
          sAlternatePath+=fileName;
          sAlternatePath+=fileExt;
          m_hModule=LoadLibrary(sAlternatePath.c_str());
          if(m_hModule!=NULL){break;}
        }
      }
			REL(piSystemManager);
			REL(piPathControl);
    }
    if(m_hModule)
    {
        m_pSystemModuleRegister=(tSystemModuleRegister)GetProcAddress(m_hModule,"SystemModuleRegister");
        m_pSystemModuleUnregister=(tSystemModuleUnregister)GetProcAddress(m_hModule,"SystemModuleUnregister");
        if(m_pSystemModuleRegister!=NULL && m_pSystemModuleRegister!=NULL){bOk=true;}
    }
    if(bOk){bOk=m_bRegistered=piSystem->RegisterModule(this);}
    if(bOk){bOk=m_pSystemModuleRegister(piSystem);}
    if(bOk){m_piSystem=ADD(piSystem);}
    if(!bOk)
    {
        if(m_bRegistered){piSystem->UnregisterModule(this);m_bRegistered=false;}
        if(m_hModule){FreeLibrary(m_hModule);m_hModule=NULL;}
        m_pSystemModuleRegister=NULL;
        m_pSystemModuleUnregister=NULL;        
        m_sPath="";
    }
    return bOk;
}

void CSystemModule::Destroy()
{
    if(m_pSystemModuleUnregister){m_pSystemModuleUnregister(m_piSystem);}
    if(m_bRegistered){m_piSystem->UnregisterModule(this);m_bRegistered=false;}
    if(m_hModule){FreeLibrary(m_hModule);m_hModule=NULL;}
    REL(m_piSystem);
}

std::string CSystemModule::GetName()
{
    return m_sPath;
}

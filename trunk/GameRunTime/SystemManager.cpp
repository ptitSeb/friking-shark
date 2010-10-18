#include "StdAfx.h"
#include ".\systemmanager.h"
#include ".\system.h"

CSystemManager *CSystemManager::g_pSystemManager=NULL;

CSystemManager::CSystemManager(void)
{
}

CSystemManager::~CSystemManager(void)
{
    _ASSERTE(m_mSystems.size()==0);
}

ISystem *CSystemManager::CreateSystem(std::string sName)
{
    ISystem *piSystem=GetSystem(sName);
    if(piSystem){return piSystem;}

    CSystem *pSystem=new CSystem(this);
    if(!pSystem->Init("System",sName,NULL)){REL(pSystem);}
    return pSystem;
}

ISystem *CSystemManager::GetSystem(std::string sName)
{
    std::map<std::string,ISystem*>::iterator i;
    i=m_mSystems.find(sName);
    if(i==m_mSystems.end()){return NULL;}
    ISystem *piSystem=i->second;
    ADD(piSystem);
    return piSystem;
}

bool CSystemManager::RegisterSystem(ISystem *piSystem)
{
    std::string sName=piSystem->GetName();
    ISystem *piTempSystem=GetSystem(sName);
    if(piTempSystem){REL(piTempSystem);return false;}
    m_mSystems[sName]=piSystem;
    ADD(piSystem);
    return true;
}

void CSystemManager::UnregisterSystem(ISystem *piSystem)
{
    std::string sName=piSystem->GetName();
    std::map<std::string,ISystem*>::iterator i;
    i=m_mSystems.find(sName);
    if(i==m_mSystems.end()){return;}
    m_mSystems.erase(sName);
    REL(piSystem);
}

void CSystemManager::AddLibraryPath(std::string sPath)
{
  m_sLibraryPaths.insert(sPath);
}

void CSystemManager::RemoveLibraryPath(std::string sPath)
{
  m_sLibraryPaths.erase(sPath);
}

void CSystemManager::ClearLibraryPaths()
{
  m_sLibraryPaths.clear();
}

void CSystemManager::GetLibraryPaths(std::set<std::string> *pvPaths)
{
  (*pvPaths)=m_sLibraryPaths;
}

void CSystemManager::DebugRegisterObject(ISystemObject *piObject)
{
	m_mDebugObjects[piObject]=(std::string)"\n\tSystem:"+piObject->GetSystemName()+"\n\tClass:"+piObject->GetClass()+"\n\tName:"+piObject->GetName();
}
void CSystemManager::DebugUnregisterObject(ISystemObject *piObject)
{
	m_mDebugObjects.erase(piObject);
}
unsigned long CSystemManager::DebugGetRegisteredObjectCount()
{
	return (unsigned long)m_mDebugObjects.size();
}

void CSystemManager::DebugGetRegisteredObjects(std::map<ISystemObject *,std::string> *pObjects)
{
	*pObjects=m_mDebugObjects;
}


ISystemManager *GetSystemManager()
{
    if(!CSystemManager::g_pSystemManager)
    {
        CSystemManager::g_pSystemManager=new CSystemManager();
    }
    else
    {
        ADD(CSystemManager::g_pSystemManager);
    }
    
    return CSystemManager::g_pSystemManager;
}


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


#include "./stdafx.h"
#include "SystemManager.h"
#include "System.h"

CSystemManager *CSystemManager::g_pSystemManager=NULL;

CSystemManager::CSystemManager(void)
{
}

CSystemManager::~CSystemManager(void)
{
    RTASSERT(m_mSystems.size()==0);
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


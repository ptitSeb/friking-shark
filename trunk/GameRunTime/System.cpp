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
#include "GameRunTimeLib.h"
#include "System.h"
#include "SystemModule.h"

CSystem::CSystem(ISystemManager *piSystemManager)
{
	m_piSystemManager=ADD(piSystemManager);
    m_piSystem=NULL;
}

CSystem::~CSystem(void)
{
	REL(m_piSystemManager);
}

bool CSystem::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    if(!CSystemObjectBase::Init(sClass,sName,piSystem)){return false;}
    if(m_piSystemManager){m_piSystemManager->RegisterSystem(this);}
    return true;
}

void CSystem::DestroyAllObjects()
{
	std::map<std::string,ISystemObject*>::iterator i1;
	std::map<std::string,ISystemObject*> mTempObjects;
	for(i1=m_mObjects.begin();i1!=m_mObjects.end();i1++){mTempObjects[i1->first]=i1->second;}
	m_mObjects.clear();
	for(i1=mTempObjects.begin();i1!=mTempObjects.end();i1++)
	{
		ISystemObject *piObject=i1->second;
		piObject->Destroy();
		REL(piObject);
	}
	mTempObjects.clear();
}

void CSystem::Destroy()
{   
		DestroyAllObjects();

    std::map<std::string,ISystemModule*>::iterator i2;
    std::map<std::string,ISystemModule*> mTempModules;
    for(i2=m_mModules.begin();i2!=m_mModules.end();i2++){mTempModules[i2->first]=i2->second;}
    m_mModules.clear();

    for(i2=mTempModules.begin();i2!=mTempModules.end();i2++){ISystemModule *piModule=i2->second;piModule->Destroy();REL(piModule);}
    mTempModules.clear();

    RTASSERT(m_mClasses.size()==0);

    CSystemObjectBase::Destroy();
    if(m_piSystemManager){m_piSystemManager->UnregisterSystem(this);}
}

bool CSystem::RegisterClassFactory(ISystemClass *piClass)
{
    std::string sName=piClass->GetName();
    std::map<std::string,ISystemClass *>::iterator i;
    i=m_mClasses.find(sName);
    if(i!=m_mClasses.end()){return false;}
    ADD(piClass);
    m_mClasses[sName]=piClass;
    return true;
}

void CSystem::UnregisterClassFactory(ISystemClass *piClass)
{
    std::string sName=piClass->GetName();
    std::map<std::string,ISystemClass *>::iterator i;
    i=m_mClasses.find(sName);
    if(i==m_mClasses.end()){return;}
    m_mClasses.erase(sName);
    REL(piClass);
}

bool CSystem::LoadModule(std::string sPath,ISystemModule **ppiModule)
{
    if(GetModule(sPath,ppiModule)){return true;}
    CSystemModule *pModule=new CSystemModule;
    if(!pModule->Init(sPath,this))
    {
        REL(pModule);
        return false;
    }
    (*ppiModule)=pModule;
    return true;
}

bool CSystem::RegisterModule(ISystemModule *piModule)
{
    std::string sName=piModule->GetName();
    std::map<std::string,ISystemModule *>::iterator i;
    i=m_mModules.find(sName);
    if(i!=m_mModules.end()){return false;}
    ADD(piModule);
    m_mModules[sName]=piModule;
    return true;
}

void CSystem::UnregisterModule(ISystemModule *piModule)
{
    std::string sName=piModule->GetName();
    std::map<std::string,ISystemModule *>::iterator i;
    i=m_mModules.find(sName);
    if(i==m_mModules.end()){return;}
    m_mModules.erase(sName);
    REL(piModule);
}

bool CSystem::GetModule(std::string sPath,ISystemModule **ppiModule)
{
    std::map<std::string,ISystemModule *>::iterator i;
    i=m_mModules.find(sPath);
    if(i==m_mModules.end()){return false;}
    (*ppiModule)=i->second;
    ADD((*ppiModule));
    return true;
}

void CSystem::GetModules( std::vector<ISystemModule *> *pvModules )
{
	std::map<std::string,ISystemModule*>::iterator i;
	for(i=m_mModules.begin();i!=m_mModules.end();i++)
	{
		pvModules->push_back(ADD(i->second));
	}
}

bool CSystem::RegisterObject(ISystemObject *piObject)
{
    std::map<std::string,ISystemObject*>::iterator i;
    std::string sName=piObject->GetName();
    if(sName==""){return false;}

    i=m_mObjects.find(sName);
    if(i!=m_mObjects.end()){return false;}

    ADD(piObject);
    m_mObjects[sName]=piObject;
    return true;
}

void CSystem::UnregisterObject(ISystemObject *piObject)
{
    std::map<std::string,ISystemObject*>::iterator i;
    std::string sName=piObject->GetName();
    i=m_mObjects.find(sName);
    if(i==m_mObjects.end()){return;}
    m_mObjects.erase(i);
    REL(piObject);
}

bool CSystem::GetObject(std::string sName,ISystemObject **ppiObject)
{
    std::map<std::string,ISystemObject*>::iterator i;
    i=m_mObjects.find(sName);
    if(i==m_mObjects.end()){return false;}
    *ppiObject=i->second;
    ADD(*ppiObject);
    return true;
}

void CSystem::GetObjects( std::vector<ISystemObject *> *pvObjects )
{
	std::map<std::string,ISystemObject*>::iterator i;
	for(i=m_mObjects.begin();i!=m_mObjects.end();i++)
	{
		pvObjects->push_back(ADD(i->second));
	}
}

bool CSystem::CreateObject(std::string sClass,ISystemObject **ppiObject)
{
    std::map<std::string,ISystemClass *>::iterator i;
    i=m_mClasses.find(sClass);
    if(i==m_mClasses.end()){return false;}
    ISystemClass *piClass=i->second;
    piClass->CreateInstance(ppiObject);
    if((*ppiObject))
    {
        bool bInitialized=(*ppiObject)->Init(sClass,"",this);
        if(!bInitialized)
        {
            REL((*ppiObject));
            return false;
        }
        return true;
    }
    return false;
}

bool CSystem::CreateObject(std::string sClass,std::string sName,ISystemObject **ppiObject)
{
    if(GetObject(sName,ppiObject)){return true;}

    std::map<std::string,ISystemClass *>::iterator i;
    i=m_mClasses.find(sClass);
    if(i==m_mClasses.end()){return false;}
    ISystemClass *piClass=i->second;
    piClass->CreateInstance(ppiObject);
    if((*ppiObject))
    {
        bool bInitialized=(*ppiObject)->Init(sClass,sName,this);
        if(!bInitialized)
        {
            REL((*ppiObject));
            return false;
        }
        return true;
    }
    return false;
}

bool CSystem::GetClass(std::string sName,ISystemClass **ppiClass)
{
	std::map<std::string,ISystemClass*>::iterator i;
	i=m_mClasses.find(sName);
	if(i==m_mClasses.end()){return false;}
	*ppiClass=i->second;
	ADD(*ppiClass);
	return true;
}

void CSystem::GetClasses( std::vector<ISystemClass *> *pvClasses )
{
	std::map<std::string,ISystemClass*>::iterator i;
	for(i=m_mClasses.begin();i!=m_mClasses.end();i++)
	{
		pvClasses->push_back(ADD(i->second));
	}
}

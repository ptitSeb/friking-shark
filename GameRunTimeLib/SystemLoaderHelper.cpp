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
#include "SystemLoaderHelper.h"

CSystemLoaderHelper::CSystemLoaderHelper(void)
{
}

CSystemLoaderHelper::~CSystemLoaderHelper(void)
{
}

ISystem *CSystemLoaderHelper::LoadSystem(std::string sFile,std::string sSystemName)
{
	CConfigFile configFile;
	if(configFile.Open(sFile))
	{
		ISystem *piSystem=LoadSystem(configFile.GetRoot(),sSystemName);
		if(piSystem==NULL){RTTRACE("CSystemLoaderHelper::LoadSystem -> Failed to load system %s from file %s",sSystemName.c_str(),sFile.c_str());}
		return piSystem;
	}
	else
	{
		RTTRACE("CSystemLoaderHelper::LoadSystem -> Failed to open config file %s to load system %s",sFile.c_str(),sSystemName.c_str());
	}
	return NULL;
}

ISystem *CSystemLoaderHelper::LoadSystem(ISystemPersistencyNode *piParent,std::string sSystemName)
{
    bool bResult=true;
    ISystemPersistencyNode  *piNode=piParent->GetNode(sSystemName);
    ISystem                 *piSystem=NULL;

	if(piNode==NULL){return NULL;}

    ISystemManager *piSystemManager=GetSystemManager();
    piSystem=piSystemManager->CreateSystem(sSystemName);
    if(piSystem!=NULL)
		{
			if(m_Modules.PersistencyLoad(piNode))
			{
				unsigned x=0;
				for(x=0;x<m_Modules.m_dModules.size();x++)
				{
					ISystemModule *piModule=NULL;
					if(!piSystem->LoadModule(m_Modules.m_dModules[x].sPath,&piModule))
					{
						bResult=false;
						RTTRACE("CSystemLoaderHelper::LoadSystem -> Failed to load Module %s in System %s",m_Modules.m_dModules[x].sPath.c_str(),sSystemName.c_str());
					}
					REL(piModule);
				}
				// Carga de objetos. Como es un CDelayedUnserializeObjectWrapper
				// El objeto solo se instancia, Realmente se deserializa en UnserializeNow
				// Se hace esto para poder resolver referencias entre objetos con nombre de un sistema.
				// Si no se hace, el attach funciona o no dependendiendo del orden en que se guardan
				
				PersistencyLoad(piNode);
				for(x=0;x<m_dObjects.size();x++)
				{
					m_dObjects[x].UnserializeNow();
					m_dObjects[x].Detach();
				}
				m_dObjects.clear();
			}
			else
			{
					piSystemManager->UnregisterSystem(piSystem);
			}
		}
    REL(piSystemManager);
    if(!bResult)
	{
		if(piSystem){piSystem->Destroy();}
		REL(piSystem);
	}
    return piSystem;
}


bool CSystemSaverHelper::SaveSystemNamedObjects(std::string sFile,std::string sSystemName)
{
	CConfigFile configFile;
	if(!configFile.Open(sFile))
	{
		RTTRACE("CSystemSaverHelper::SaveSystemNamedObjects -> Failed to open config file %s to save system %s objects",sSystemName.c_str());
	}	
	
	ISystemPersistencyNode  *piNode=configFile.GetRoot()->AddNode(sSystemName);
	bool bOk=(piNode!=NULL);
	if(bOk){bOk=PersistencyLoad(piNode);}
	if(bOk){bOk=SaveSystemNamedObjects(piNode,sSystemName);}
	if(bOk){bOk=configFile.Save(sFile);}	
	return bOk;
}

bool CSystemSaverHelper::SaveSystemNamedObjects(ISystemPersistencyNode *piNode,std::string sSystemName)
{
	std::vector<ISystemObject *> vObjects;
	GetSystemObjects(sSystemName,&vObjects);
	for(unsigned long x=0;x<vObjects.size();x++)
	{
		ISystemObject *piObject=vObjects[x];
		CSystemObjectWrapper wrapper;
		// Aqui se usa create para que el wrapper serialice el objeto
		if(wrapper.Create(sSystemName,piObject->GetClass(),piObject->GetName()))
		{
			m_dObjects.push_back(wrapper);
			
		}
	}
	for(unsigned long x=0;x<vObjects.size();x++){ISystemObject *piObject=vObjects[x];REL(piObject);}
	// Carga de objetos.
	return PersistencySave(piNode);
}

CSystemSaverHelper::CSystemSaverHelper(){}
CSystemSaverHelper::~CSystemSaverHelper(){}


CDelayedUnserializeObjectWrapper::CDelayedUnserializeObjectWrapper()
{
	m_piNode=NULL;
}

CDelayedUnserializeObjectWrapper::~CDelayedUnserializeObjectWrapper()
{
	m_piNode=NULL;
}

bool CDelayedUnserializeObjectWrapper::UnserializeObject(ISystemPersistencyNode *piNode)
{
	m_piNode=piNode;
	return true;
}

bool CDelayedUnserializeObjectWrapper::UnserializeNow()
{
	if(m_piSerializable==NULL || m_piNode==NULL){return false;}
	if(m_piSerializable->Unserialize(m_piNode))
	{
		return true;
	}
	else
	{
		RTTRACE("CSystemObjectWrapper::Load -> Failed to unserialize System: %s, Class: %s, Object:%s",m_piObject->GetSystemName().c_str(),m_piObject->GetClass().c_str(),m_piObject->GetName().c_str());
		return false;
	}
}	

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem){return pItem->GetValueAddress()->Save(piNode,pItem->GetName())?true:false;}
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem){return pItem->GetValueAddress()->Load(piNode,pItem->GetName())?true:false;}
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem){piNode->DeleteNode(pItem->GetName());return true;}
void MRPersistencyInitialize(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem){}
void MRPersistencyFree(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem){}

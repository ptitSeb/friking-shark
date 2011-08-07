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
#include "SystemObjectWrapper.h"

CSystemObjectWrapper::CSystemObjectWrapper()
{
    m_piSerializable=NULL;
    m_piObject=NULL;
    m_bObjectMustBeDestroyed=false;
    m_bAttached=false;
}


CSystemObjectWrapper::CSystemObjectWrapper(const CSystemObjectWrapper &otherWrapper)
{
    m_piSerializable=NULL;
    m_piObject=NULL;
    m_bObjectMustBeDestroyed=false;
    if(otherWrapper.m_piObject){Attach(otherWrapper.m_piObject);}
    m_bAttached=otherWrapper.m_bAttached;
}

CSystemObjectWrapper &CSystemObjectWrapper::operator=(const CSystemObjectWrapper &otherWrapper)
{
    Attach(static_cast<ISystemUnknown*>(otherWrapper.m_piObject));
	m_bAttached=otherWrapper.m_bAttached;
    return *this;
}

CSystemObjectWrapper::~CSystemObjectWrapper()
{
    ReleaseInterfaces();
}

bool CSystemObjectWrapper::GetInterfaces(ISystemUnknown *piUnknown)
{
    m_piObject=QI(ISystemObject,piUnknown);
    if(m_piObject){m_piSerializable=QI(ISystemSerializable,piUnknown);}
    if(!m_piObject){ReleaseInterfaces();return false;}
    return true;
}

void CSystemObjectWrapper::ReleaseInterfaces()
{
    if(m_piObject && m_bObjectMustBeDestroyed)
    {
        m_piObject->Destroy();
    }
    REL(m_piObject);
    REL(m_piSerializable);
    m_bObjectMustBeDestroyed=false;
	m_bAttached=false;
}

bool CSystemObjectWrapper::Attach(std::string sSystemName,std::string sName,bool bTraceIfFailed)
{
    bool bOk=false;
	ISystemManager *piSystemManager=GetSystemManager();
    ISystem *piSystem=piSystemManager->GetSystem(sSystemName);
	if(piSystem){bOk=Attach(piSystem,sName,bTraceIfFailed);}
    REL(piSystem);
	REL(piSystemManager);
    return bOk;
}

bool CSystemObjectWrapper::Attach(ISystem *piSystem,std::string sName,bool bTraceIfFailed)
{
    bool bOk=false;
    ISystemObject *piObject=NULL;
	if(piSystem->GetObject(sName,&piObject)){bOk=Attach(piObject);}
	if(!bOk && bTraceIfFailed)
	{
		RTTRACE("CSystemObjectWrapper::Attach -> Failed to attach to  System: %s, Object:%s",piSystem->GetName().c_str(),sName.c_str());
	}
    REL(piObject);
    return bOk;
}

bool CSystemObjectWrapper::Attach(ISystemUnknown *piUnknown)
{
    Detach();
	m_bAttached=true;
    if(piUnknown){return GetInterfaces(piUnknown);}
    return true;
}

void CSystemObjectWrapper::Detach(bool bDestroyIfMust)
{
    if(!bDestroyIfMust){m_bObjectMustBeDestroyed=false;}
    ReleaseInterfaces();
	m_bAttached=false;
}

void CSystemObjectWrapper::Destroy()
{
    m_bObjectMustBeDestroyed=true;
    ReleaseInterfaces();
}

bool CSystemObjectWrapper::UnserializeObject(ISystemPersistencyNode *piNode)
{
	if(m_piSerializable==NULL){return false;}
	return m_piSerializable->Unserialize(piNode);
}

bool CSystemObjectWrapper::Load(ISystemPersistencyNode *piNode,std::string sName)
{
    if(!piNode){return false;}
	ISystemPersistencyNode *piSystemNode=piNode->GetNode("System");
	ISystemPersistencyNode *piClassNode=piNode->GetNode("Class");
	ISystemPersistencyNode *piNameNode=piNode->GetNode("Name");
	
	std::string sSystem,sClass,sObjectName;
	sSystem=piSystemNode?piSystemNode->GetValue():"";
	sClass=piClassNode?piClassNode->GetValue():"";
	sObjectName=piNameNode?piNameNode->GetValue():"";

	if( piSystemNode)
	{
		if(!piClassNode)
		{
			return Attach(sSystem,sObjectName);
		}
		else
		{
			if(Create(sSystem,sClass,sObjectName))
			{
				ISystemPersistencyNode *piChild=piNode->GetNode("Data");
				if(m_piSerializable && piChild)
				{
					ISystemPersistencyNode *piPersistencyNode=piChild;
					
					if(UnserializeObject(piPersistencyNode))
					{
						return true;
					}
					else
					{
						RTTRACE("CSystemObjectWrapper::Load -> Failed to unserialize System: %s, Class: %s, Object:%s",sSystem.c_str(),sClass.c_str(),sName.c_str());
						return false;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

bool CSystemObjectWrapper::Save(ISystemPersistencyNode *piNode,std::string sName)
{
    if(!m_piObject){return true;}

    if(!piNode){return false;}

	ISystemPersistencyNode *piSystemNode=piNode->AddNode("System");
	ISystemPersistencyNode *piClassNode=m_bAttached?NULL:piNode->AddNode("Class");
	ISystemPersistencyNode *piNameNode=piNode->AddNode("Name");

	std::string sObjectSystem=m_piObject->GetSystemName();
	std::string sObjectClass=m_piObject->GetClass();
	std::string sObjectName=m_piObject->GetName();
	if(piSystemNode){piSystemNode->SetValue(sObjectSystem);}
	if(piClassNode){piClassNode->SetValue(sObjectClass);}
	if(piNameNode){piNameNode->SetValue(sObjectName);}

	if( m_bAttached && piSystemNode &&  piNameNode)
	{
		return true;
	}
	if( piSystemNode && piClassNode &&  piNameNode)
    {
        if(m_piSerializable)
        {
            ISystemPersistencyNode *piChild=piNode->AddNode("Data");
            if(piChild)
			{
				bool bOk=m_piSerializable->Serialize(piChild);
				if(!bOk)
				{
					RTTRACE("CSystemObjectWrapper::Save -> Failed to serialize System: %s, Class: %s, Object:%s",m_piObject->GetSystemName().c_str(),m_piObject->GetClass().c_str(),m_piObject->GetName().c_str());
				}
				return bOk;
			}
        }
        else
        {
            return true;
        }
    }
    return false;
}

bool CSystemObjectWrapper::Create(std::string sSystem,std::string sClass,std::string sName)
{
    ReleaseInterfaces();

    bool            bOk=false;
    ISystemObject   *piObject=NULL;
    ISystem         *piSystem=NULL;
    ISystemManager  *piManager=GetSystemManager();

    if(piManager){piSystem=piManager->GetSystem(sSystem);}
    if(piSystem){bOk=Create(piSystem,sClass,sName);}
    if(piObject){bOk=GetInterfaces(piObject);}
	if(!bOk)
	{
		RTTRACE("CSystemObjectWrapper::Create -> Failed to create System: %s, Class: %s, Object:%s",sSystem.c_str(),sClass.c_str(),sName.c_str());
	}
    REL(piManager);
    REL(piSystem);
    REL(piObject);
    return bOk;
}

bool CSystemObjectWrapper::Create(ISystem *piSystem,std::string sClass,std::string sName)
{
    ReleaseInterfaces();
    bool            bOk=false;
    ISystemObject   *piObject=NULL;
    if(!piSystem->GetObject(sName,&piObject))
    {
        if(piSystem->CreateObject(sClass,sName,&piObject))
        {
            //m_bObjectMustBeDestroyed=true;
        }
    }
    if(piObject){bOk=GetInterfaces(piObject);}
    REL(piObject);
    return bOk;
}

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){return pItem->GetValueAddress()->Save(piNode,pItem->GetName())?true:false;}
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){return pItem->GetValueAddress()->Load(piNode,pItem->GetName())?true:false;}
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){piNode->DeleteNode(pItem->GetName());return true;}
void MRPersistencyInitialize(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){}
void MRPersistencyFree(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){}

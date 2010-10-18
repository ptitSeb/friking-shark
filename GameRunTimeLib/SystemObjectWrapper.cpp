#include "StdAfx.h"
#include "GameRunTimeLib.h"
#include ".\systemobjectwrapper.h"
#include "crtdbg.h"

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

bool CSystemObjectWrapper::Attach(std::string sSystemName,std::string sName)
{
    bool bOk=false;
	ISystemManager *piSystemManager=GetSystemManager();
    ISystem *piSystem=piSystemManager->GetSystem(sSystemName);
    if(piSystem){bOk=Attach(piSystem,sName);}
    REL(piSystem);
	REL(piSystemManager);
    return bOk;
}

bool CSystemObjectWrapper::Attach(ISystem *piSystem,std::string sName)
{
    bool bOk=false;
    ISystemObject *piObject=NULL;
    if(piSystem->GetObject(sName,&piObject)){bOk=Attach(piObject);}
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
					if(m_piSerializable->Unserialize(piChild))
					{
						return true;
					}
					else
					{
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
    if(!m_piObject){return S_OK;}

    if(!piNode){return false;}

	ISystemPersistencyNode *piSystemNode=piNode->AddNode("System");
	ISystemPersistencyNode *piClassNode=m_bAttached?NULL:piNode->AddNode("Class");
	ISystemPersistencyNode *piNameNode=piNode->AddNode("Name");

	if(piSystemNode){piSystemNode->SetValue(m_piObject->GetSystemName());}
	if(piClassNode){piClassNode->SetValue(m_piObject->GetClass());}
	if(piNameNode){piNameNode->SetValue(m_piObject->GetName());}

	if( m_bAttached && piSystemNode &&  piNameNode)
	{
		return true;
	}
	if( piSystemNode && piClassNode &&  piNameNode)
    {
        if(m_piSerializable)
        {
            ISystemPersistencyNode *piChild=piNode->AddNode("Data");
            if(piChild){return m_piSerializable->Serialize(piChild);}
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
    _ASSERTE(piSystem);
    if(piSystem){bOk=Create(piSystem,sClass,sName);}
    if(piObject){bOk=GetInterfaces(piObject);}
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

HRESULT MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){return pItem->GetValueAddress()->Save(piNode,pItem->GetName())?S_OK:E_FAIL;}
HRESULT MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){return pItem->GetValueAddress()->Load(piNode,pItem->GetName())?S_OK:E_FAIL;}
HRESULT MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){piNode->DeleteNode(pItem->GetName());return S_OK;}
void    MRPersistencyInitialize(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){}
void    MRPersistencyFree(CMRPersistentReferenceT<CSystemObjectWrapper> *pItem){}
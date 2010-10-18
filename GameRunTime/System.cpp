#include "StdAfx.h"
#include "GameRunTimeLib.h"
#include ".\system.h"
#include ".\SystemModule.h"

CSystem::CSystem(ISystemManager *piSystemManager)
{
	m_piSystemManager=piSystemManager;
    m_piSystem=NULL;
}

CSystem::~CSystem(void)
{
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

    _ASSERT(m_mClasses.size()==0);

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
};

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
};

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

void CSystem::GetObjects( std::vector<ISystemObject *> *pvObjects )
{
	std::map<std::string,ISystemObject*>::iterator i;
	for(i=m_mObjects.begin();i!=m_mObjects.end();i++)
	{
		pvObjects->push_back(ADD(i->second));
	}
}
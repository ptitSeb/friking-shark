#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "SystemUnknownBase.h"
#include "SystemObjectBase.h"


CSystemObjectBase::CSystemObjectBase(void)
{
	m_piSystem=NULL;
#ifdef _DEBUG
	ISystemManager *piManager=GetSystemManager();
	piManager->DebugRegisterObject(this);
	REL(piManager);
#endif
}

CSystemObjectBase::~CSystemObjectBase(void)
{
#ifdef _DEBUG
	ISystemManager *piManager=GetSystemManager();
	piManager->DebugUnregisterObject(this);
	REL(piManager);
#endif
}

void CSystemObjectBase::FinalRelease()
{
	Destroy();
}

bool CSystemObjectBase::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    m_sClass=sClass;
    m_sName=sName;
    m_piSystem=ADD(piSystem);
    if(m_piSystem && m_sName!=""){piSystem->RegisterObject(this);}
#ifdef _DEBUG
	ISystemManager *piManager=GetSystemManager();
	piManager->DebugRegisterObject(this);
	REL(piManager);
#endif
	return true;
}

void CSystemObjectBase::Destroy()
{
    if(m_piSystem && m_sName!=""){m_piSystem->UnregisterObject(this);}
    REL(m_piSystem);
}

std::string CSystemObjectBase::GetName(){return m_sName;}
std::string CSystemObjectBase::GetClass(){return m_sClass;}
std::string CSystemObjectBase::GetSystemName(){return m_piSystem?m_piSystem->GetName():"";}
ISystem     *CSystemObjectBase::GetSystem(){return ADD(m_piSystem);}

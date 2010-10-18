#include "StdAfx.h"
#include "GameRunTimeLib.h"
#include ".\systemunknownbase.h"
#include ".\systemobjectbase.h"
#include <crtdbg.h>


CSystemObjectBase::CSystemObjectBase(void)
{
	m_piSystem=NULL;
#ifdef _DEBUG
	GetSystemManager()->DebugRegisterObject(this);
#endif
}

CSystemObjectBase::~CSystemObjectBase(void)
{
	_ASSERTE(m_piSystem==NULL);
#ifdef _DEBUG
	GetSystemManager()->DebugUnregisterObject(this);
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
	GetSystemManager()->DebugRegisterObject(this);
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

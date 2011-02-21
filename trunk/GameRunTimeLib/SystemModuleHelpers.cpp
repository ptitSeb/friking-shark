#include "stdafx.h"
#include "GameRunTimeLib.h"
#include "./SystemUnknownBase.h"
#include "./SystemModuleHelpers.h"

CSystemModuleHelper::CSystemModuleHelper(HINSTANCE hInstance,tBuildClassMap pBuildClassMap)
{
	  m_hInstance=hInstance;
    memset(m_pSystemClasses,0,sizeof(m_pSystemClasses));
    m_nSystemClasses=0;
    pBuildClassMap(this);
}

CSystemModuleHelper::~CSystemModuleHelper()
{
    for(int x=0;x<CSystemModuleHelper::m_nSystemClasses;x++){REL(m_pSystemClasses[x]);}
    m_nSystemClasses=0;
}

void CSystemModuleHelper::AddClass(ISystemClass *piClass)
{
    m_pSystemClasses[m_nSystemClasses]=ADD(piClass);
    m_nSystemClasses++;
}
void CSystemModuleHelper::RegisterClasses(ISystem *piSystem)
{
    for(int x=0;x<CSystemModuleHelper::m_nSystemClasses;x++){piSystem->RegisterClassFactory(CSystemModuleHelper::m_pSystemClasses[x]);}
}
void CSystemModuleHelper::UnregisterClasses(ISystem *piSystem)
{
    for(int x=0;x<CSystemModuleHelper::m_nSystemClasses;x++){piSystem->UnregisterClassFactory(CSystemModuleHelper::m_pSystemClasses[x]);}
}
HINSTANCE CSystemModuleHelper::GetInstance()
{
	return m_hInstance;
}
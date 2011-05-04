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

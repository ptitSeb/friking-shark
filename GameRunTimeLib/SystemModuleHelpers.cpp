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
#include "./SystemUnknownBase.h"
#include "./SystemModuleHelpers.h"

CSystemModuleHelper::CSystemModuleHelper(tBuildClassMap pBuildClassMap)
{
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

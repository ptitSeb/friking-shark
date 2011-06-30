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
#include "StaticStructureType.h"
#include "DreadnoughtTowerType.h"

CDreadnoughtTowerType::CDreadnoughtTowerType()
{
	PersistencyInitialize();
}

CDreadnoughtTowerType::~CDreadnoughtTowerType()
{
}

IEntity *CDreadnoughtTowerType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CDreadnoughtTower *piEntity=new CDreadnoughtTower(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CDreadnoughtTower::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	m_nCurrentTime=dwCurrentTime;
	CStaticStructure::ProcessFrame(dwCurrentTime,dTimeFraction);
}

CDreadnoughtTower::CDreadnoughtTower(CDreadnoughtTowerType *pType,unsigned int dwCurrentTime)
:CStaticStructure(pType,dwCurrentTime)
{
	m_sClassName="CDreadnoughtTower";
	m_pType=pType;
	m_nCurrentTime=dwCurrentTime;
}

void CDreadnoughtTower::OnDamage(double dDamage,IEntity *piAggresor)
{
	if(m_piParent && m_dwCreationTime+m_piParent->GetRouteDelay()>m_nCurrentTime)
	{
		double dHealthFactor=((double)m_pType->m_nPreRouteHealthLimit)/100.0;
		double dHealthLimit=m_dMaxHealth*dHealthFactor;
		if(m_dHealth-dDamage<dHealthLimit)
		{
			dDamage=dHealthLimit-m_dHealth;
		}
	}
	CStaticStructure::OnDamage(dDamage,piAggresor);
}





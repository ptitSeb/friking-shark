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
#include "WorldEntity.h"
#include "WorldManager.h"

CWorldEntity::CWorldEntity(CWorldManager *pManager)
{
    m_sName="World";
    m_sClassName="CWorldEntity";
    m_pManager=pManager;

	m_dHealth=100000000;
	m_dwDamageType=DAMAGE_TYPE_NORMAL;
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_BSP;
}

CWorldEntity::~CWorldEntity(void)
{
}

void CWorldEntity::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
    m_pManager->Render(piRender,piCamera);
}

void CWorldEntity::OnDamage(double dDamage,IEntity *pAggresor)
{
}

CTraceInfo CWorldEntity::GetTrace( const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	if(m_pManager)
	{
		info=m_pManager->GetTerrainTrace(p1,p2);
	}
	else
	{
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
	}
	return info;
}
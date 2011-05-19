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
#include "GroundBossHatchType.h"

CGroundBossHatchType::CGroundBossHatchType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	
	PersistencyInitialize();
}

CGroundBossHatchType::~CGroundBossHatchType()
{
}

IEntity *CGroundBossHatchType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CGroundBossHatch *piEntity=new CGroundBossHatch(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CGroundBossHatchType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eGroundBossHatchState_Normal);
}

CGroundBossHatch::CGroundBossHatch(CGroundBossHatchType *pType,unsigned int dwCurrentTime)
{
	m_sClassName="CGroundBossHatch";
	m_pType=pType;
	m_bIsOpen=false;
	m_dRadius=m_pType->DesignGetRadius();
	m_dwNextStateChange=dwCurrentTime+m_pType->m_nOpenTime;
}


void CGroundBossHatch::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(m_piParent &&  m_piParent->GetHealth()<=0){return;}
	
	GetTarget();
	
	bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius,true);
	if(GetState()==eGroundBossHatchState_Open)
	{
		if(m_dwNextStateChange<dwCurrentTime && bVisible)
		{
			m_bIsOpen=false;
			SetState(eGroundBossHatchState_Closed,0);
			m_dwNextStateChange=dwCurrentTime+m_pType->m_nClosedTime;
		}
	}
	else
	{
		if(m_dwNextStateChange<dwCurrentTime && bVisible)
		{
			SetState(eGroundBossHatchState_Open,0);
			m_dwNextStateChange=dwCurrentTime+m_pType->m_nOpenTime;
		}
	}
	
	if(m_piTarget && m_bIsOpen && m_vWeapons.size())
	{
		bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius,true);
		if(bVisible){FireWeapon(0,dwCurrentTime);}
	}
}
void CGroundBossHatch::OnAnimationEvent(string sEvent,string sParams)
{
	if(sEvent=="Open"){m_bIsOpen=true;}
}

IEntity *CGroundBossHatch::GetTarget()
{
	if(m_piTarget==NULL)
	{
		IEntityManager *piManager=GetEntityManager();
		if(piManager){SetTarget(piManager->FindEntity("Player"));}
	}
	
	return m_piTarget;
}

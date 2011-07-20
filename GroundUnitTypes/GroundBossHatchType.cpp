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
#include "GroundUnitTypes.h"
#include "GroundBossHatchType.h"

CGroundBossHatchType::CGroundBossHatchType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	
	g_PlayerManagerWrapper.AddRef();
	PersistencyInitialize();
}

CGroundBossHatchType::~CGroundBossHatchType()
{
	g_PlayerManagerWrapper.Release();
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
	m_bFirstTimeVisible=true;
	m_dwNextShotTime=CHECKPOINT_HOLD_FIRE_TIME;
	m_dRadius=m_pType->DesignGetRadius();
	m_dwNextStateChange=dwCurrentTime+m_pType->m_nOpenTime;
}


void CGroundBossHatch::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(m_piParent &&  m_piParent->GetHealth()<=0){return;}
	
	GetTarget();
	
	bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius);
	if(GetState()==eGroundBossHatchState_Open)
	{
		if(m_dwNextStateChange<dwCurrentTime || m_piTarget==NULL)
		{
			m_bIsOpen=false;
			SetState(eGroundBossHatchState_Closed,0);
			m_dwNextStateChange=dwCurrentTime+m_pType->m_nClosedTime;
		}
	}
	else 
	{
		if(m_piTarget && m_dwNextStateChange<dwCurrentTime && bVisible)
		{
			SetState(eGroundBossHatchState_Open,0);
			m_dwNextStateChange=dwCurrentTime+m_pType->m_nOpenTime;
		}
	}
	
	if(m_piTarget && dwCurrentTime>m_dwNextShotTime && m_bIsOpen && m_vWeapons.size())
	{
		bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,0);
		if(bVisible)
		{
			double dDifficulty=g_PlayerManagerWrapper.m_piInterface->GetEffectiveDifficulty();
			double dTimeFirstShotMin=m_pType->m_dTimeFirstShotMin/dDifficulty;
			double dTimeFirstShotMax=m_pType->m_dTimeFirstShotMax/dDifficulty;
			double dTimeBetweenShotsMin=m_pType->m_dTimeBetweenShotsMin/dDifficulty;
			double dTimeBetweenShotsMax=m_pType->m_dTimeBetweenShotsMax/dDifficulty;
			
			if(m_bFirstTimeVisible)
			{
				m_bFirstTimeVisible=false;
				m_dwNextShotTime=dwCurrentTime+drand()*(dTimeFirstShotMax-dTimeFirstShotMin)+dTimeFirstShotMin;
			}
			else
			{
				for(unsigned int x=0;x<m_vWeapons.size();x++){FireWeapon(x,dwCurrentTime);}
				m_dwNextShotTime=dwCurrentTime+drand()*(dTimeBetweenShotsMax-dTimeBetweenShotsMin)+dTimeBetweenShotsMin;
			}			
		}
	}
}
void CGroundBossHatch::OnAnimationEvent(string sEvent,string sParams)
{
	CEntityBase::OnAnimationEvent(sEvent,sParams);
	if(sEvent=="Open"){m_bIsOpen=true;}
}

IEntity *CGroundBossHatch::GetTarget()
{
	if(m_piTarget==NULL)
	{
		IEntity 		*piTarget=NULL;
		IEntityManager 	*piManager=GetEntityManager();
		if(piManager){piTarget=piManager->FindEntity("Player");}
		if(piTarget && piTarget->GetHealth()>0)
		{
			SetTarget(piTarget);
		}
	}
	
	return m_piTarget;
}

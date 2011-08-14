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
#include "DreadnoughtTowerType.h"

CDreadnoughtTowerType::CDreadnoughtTowerType()
{
	g_PlayerManagerWrapper.AddRef();
	PersistencyInitialize();
}

CDreadnoughtTowerType::~CDreadnoughtTowerType()
{
	g_PlayerManagerWrapper.Release();
}

IEntity *CDreadnoughtTowerType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CDreadnoughtTower *piEntity=new CDreadnoughtTower(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CDreadnoughtTowerType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eDreadnoughtTowerState_Normal,0);
}

CDreadnoughtTower::CDreadnoughtTower(CDreadnoughtTowerType *pType,unsigned int dwCurrentTime)
{
	m_sClassName="CDreadnoughtTower";
	m_pType=pType;
	m_nCurrentTime=dwCurrentTime;
	
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
	m_dwNextShotTime=CHECKPOINT_HOLD_FIRE_TIME;
	m_dRadius=m_pType->DesignGetRadius();
	m_bFirstTimeVisible=true;
}

IEntity *CDreadnoughtTower::GetTarget()
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
void CDreadnoughtTower::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	m_nCurrentTime=dwCurrentTime;
	
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(GetState()==eDreadnoughtTowerState_Destroyed){return;}
	if(GetState()==eDreadnoughtTowerState_Normal)
	{
		size_t nAnimationToSet=0;
		double dMaxHealth=GetMaxHealth();
		unsigned int nAnimations=m_pTypeBase->GetStateAnimations(ENTITY_STATE_BASE);
		nAnimationToSet=(size_t)(((dMaxHealth-m_dHealth)/dMaxHealth)*((double)nAnimations));
		if(nAnimationToSet>nAnimations-1){nAnimationToSet=nAnimations-1;}
		SetState(ENTITY_STATE_BASE,(int)nAnimationToSet);
	}
	
	bool bAllChildDead=true;
	for(unsigned int x=0;x<m_vChildren.size();x++){if(m_vChildren[x].piEntity->GetHealth()>0){bAllChildDead=false;}}
	m_dwDamageType=(bAllChildDead?m_nConfiguredDamageType:DAMAGE_TYPE_NONE);
	
	if(m_dwAlignment==ENTITY_ALIGNMENT_ENEMIES)
	{
		GetTarget();
	}
	
	if(m_piTarget && m_vWeapons.size() &&
		(m_dwCreationTime+m_piParent->GetRouteDelay())<m_nCurrentTime &&
		dwCurrentTime>m_dwNextShotTime)
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


void CDreadnoughtTower::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	
	if(m_pTypeBase->GetStateAnimations(eDreadnoughtTowerState_Destroyed))
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		
		SetState(eDreadnoughtTowerState_Destroyed);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
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
	CEntityBase::OnDamage(dDamage,piAggresor);
}

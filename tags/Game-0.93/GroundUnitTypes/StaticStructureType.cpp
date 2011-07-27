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
#include "StaticStructureType.h"

CStaticStructureType::CStaticStructureType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	
	g_PlayerManagerWrapper.AddRef();
	PersistencyInitialize();
}

CStaticStructureType::~CStaticStructureType()
{
	g_PlayerManagerWrapper.Release();
}

IEntity *CStaticStructureType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CStaticStructure *piEntity=new CStaticStructure(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CStaticStructureType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eStaticStructureState_Normal);
}

void CStaticStructureType::GetVulnerableRegions(std::vector<SBBox> *pvRegions){*pvRegions=m_vVulnerableRegions;}
void CStaticStructureType::SetVulnerableRegions(std::vector<SBBox> *pvRegions){m_vVulnerableRegions=*pvRegions;}
void CStaticStructureType::GetProtectiveRegions(std::vector<SBBox> *pvRegions){*pvRegions=m_vProtectiveRegions;}
void CStaticStructureType::SetProtectiveRegions(std::vector<SBBox> *pvRegions){m_vProtectiveRegions=*pvRegions;}
void CStaticStructureType::GetProtectiveDestroyedRegions(std::vector<SBBox> *pvRegions){*pvRegions=m_vProtectiveDestroyedRegions;}
void CStaticStructureType::SetProtectiveDestroyedRegions(std::vector<SBBox> *pvRegions){m_vProtectiveDestroyedRegions=*pvRegions;}

CStaticStructure::CStaticStructure(CStaticStructureType *pType,unsigned int dwCurrentTime)
{
	m_sClassName="CStaticStructure";
	m_pType=pType;
	
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
	m_dwNextShotTime=0;
	m_dRadius=m_pType->DesignGetRadius();
	m_bFirstTimeVisible=true;
}

void CStaticStructure::OnDamage(double dDamage,IEntity *piAggresor)
{
	if((m_dHealth-dDamage)<=0 && m_pTypeBase->GetStateAnimations(eStaticStructureState_Destroyed))
	{
		SetState(eStaticStructureState_Destroyed);
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
	}
	else
	{
		CEntityBase::OnDamage(dDamage,piAggresor);
	}
}

void CStaticStructure::OnKilled()
{
	bool bRemove=false;
	if(!m_pTypeBase->GetStateAnimations(eStaticStructureState_Destroyed))
	{
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}

void CStaticStructure::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	m_dwNextProcessFrame=dwCurrentTime+10;
	
	if(GetState()==eStaticStructureState_Destroyed){return;}
	if(GetState()==eStaticStructureState_Normal)
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
	
	if(m_piTarget && dwCurrentTime>m_dwNextShotTime && m_vWeapons.size())
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

IEntity *CStaticStructure::GetTarget()
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

void CStaticStructure::OnAnimationEvent(string sEvent,string sParams)
{
	CEntityBase::OnAnimationEvent(sEvent,sParams);
	if(sEvent=="DestructionFinished")
	{
		Kill();
	}
}

const std::vector<SBBox> &CStaticStructure::GetVulnerableRegions(){return m_pType->m_vVulnerableRegions;}
const std::vector<SBBox> &CStaticStructure::GetProtectiveRegions(){return m_dHealth>0?m_pType->m_vProtectiveRegions:m_pType->m_vProtectiveDestroyedRegions;}


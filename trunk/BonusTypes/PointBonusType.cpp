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
#include "PointBonusType.h"

CPointBonusType::CPointBonusType()
{
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_NONE;
}

CPointBonusType::~CPointBonusType()
{
	
}

IEntity *CPointBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CPointBonus *piEntity=new CPointBonus(this);
	SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	InitializeEntity(piEntity,dwCurrentTime);
	pPhysicInfo->vPosition=piParent?piParent->GetPhysicInfo()->vPosition:Origin;
	piEntity->SetState(ENTITY_STATE_BASE,ANIMATION_RANDOM);
    return piEntity;
}

CPointBonus::CPointBonus(CPointBonusType *pType)
{
  m_sClassName="CPointBonus";
  m_sName="PointBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
  
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(GetEntityManager()->FindEntity("Player"));
  if(piPlayer)
  {
	  SEntityTypeConfig sConfig;
	  m_pType->GetEntityTypeConfig(&sConfig);
	  if(sConfig.nPoints){piPlayer->AddPoints(sConfig.nPoints);}
	  Remove();
  }
}

void CPointBonus::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	Remove();
}






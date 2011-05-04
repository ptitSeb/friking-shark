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
#include "LiveBonusType.h"

CLiveBonusType::CLiveBonusType()
{
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CLiveBonusType::~CLiveBonusType()
{
}

IEntity *CLiveBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CLiveBonus *piEntity=new CLiveBonus(this);
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
  InitializeEntity(piEntity,dwCurrentTime);
  pPhysicInfo->vPosition=piParent?piParent->GetPhysicInfo()->vPosition:Origin;
  pPhysicInfo->vAngleVelocity.c[0]=50.0;
  pPhysicInfo->vAngleVelocity.c[1]=150.0;
  piEntity->SetState(eLiveBonusState_Normal);
  return piEntity;
}

CLiveBonus::CLiveBonus(CLiveBonusType *pType)
{
  m_sClassName="CLiveBonus";
  m_sName="LiveBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
}

bool CLiveBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
	SEntityTypeConfig sConfig;
	m_pType->GetEntityTypeConfig(&sConfig);
	if(sConfig.nPoints){piPlayer->AddPoints(sConfig.nPoints);}
	piPlayer->AddLivesLeft(1);
	SetState(eLiveBonusState_Taken);
    Remove();
  }
  return false;
}


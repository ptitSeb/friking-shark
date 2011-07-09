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
#include "BombBonusType.h"
#include "GameGraphics.h"

CBombBonusType::CBombBonusType()
{
	m_PlayAreaManager.Attach("GameSystem","PlayAreaManager");
	
	m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CBombBonusType::~CBombBonusType()
{
}

IEntity *CBombBonusType::CreateInstance(IEntity *piOwner,unsigned int dwCurrentTime)
{
	CBombBonus *piEntity=new CBombBonus(this,piOwner);
	InitializeEntity(piEntity,dwCurrentTime);
  
	piEntity->SetState(eBombBonusState_Normal,ANIMATION_RANDOM);
	return piEntity;
}

CBombBonus::CBombBonus(CBombBonusType *pType,IEntity *piOwner)
{
  m_sClassName="CBombBonus";
  m_sName="BombBonus";
  m_pType=pType;
  m_piCamera=NULL;
  m_dwDamageType=DAMAGE_TYPE_NONE;
  m_piOwner=piOwner;
  if(m_piOwner)
  {
	  SUBSCRIBE_TO_CAST(m_piOwner,IEntityEvents);
	  m_vOriginalPosition=m_piOwner->GetPhysicInfo()->vPosition;
  }
  
  
  if(m_pType->m_PlayAreaManager.m_piPlayAreaManager)
  {
	  CVector vStart,vEnd;
	  CVector vCameraPos;
	  m_pType->m_PlayAreaManager.m_piPlayAreaManager->GetPlayerRoute(&vStart,&vEnd);
	  m_piCamera=m_pType->m_PlayAreaManager.m_piPlayAreaManager->GetCamera();
	  m_PlayAreaPlane=CPlane(AxisPosY,vStart);
	  
	  CVector vCut;
	  if(m_piCamera){vCameraPos=m_piCamera->GetPosition();}
	  if(m_PlayAreaPlane.Cut(m_vOriginalPosition,vCameraPos,&vCut))
	  {
		  m_PhysicInfo.vPosition=vCut;
	  }
  }
}

CBombBonus::~CBombBonus()
{
	REL(m_piCamera);
}

bool CBombBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
	SEntityTypeConfig sConfig;
	m_pType->GetEntityTypeConfig(&sConfig);
	if(sConfig.nPoints){piPlayer->AddPoints(sConfig.nPoints);}
	
    vector<IWeapon*> vWeapons;
    piPlayer->GetWeaponsOnSlot(1,&vWeapons);
    for(size_t x=0;x<vWeapons.size();x++)
    {
      IWeapon *piWeapon=vWeapons[x];
      piWeapon->AddAmmo(1);
    }
    SetState(eBombBonusState_Taken);
    Remove();
  }
  return false;
}

void CBombBonus::OnRemoved(IEntity *piEntity)
{
	CEntityBase::OnRemoved(piEntity);
	if(piEntity==m_piOwner)
	{
		UNSUBSCRIBE_FROM_CAST(m_piOwner,IEntityEvents);
		m_piOwner=NULL;
		Remove();
	}
}

void CBombBonus::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	
	if(m_piOwner){m_vOriginalPosition=m_piOwner->GetPhysicInfo()->vPosition;}
	
	CVector vMins,vMaxs;
	m_pType->m_PlayAreaManager.m_piPlayAreaManager->GetCurrentVisibleArea(&vMins,&vMaxs);
	
	if(m_PhysicInfo.vPosition.c[0]+m_PhysicInfo.vMaxs.c[0]<vMins.c[0])
	{
		Remove();
		return;
	}
	
	CVector vCameraPos,vCut;
	if(m_piCamera){vCameraPos=m_piCamera->GetPosition();}
	if(m_PlayAreaPlane.Cut(m_vOriginalPosition,vCameraPos,&vCut))
	{
		m_PhysicInfo.vPosition=vCut;
	}
}

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
#include "BulletProjectileType.h"
#include "../GameGraphics/GameGraphics.h"

struct SBulletCollisionData
{
	CPlane  forwardPlane;
	CPlane  fallPlane;
};

CBulletProjectileType::CBulletProjectileType()
{
  m_dDamage=1.0;
  m_dwDuration=1000;
  m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
  
}

CBulletProjectileType::~CBulletProjectileType(){}

IEntity *CBulletProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBulletProjectile *piEntity=new CBulletProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetState(eBulletState_Normal);
  return piEntity;
}

CBulletProjectile::CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent)
{
	m_dwNextCollisionCheck=0;
	m_sClassName="CBulletProjectile";
	m_sName="BulletProjectile";
	m_pType=pType;
	m_piParent=piParent;
	m_dRadius=pType->DesignGetRadius();

	g_PlayAreaManagerWrapper.AddRef();
}

CBulletProjectile::~CBulletProjectile()
{
	g_PlayAreaManagerWrapper.Release();
}

void CBulletProjectile::CheckCollisions(IEntity *piEntity,void *pParam1,void *pParam2)
{
	SBulletCollisionData *pCollisionData=(SBulletCollisionData *)pParam2;
	CBulletProjectile *pThis=(CBulletProjectile *)pParam1;
	
	if(pThis->m_bRemoved){return;}
	if(pThis->m_piParent==NULL){return;}
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}
	if(piEntity->GetDamageType()==DAMAGE_TYPE_NONE){return;}
	if(piEntity->GetHealth()<=0.0){return;}
	
	SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	CVector pVolumePoints[8];
	CalcBBoxVolume(pPhysicInfo->vPosition,pPhysicInfo->vAngles,pPhysicInfo->vMins,pPhysicInfo->vMaxs,pVolumePoints);

	int nPointsIn=0,nPointsOut=0;
	for(unsigned int x=0;x<8;x++){if(pCollisionData->fallPlane.GetSide(pVolumePoints[x])>=0){nPointsOut++;}else{nPointsIn++;}}
	if(nPointsOut && nPointsIn)
	{
		nPointsIn=0,nPointsOut=0;
		for(unsigned int x=0;x<8;x++){if(pCollisionData->forwardPlane.GetSide(pVolumePoints[x])>=0){nPointsOut++;}else{nPointsIn++;}}
		if(nPointsOut && nPointsIn) 
		{
			piEntity->OnCollision(pThis,pThis->m_PhysicInfo.vPosition);
			pThis->OnCollision(piEntity,pThis->m_PhysicInfo.vPosition);
		}
	}
}

void CBulletProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	bool bVisible=g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius);

	if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime || !bVisible)
	{
		Remove();
	}
	else if(m_dwNextCollisionCheck<dwCurrentTime)
	{
		if(m_dwAlignment==ENTITY_ALIGNMENT_PLAYER)
		{
			IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
			CVector vCameraPos=piCamera?piCamera->GetPosition():Origin;
			REL(piCamera);
			
			CVector vRight,vNormalizedVelocity=m_PhysicInfo.vVelocity;
			vNormalizedVelocity.N();
			VectorsFromAngles(m_PhysicInfo.vAngles,NULL,&vRight,NULL);

			SBulletCollisionData data;
			data.fallPlane=CPlane(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+m_PhysicInfo.vVelocity,vCameraPos);
			data.forwardPlane=CPlane(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+vRight*10.0,vCameraPos);
			
			GetEntityManager()->PerformUnaryOperation(CheckCollisions,this,&data);
			m_dwNextCollisionCheck=dwCurrentTime+20;
		}
	}
}
void CBulletProjectile::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	CEntityBase::Render(piRender,piCamera);	
}

bool CBulletProjectile::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(piOther->GetAlignment()!=m_dwAlignment && piOther->GetDamageType()!=DAMAGE_TYPE_NONE && piOther->GetHealth()>0.0 )
  {
    piOther->OnDamage(m_pType->m_dDamage,m_piParent);

	if(m_pTypeBase->GetStateAnimations(eBulletState_Hit))
    {
		SetState(eBulletState_Hit);
    }
    Remove();
  }
  return false;
}


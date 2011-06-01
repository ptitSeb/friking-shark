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

struct SBulletTargetingData
{
	CPlane  forwardPlane;
	CPlane  velPlane;
	CPlane  fallPlane;
	CVector vFallDirection;
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
	m_dwNextTryAcquireTarget=0;
	m_sClassName="CBulletProjectile";
	m_sName="BulletProjectile";
	m_pType=pType;
	m_piParent=piParent;
	m_bTargetAcquired=false;
	m_dRadius=pType->DesignGetRadius();

	g_PlayAreaManagerWrapper.AddRef();
}

CBulletProjectile::~CBulletProjectile()
{
	g_PlayAreaManagerWrapper.Release();
}

void CBulletProjectile::AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
	SBulletTargetingData *pTargetingData=(SBulletTargetingData *)pParam2;
	CBulletProjectile *pThis=(CBulletProjectile *)pParam1;
	
	if(pThis->m_piParent==NULL){return;}
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}
	if(piEntity->GetDamageType()==DAMAGE_TYPE_NONE){return;}
	if(piEntity->GetHealth()<=0.0){return;}
	
	if(pTargetingData->forwardPlane.GetSide(piEntity->GetPhysicInfo()->vPosition)<0)
	{
		return;
	}	
	
	SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	CVector pVolumePoints[8];
	CalcBBoxVolume(pPhysicInfo->vPosition,pPhysicInfo->vAngles,pPhysicInfo->vMins,pPhysicInfo->vMaxs,pVolumePoints);

	int nPointsIn=0,nPointsOut=0;
	for(unsigned int x=0;x<8;x++){if(pTargetingData->fallPlane.GetSide(pVolumePoints[x])>=0){nPointsOut++;}else{nPointsIn++;}}
	if(nPointsOut && nPointsIn)
	{
		CVector vTargetPos=pPhysicInfo->vPosition-(pTargetingData->fallPlane)*pTargetingData->fallPlane.GetSide(pPhysicInfo->vPosition);
		double dTargetDistance=pThis->m_PhysicInfo.vPosition-vTargetPos;
		if(pThis->m_bTargetAcquired==false || dTargetDistance<(pThis->m_PhysicInfo.vPosition-pThis->m_vTargetPosition))
		{
			CVector vTargetDif=vTargetPos-pThis->m_PhysicInfo.vPosition;
			vTargetDif.c[1]=0;
			double dTimeToTarget=(vTargetDif)/pThis->m_PhysicInfo.dMaxVelocity;
			if(dTimeToTarget<=0.2)
			{
				double dVerticalVel=(vTargetPos.c[1]-pThis->m_PhysicInfo.vPosition.c[1])/dTimeToTarget;
				CVector vVel=pTargetingData->vFallDirection*dVerticalVel;
				// Dada la perspectiva puede que las ballas tengan que ir hacia atras,
				if(pTargetingData->velPlane.GetSide(pPhysicInfo->vPosition)<0)
				{
					pThis->m_PhysicInfo.vVelocity=(Origin-pThis->m_vOriginalVelocity)+vVel;
				}
				else
				{
					pThis->m_PhysicInfo.vVelocity=pThis->m_vOriginalVelocity+vVel;
				}
				pThis->m_vTargetPosition=vTargetPos;
				pThis->m_bTargetAcquired=true;
			}
		}
	}
}

void CBulletProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	if(!m_bTargetAcquired){m_vOriginalVelocity=m_PhysicInfo.vVelocity;}

	bool bVisible=g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius,true);

	if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime || !bVisible)
	{
		Remove();
	}
	else if(m_dwNextTryAcquireTarget<dwCurrentTime &&  !m_bTargetAcquired)
	{
		if(m_dwAlignment==ENTITY_ALIGNMENT_PLAYER)
		{
			IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
			CVector vCameraPos=piCamera?piCamera->GetPosition():Origin;
			REL(piCamera);
			
			CVector vRight,vNormalizedVelocity=m_vOriginalVelocity;
			vNormalizedVelocity.N();
			VectorsFromAngles(m_PhysicInfo.vAngles,NULL,&vRight,NULL);

			SBulletTargetingData data;
			data.velPlane=CPlane(vNormalizedVelocity,m_PhysicInfo.vPosition);
			data.fallPlane=CPlane(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+m_vOriginalVelocity,vCameraPos);
			data.forwardPlane=CPlane(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+vRight*10.0,vCameraPos);
			data.vFallDirection=(vNormalizedVelocity)^data.fallPlane;
			
			GetEntityManager()->PerformUnaryOperation(AcquireTargetOperation,this,&data);
			if(!m_bTargetAcquired)
			{
				m_dwNextTryAcquireTarget=dwCurrentTime+20;
			}
		}
		else
		{
			m_bTargetAcquired=true;
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


#include "./stdafx.h"
#include "BulletProjectileType.h"
#include "../GameGraphics/GameGraphics.h"

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

	g_PlayAreaManagerWrapper.AddRef();
}

CBulletProjectile::~CBulletProjectile()
{
	g_PlayAreaManagerWrapper.Release();
}

void CBulletProjectile::AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CBulletProjectile *pThis=(CBulletProjectile *)pParam1;
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}
	if(pThis->m_piParent==NULL){return;}

	
	if(piEntity->GetDamageType()!=DAMAGE_TYPE_NONE && piEntity->GetHealth()>0.0)
	{
		SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
		CVector pVolumePoints[8];
		CalcBBoxVolume(pPhysicInfo->vPosition,pPhysicInfo->vAngles,pPhysicInfo->vMins,pPhysicInfo->vMaxs,pVolumePoints);

		IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
		CVector vPlanePoints[3];
		vPlanePoints[0]=pThis->m_PhysicInfo.vPosition;
		vPlanePoints[1]=pThis->m_PhysicInfo.vPosition+pThis->m_PhysicInfo.vVelocity;
		vPlanePoints[2]=piCamera?piCamera->GetPosition():Origin;
		REL(piCamera);

		CPlane plane(vPlanePoints[0],vPlanePoints[1],vPlanePoints[2]);

		int nPointsIn=0,nPointsOut=0;
		for(unsigned int x=0;x<8;x++){if(plane.GetSide(pVolumePoints[x])>=0){nPointsOut++;}else{nPointsIn++;}}
		if(nPointsOut && nPointsIn)
		{
			double dTargetDistance=pThis->m_PhysicInfo.vPosition-pPhysicInfo->vPosition;
			if(pThis->m_bTargetAcquired==false || dTargetDistance<(pThis->m_PhysicInfo.vPosition-pThis->m_vTargetPosition))
			{
				pThis->m_vTargetPosition=piEntity->GetPhysicInfo()->vPosition;
				CVector vTargetDif=pThis->m_vTargetPosition-pThis->m_PhysicInfo.vPosition;
				vTargetDif.c[1]=0;
				double dTimeToTarget=(vTargetDif)/pThis->m_PhysicInfo.dMaxVelocity;
				double dVerticalVel=(pThis->m_vTargetPosition.c[1]-pThis->m_PhysicInfo.vPosition.c[1])/dTimeToTarget;
				CVector vVel=(vPlanePoints[0]-vPlanePoints[1])^(Origin-plane);
				vVel.N();
				vVel*=dVerticalVel;
				pThis->m_PhysicInfo.vVelocity=pThis->m_vOriginalVelocity+vVel;
				pThis->m_bTargetAcquired=true;
			}
		}
	}
}
void CBulletProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	if(!m_bTargetAcquired)
	{
		if(m_dwAlignment==ENTITY_ALIGNMENT_ENEMIES)
		{
			m_PhysicInfo.vVelocity.c[1]=0;
			m_vOriginalVelocity=m_PhysicInfo.vVelocity;
		}
		else
		{
			m_vOriginalVelocity=m_PhysicInfo.vVelocity;
		}
	}

	if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime)
	{
		Remove();
	}
	else if(m_dwAlignment==ENTITY_ALIGNMENT_PLAYER && m_dwNextTryAcquireTarget<dwCurrentTime &&  !m_bTargetAcquired)
	{
		GetEntityManager()->PerformUnaryOperation(AcquireTargetOperation,this,NULL);
		if(!m_bTargetAcquired)
		{
			m_dwNextTryAcquireTarget=dwCurrentTime+100;
		}
	}
	else if(m_dwAlignment==ENTITY_ALIGNMENT_ENEMIES && m_dwNextTryAcquireTarget<dwCurrentTime &&  !m_bTargetAcquired)
	{
		m_bTargetAcquired=true;
		/*
		GetEntityManager()->PerformUnaryOperation(AcquireTargetOperation,this,NULL);
		if(!m_bTargetAcquired)
		{
			m_dwNextTryAcquireTarget=dwCurrentTime+100;
		}
		*/
	}

	if(m_bTargetAcquired && m_dwAlignment==ENTITY_ALIGNMENT_ENEMIES)
	{
		CVector vPlayerStart,vPlayerEnd;
		g_PlayAreaManagerWrapper.m_piInterface->GetPlayerRoute(&vPlayerStart,&vPlayerEnd);
		double dPlayerHeight=vPlayerStart.c[1];
		IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
		CVector vPlanePoints[3];
		vPlanePoints[0]=m_PhysicInfo.vPosition;
		vPlanePoints[1]=m_PhysicInfo.vPosition+m_PhysicInfo.vVelocity;
		vPlanePoints[2]=piCamera?piCamera->GetPosition():Origin;

		CPlane plane(vPlanePoints[0],vPlanePoints[1],vPlanePoints[2]);

		CVector vVel=(vPlanePoints[0]-vPlanePoints[1])^(Origin-plane);
		double dVerticalVel=(dPlayerHeight-m_PhysicInfo.vPosition.c[1])*10.0;// 0.1 es el tiempo que tarda en subir
		vVel.N();
		vVel*=dVerticalVel;

		m_PhysicInfo.vVelocity=m_vOriginalVelocity+vVel;
		REL(piCamera);
	}
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


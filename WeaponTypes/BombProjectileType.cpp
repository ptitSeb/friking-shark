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
#include "BombProjectileType.h"
#include "../GameGraphics/GameGraphics.h"


struct SBombDamageData
{
	double dTimeFraction;
	double dDamage;
	double dRadius;
	CVector vCameraPos;
	CPlane  playAreaPlane;
	bool   bDamageEffect;
	
	SBombDamageData(){bDamageEffect=false;dTimeFraction=0;dDamage=0;dRadius=0;}
};

CBombProjectileType::CBombProjectileType()
{
   m_nCollisionType=PHYSIC_COLLISION_TYPE_NONE;
   m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
   PersistencyInitialize();
   g_PlayAreaManagerWrapper.AddRef();
   g_PhysicsManagerWrapper.AddRef();
   
}

CBombProjectileType::~CBombProjectileType()
{
	g_PlayAreaManagerWrapper.Release();
	g_PhysicsManagerWrapper.Release();
	
}

IEntity *CBombProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBombProjectile *piEntity=new CBombProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetState(eBombState_Normal);
  return piEntity;
}

CBombProjectile::CBombProjectile(CBombProjectileType *pType,IEntity *piParent)
{
  m_nCurrentTime=0;
  m_sClassName="CBombProjectile";
  m_sName="BombProjectile";
  m_pType=pType;
  m_piParent=piParent;
  m_nNextDamageEffect=0;
}

CBombProjectile::~CBombProjectile()
{
	std::vector<IParticleSystem *>::iterator i;
	for(i=m_vParticleSystems.begin();i!=m_vParticleSystems.end();i++)
	{
		delete *i;
	}
	m_vParticleSystems.clear();
}

void CBombProjectile::ApplyDamageOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CBombProjectile *pThis=(CBombProjectile *)pParam1;
	SBombDamageData *pDamageData=((SBombDamageData*)pParam2);
	
	bool bProjectile=(*piEntity->GetEntityClass())=="CBulletProjectile";
	
	if(pThis->m_piParent==NULL){return;}
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}
	if(piEntity->GetDamageType()==DAMAGE_TYPE_NONE && !bProjectile){return;}
	if(piEntity->GetHealth()<=0.0){return;}
	
	SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
	
	CVector vProjection;
	bool bHit=false;
	
	if(pPhysicInfo->dwBoundsType==PHYSIC_BOUNDS_TYPE_BBOX)
	{
		const std::vector<SBBox> *pvBBoxes=pPhysicInfo->pvBBoxes;
		
		if(pDamageData->playAreaPlane.Cut(pDamageData->vCameraPos,pPhysicInfo->vPosition,&vProjection))
		{
			CVector vDist=vProjection-pThis->m_PhysicInfo.vPosition;
			double dDist=vDist;
			bHit=(dDist<pDamageData->dRadius);
		}
		for(unsigned int b=0;!bHit && pvBBoxes && b<pvBBoxes->size();b++)
		{
			CVector pVolumePoints[8];
			CalcBBoxVolume(pPhysicInfo->vPosition,pPhysicInfo->vAngles,(*pvBBoxes)[b].vMins,(*pvBBoxes)[b].vMaxs,pVolumePoints);
			
			for(unsigned int x=0;!bHit && x<8;x++)
			{
				if(pDamageData->playAreaPlane.Cut(pDamageData->vCameraPos,pVolumePoints[x],&vProjection))
				{
					CVector vDist=vProjection-pThis->m_PhysicInfo.vPosition;
					double dDist=vDist;
					bHit=(dDist<pDamageData->dRadius);
				}
			}
		}
	}
	else if(pPhysicInfo->dwBoundsType==PHYSIC_BOUNDS_TYPE_BSP)
	{		
		const std::vector<SBBox> *pvVulnerableRegions=NULL;
		if(*piEntity->GetEntityClass()=="CStaticStructure")
		{
			IStaticStructure *piStaticStructure=dynamic_cast<IStaticStructure *>(piEntity);
			if(piStaticStructure && piStaticStructure->GetVulnerableRegions().size()){pvVulnerableRegions=&piStaticStructure->GetVulnerableRegions();}
		}
		
		for(double dX=-pDamageData->dRadius;dX<=pDamageData->dRadius;dX+=pThis->m_pType->m_dDamageEffectSeparation)
		{
			for(double dZ=-pDamageData->dRadius;dZ<=pDamageData->dRadius;dZ+=pThis->m_pType->m_dDamageEffectSeparation)
			{
				CVector vOffset(dX,0,dZ);
				if(vOffset>pDamageData->dRadius){continue;}
				
				vOffset+=CVector(pThis->m_pType->m_dDamageEffectSeparation*(drand()-0.5),0,pThis->m_pType->m_dDamageEffectSeparation*(drand()-0.5));
				CVector vDirection=pThis->m_PhysicInfo.vPosition+vOffset-pDamageData->vCameraPos;
				CVector vDest=pThis->m_PhysicInfo.vPosition+vDirection*1000.0;
				
				CTraceInfo info=piEntity->GetTrace(pDamageData->vCameraPos,vDest);
				if(pThis->m_pType->m_DamageEffect.m_piParticleSystemType && info.m_bTraceHit)
				{
					CVector vTempPos=info.m_vTracePos-pPhysicInfo->vPosition;
					CVector vRelPos;
					vRelPos.c[0]=pPhysicInfo->vOwnX*vTempPos;
					vRelPos.c[1]=pPhysicInfo->vOwnY*vTempPos;
					vRelPos.c[2]=pPhysicInfo->vOwnZ*vTempPos;
					
					bool bSimpleHit=true;
					
					if(pvVulnerableRegions)
					{
						bSimpleHit=false;
						for(unsigned int b=0;!bSimpleHit && b<pvVulnerableRegions->size();b++)
						{
							bSimpleHit= (vRelPos.c[0]>=(*pvVulnerableRegions)[b].vMins.c[0] &&
										vRelPos.c[0]<=(*pvVulnerableRegions)[b].vMaxs.c[0] &&
										vRelPos.c[1]>=(*pvVulnerableRegions)[b].vMins.c[1] &&
										vRelPos.c[1]<=(*pvVulnerableRegions)[b].vMaxs.c[1] &&
										vRelPos.c[2]>=(*pvVulnerableRegions)[b].vMins.c[2] &&
										vRelPos.c[2]<=(*pvVulnerableRegions)[b].vMaxs.c[2]);
						}
					}
					if(bSimpleHit)
					{
						bHit=true;
						if(pDamageData->bDamageEffect)
						{
							IParticleSystem *piParticleSystem=pThis->m_pType->m_DamageEffect.m_piParticleSystemType->CreateInstance(pThis->m_nCurrentTime);
							if(piParticleSystem)
							{
								piParticleSystem->SetPosition(info.m_vTracePos);
								pThis->m_vParticleSystems.push_back(piParticleSystem);
							}
						}
					}
				}
			}
		}
	}
	if(bHit)
	{		
		if(bProjectile)
		{
			piEntity->Remove();
		}
		else
		{
			piEntity->OnDamage(pDamageData->dDamage,pThis);
		}
	}
}

void CBombProjectile::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	CEntityBase::Render(piRender,piCamera);
	
	std::vector<IParticleSystem *>::iterator i;
	for(i=m_vParticleSystems.begin();i!=m_vParticleSystems.end();i++)
	{
		IParticleSystem *piParticleSystem=*i;
		piParticleSystem->CustomRender(piRender,piCamera);
	}
}
	
void CBombProjectile::ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)
{
	CEntityBase::ProcessAnimations(dwCurrentTime,dTimeFraction,pbAnimationsFinished);
	
	std::vector<IParticleSystem *>::iterator i;
	for(i=m_vParticleSystems.begin();i!=m_vParticleSystems.end();)
	{
		IParticleSystem *piParticleSystem=*i;
		if(!piParticleSystem->ProcessFrame(g_PhysicsManagerWrapper.m_piInterface,dwCurrentTime,dTimeFraction))
		{
			delete piParticleSystem;
			i=m_vParticleSystems.erase(i);
		}
		else
		{
			if(pbAnimationsFinished){*pbAnimationsFinished=false;}
			i++;
		}
	}
}

void CBombProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	m_nCurrentTime=dwCurrentTime;
	if(GetState()==eBombState_Normal)
	{
		if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode))
		{
			if(m_pTypeBase->GetStateAnimations(eBombState_Hit))
			{
				m_PhysicInfo.vVelocity=Origin;
				m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
				
				SetState(eBombState_Hit);
			}
			else
			{
				Remove();
			}
		}
	}
	else if(GetState()==eBombState_Hit)
	{
		if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageEndTime))
		{
			Remove();
		}
		else if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageStartTime))
		{
			IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
			CVector vCameraPos=piCamera?piCamera->GetPosition():Origin;
			REL(piCamera);
			
			double dElapsedTime=((double)(dwCurrentTime-(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageStartTime)));
			double dTotalDuration=((double)(m_pType->m_nDamageEndTime-m_pType->m_nDamageStartTime));
			double dElapsedFraction=dElapsedTime/dTotalDuration;
			
			SBombDamageData data;
			data.dTimeFraction=dTimeFraction;
			data.dDamage=m_pType->m_dDamagePerSecond*dTimeFraction;
			data.dRadius=(m_pType->m_dDamageEndRadius-m_pType->m_dDamageStartRadius)*dElapsedFraction+m_pType->m_dDamageStartRadius;
			data.vCameraPos=vCameraPos;
			data.playAreaPlane=CPlane(AxisPosY,m_PhysicInfo.vPosition);
			if(dwCurrentTime>m_nNextDamageEffect)
			{
				data.bDamageEffect=true;
				m_nNextDamageEffect=dwCurrentTime+m_pType->m_nDamageEffectInterval;
			}
			GetEntityManager()->PerformUnaryOperation(ApplyDamageOperation,this,&data);
		}
	}
}







	

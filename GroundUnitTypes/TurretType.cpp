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
#include "TurretType.h"
#include "GameGraphics.h"

CTurretType::CTurretType()
{
	g_PlayAreaManagerWrapper.AddRef();
	g_PlayerManagerWrapper.AddRef();
	PersistencyInitialize();
}

CTurretType::~CTurretType()
{
	g_PlayAreaManagerWrapper.Release();
	g_PlayerManagerWrapper.Release();
}

IEntity *CTurretType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CTurret *piEntity=new CTurret(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	// Set damage type to none till first frame to let the unit know if it is inside a building.
	piEntity->SetDamageType(DAMAGE_TYPE_NONE);	
	return piEntity;
}

void CTurretType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eTurretState_Normal,0);
}

CTurret::CTurret(CTurretType *pType,unsigned int dwCurrentTime)
{
	m_bTargetLocked=false;
	m_sClassName="CTurret";
	m_pType=pType;
	m_dwNextShotTime=CHECKPOINT_HOLD_FIRE_TIME;
	m_dRadius=m_pType->DesignGetRadius();
	m_piContainerBuilding=NULL;
	m_nNextCheckContainerBuilding=0;
	m_bFirstTimeVisible=true;
	
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
}

void CTurret::OnRemoved(IEntity *piEntity)
{
	CEntityBase::OnRemoved(piEntity);
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}

void CTurret::OnKilled(IEntity *piEntity)
{
	CEntityBase::OnKilled(piEntity);
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}

void CTurret::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	
	if(m_pTypeBase->GetStateAnimations(eTurretState_Destroyed))
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		
		SetState(eTurretState_Destroyed);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}
void CTurret::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	//piRender->RenderBBox(m_PhysicInfo.vPosition,Origin,m_PhysicInfo.vMins,m_PhysicInfo.vMaxs,CVector(1,1,1),0x8888);
	CEntityBase::Render(piRender,piCamera);
}

bool CTurret::IsInsideBuilding(IStaticStructure *piStaticStructure)
{
	const std::vector<SBBox> &vBBoxes=piStaticStructure->GetProtectiveRegions();
	if(vBBoxes.size()==0){return false;}
	
	
	CVector vFake1,vFake2;
	CVector vForward,vRight,vUp;
	ComputeReferenceSystem(piStaticStructure->GetPhysicInfo()->vPosition,piStaticStructure->GetPhysicInfo()->vAngles,Origin,Origin,&vFake1,&vFake2,&vForward,&vUp,&vRight);
	
	CVector vMyCenter=m_PhysicInfo.vPosition;
	vMyCenter-=piStaticStructure->GetPhysicInfo()->vPosition;
	CMatrix m;
	m.Ref(vForward,vUp,vRight);
	vMyCenter*=m;
	
	bool bInside=false;
	for(unsigned int b=0;!bInside && b<vBBoxes.size();b++)
	{
		bInside=true;
		for(int c=0;c<3;c++)
		{
			if(vMyCenter.c[c]<vBBoxes[b].vMins.c[c]){bInside=false;break;}
			if(vMyCenter.c[c]>vBBoxes[b].vMaxs.c[c]){bInside=false;break;}
		}
	}
	return bInside;
}

void CTurret::FindBuilding(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CTurret *pThis=(CTurret *)pParam1;
	if(pThis->m_piContainerBuilding){return;}
	if(piEntity->IsRemoved()){return;}
	if(*piEntity->GetEntityClass()!="CStaticStructure"){return;}
	
	IStaticStructure *piStructure=dynamic_cast<IStaticStructure*>(piEntity);
	if(piStructure==NULL){return;}
	
	bool bInside=pThis->IsInsideBuilding(piStructure);
	if(bInside){pThis->m_piContainerBuilding=piStructure;}
}

void CTurret::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	m_dwNextProcessFrame=dwCurrentTime+10;
	
	if(GetState()==eTurretState_Destroyed){return;}
	if(GetState()==eTurretState_Normal)
	{
		size_t nAnimationToSet=0;
		double dMaxHealth=GetMaxHealth();
		unsigned int nAnimations=m_pTypeBase->GetStateAnimations(ENTITY_STATE_BASE);
		nAnimationToSet=(size_t)(((dMaxHealth-m_dHealth)/dMaxHealth)*((double)nAnimations));
		if(nAnimationToSet>nAnimations-1){nAnimationToSet=nAnimations-1;}
		SetState(ENTITY_STATE_BASE,(int)nAnimationToSet);
	}
	
	if(m_nNextCheckContainerBuilding<dwCurrentTime)
	{
		if(m_piContainerBuilding)
		{
			if(!IsInsideBuilding(m_piContainerBuilding))
			{
				UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
				m_piContainerBuilding=NULL;			
			}
		}
		else
		{
			m_piContainerBuilding=NULL;
			GetEntityManager()->PerformUnaryOperation(FindBuilding,this,0);
			if(m_piContainerBuilding){SUBSCRIBE_TO_CAST(m_piContainerBuilding,IEntityEvents);}
		}
		m_nNextCheckContainerBuilding=dwCurrentTime+100;
	}
	
	if(m_piContainerBuilding)
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
	}
	else
	{
		m_dwDamageType=m_nConfiguredDamageType;
	}
		
	if(m_piTarget && !m_piContainerBuilding)
	{
		CVector vTargetVel=m_piTarget->GetPhysicInfo()->vVelocity;
		CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
		
		bool bTargetDir=false;
		CVector vWorldDir;
		if(m_vWeapons.size())
		{
			// Average of my weapons
			for(unsigned int y=0;y<m_vWeapons.size();y++)
			{			
				vWorldDir+=m_vWeapons[y]->GetIdealHeadingToTarget(vTargetPos,vTargetVel);
				bTargetDir=true;
			}
		}
		else
		{
			// Average of the children weapons
			for(unsigned int x=0;x<m_vChildren.size();x++)
			{
				for(unsigned int y=0;y<m_vChildren[x].piEntity->GetWeapons();y++)
				{
					vWorldDir+=m_vChildren[x].piEntity->GetWeapon(y)->GetIdealHeadingToTarget(vTargetPos,vTargetVel);
					bTargetDir=true;
				}
			}
		}
		if(!bTargetDir){vWorldDir=m_PhysicInfo.vPosition-vTargetPos;}
		
		CMatrix m;
		m.Ref(m_PhysicInfo.vRefSysX,m_PhysicInfo.vRefSysY,m_PhysicInfo.vRefSysZ);
		vWorldDir*=m;
		CVector vLocalAngles=AnglesFromVector(vWorldDir);
		double dIdealYaw=vLocalAngles.c[YAW];
		m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],dIdealYaw,m_pType->m_dMaxAngularSpeed*dTimeFraction);
		if(m_pType->m_dMaxAngle>0)
		{
			if(dIdealYaw>180)
			{
				dIdealYaw=dIdealYaw-360;
				if(dIdealYaw<m_pType->m_dMinAngle){dIdealYaw=m_pType->m_dMinAngle;}
			}
			else
			{
				if(dIdealYaw>m_pType->m_dMaxAngle){dIdealYaw=m_pType->m_dMaxAngle;}
			}
			
			
			m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],dIdealYaw,m_pType->m_dMaxAngularSpeed*dTimeFraction);
		}
		double dYawDiff,dPitchDiff;
		dYawDiff=fabs(AngleDiff(vLocalAngles.c[YAW],m_PhysicInfo.vLocalAngles.c[YAW]));
		dPitchDiff=fabs(AngleDiff(vLocalAngles.c[PITCH]	,m_PhysicInfo.vLocalAngles.c[PITCH]));
		
		m_bTargetLocked=dYawDiff<m_pType->m_dTargetLockYawDiff && dPitchDiff<m_pType->m_dTargetLockPitchDiff;
	}
	else
	{
		m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],0,m_pType->m_dMaxAngularSpeed*dTimeFraction);
		m_bTargetLocked=false;
	}
	if(m_piTarget && m_bTargetLocked && dwCurrentTime>m_dwNextShotTime && m_vWeapons.size() && !m_piContainerBuilding)
	{
		bool bTargetTooClose=false;
		// Check minimun distance to target in the target's plane.
		if(m_pType->m_dTargetMinDistance!=0)
		{
			IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
			CVector vCameraPos=piCamera?piCamera->GetPosition():Origin;
			REL(piCamera);
			CVector vPosInTargetPlane;
			CPlane plane(AxisPosY,m_piTarget->GetPhysicInfo()->vPosition);
			if(plane.Cut(vCameraPos,m_PhysicInfo.vPosition,&vPosInTargetPlane))
			{
				bTargetTooClose=(m_piTarget->GetPhysicInfo()->vPosition-vPosInTargetPlane)<m_pType->m_dTargetMinDistance;
			}
		}
		
		if(!bTargetTooClose)
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
}
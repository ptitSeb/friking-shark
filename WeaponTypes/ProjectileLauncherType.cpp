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
#include "WeaponTypes.h"
#include "ProjectileLauncherType.h"
#include "../GameGraphics/GameGraphics.h"

CProjectileLauncherType::CProjectileLauncherType(void)
{
  m_bProjectileEntityTypesResolved=false;
  m_dwWeaponSlot=0;
  m_bIgnoreRoll=true;
  m_bUsesAmmo=false;
  m_nInitialAmmo=0;
  m_nAmmoPerRound=1;
  g_PlayAreaManagerWrapper.AddRef();
}

CProjectileLauncherType::~CProjectileLauncherType(void)
{
	g_PlayAreaManagerWrapper.Release();
}

IWeapon *CProjectileLauncherType::CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)
{
 CProjectileLauncher *pProyectileLauncher=new CProjectileLauncher(this,piEntity,dwCurrentTime);
  return pProyectileLauncher;
}

unsigned int CProjectileLauncherType::GetSlot()
{
  return m_dwWeaponSlot;
}

unsigned int CProjectileLauncherType::GetMaxLevel()
{
  if(m_dLevels.size()==0){return 0;}
  unsigned int dwMaxLevel=(unsigned int)m_dLevels.size();
  return dwMaxLevel-1;
}
bool CProjectileLauncherType::UsesAmmo(){return m_bUsesAmmo;}
unsigned int CProjectileLauncherType::GetInitialAmmo(){return m_nInitialAmmo;}
unsigned int CProjectileLauncherType::GetAmmoPerRound(){return m_nAmmoPerRound;}

SProjectileLauncherLevel *CProjectileLauncherType::GetLevel(unsigned int dwLevel)
{
  if(m_dLevels.size()==0){return NULL;}

  if(dwLevel>(m_dLevels.size()-1))
  {
    dwLevel=(unsigned int)m_dLevels.size()-1;
  }
  return &m_dLevels[dwLevel];
}

bool CProjectileLauncherType::PrepareResources()
{
	bool bOk=true;
	for(unsigned int nLevel=0;nLevel<m_dLevels.size();nLevel++)
	{
		SProjectileLauncherLevel *pLevel=&m_dLevels[nLevel];
		for(unsigned int nProjectile=0;nProjectile<pLevel->dProjectiles.size();nProjectile++)
		{
			SProjectileLauncherProjectile *pProjectileInfo=&pLevel->dProjectiles[nProjectile];
			if(pProjectileInfo->projectileEntityType.m_piEntityType)
			{
				bOk=bOk && pProjectileInfo->projectileEntityType.m_piEntityType->PrepareResources();
			}
		}
	}
	return bOk;
}
CProjectileLauncher::CProjectileLauncher(CProjectileLauncherType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase)
{
  m_pType=pType;
  m_piEntity=piEntity;
  m_dwLastFireTime=0;
  m_dwCurrentLevel=0;
  m_pCurrentLevel=m_pType->GetLevel(m_dwCurrentLevel);
  m_nAmmo=m_pType->GetInitialAmmo();  
}

CProjectileLauncher::~CProjectileLauncher()
{
}


bool         CProjectileLauncher::UsesAmmo(){return m_pType->UsesAmmo();}
void 		 CProjectileLauncher::SetAmmo(unsigned int nAmmo){m_nAmmo=nAmmo;}
void 		 CProjectileLauncher::AddAmmo(unsigned int nAmmo){m_nAmmo+=nAmmo;}
unsigned int CProjectileLauncher::GetAmmo(){return m_nAmmo;}

void CProjectileLauncher::Fire(unsigned int dwCurrentTime)
{
	bool bFire=IsReady(dwCurrentTime);
	if(bFire && m_pType->UsesAmmo())
	{
		unsigned int nAmmoPerRound=m_pType->GetAmmoPerRound();
		if(m_nAmmo<nAmmoPerRound)
		{
			bFire=false;
		}
		else
		{
			bFire=true;
			m_nAmmo-=nAmmoPerRound;
		}
	}
	if(bFire)
	{
		size_t nProjectile;
		SPhysicInfo *pEntityPhysicInfo=m_piEntity->GetPhysicInfo();
		CVector vOwnerAngles=pEntityPhysicInfo->vAngles,vOwnerForward,vOwnerRight,vOwnerUp;
		CVector vTargetAngles,vTargetForward,vTargetRight,vTargetUp;
		IEntity *piTarget=m_piEntity->GetTarget();
		CVector vLaunchPosition=m_piEntity->GetPhysicInfo()->vPosition;
		
		if(piTarget)
		{
			SPhysicInfo *pTargetPhysicInfo=piTarget->GetPhysicInfo();
			if(m_piEntity->GetAlignment()==ENTITY_ALIGNMENT_ENEMIES){vLaunchPosition=ProjectToAirPlane(m_piEntity->GetPhysicInfo()->vPosition);}
			CVector vTargetDirection=GetIdealHeadingToTarget(vLaunchPosition,pTargetPhysicInfo->vPosition,pTargetPhysicInfo->vVelocity);
			AnglesFromVector(vTargetDirection,&vTargetAngles);
		}
		
		if(m_pType->m_bIgnoreRoll){vTargetAngles.c[ROLL]=0;}
		if(m_pType->m_bIgnoreRoll){vOwnerAngles.c[ROLL]=0;}
		VectorsFromAngles(vOwnerAngles,&vOwnerForward,&vOwnerRight,&vOwnerUp);
		VectorsFromAngles(vTargetAngles,&vTargetForward,&vTargetRight,&vTargetUp);
		
		for(nProjectile=0;nProjectile<m_pCurrentLevel->dProjectiles.size();nProjectile++)
		{
			SProjectileLauncherProjectile *pProjectileInfo=&m_pCurrentLevel->dProjectiles[nProjectile];
			if(pProjectileInfo->projectileEntityType.m_piEntityType)
			{
			
				CVector vVelAngles,vVelForward,vVelRight,vVelUp;
				CVector vPosAngles,vPosForward,vPosRight,vPosUp;
				
				vVelAngles=vOwnerAngles;
				vVelForward=vOwnerForward;
				vVelRight=vOwnerRight;
				vVelUp=vOwnerUp;
				vPosAngles=vOwnerAngles;
				vPosForward=vOwnerForward;
				vPosRight=vOwnerRight;
				vPosUp=vOwnerUp;			
				
				if(pProjectileInfo->dwPositionReferenceSystem==eProjectileLauncherReferenceSystem_Target && piTarget)
				{
					vPosAngles=vTargetAngles;
					vPosForward=vTargetForward;
					vPosRight=vTargetRight;
					vPosUp=vTargetUp;
				}
				
				IEntity *piProjectile=pProjectileInfo->projectileEntityType.m_piEntityType->CreateInstance(m_piEntity,dwCurrentTime);
				if(piProjectile)
				{
					SPhysicInfo *pProjectilePhysicInfo=piProjectile->GetPhysicInfo();
					
					CVector vDirection;
					CVector vFinalHeading=pProjectileInfo->vHeading;
					vFinalHeading.c[0]+=pProjectileInfo->vHeadingJitter.c[0]*drand()-pProjectileInfo->vHeadingJitter.c[0]*0.5;
					vFinalHeading.c[1]+=pProjectileInfo->vHeadingJitter.c[1]*drand()-pProjectileInfo->vHeadingJitter.c[1]*0.5;
					vFinalHeading.c[2]+=pProjectileInfo->vHeadingJitter.c[2]*drand()-pProjectileInfo->vHeadingJitter.c[2]*0.5;
					
					VectorsFromAngles(vFinalHeading,&vDirection);
					
					pProjectilePhysicInfo->vPosition=pEntityPhysicInfo->vPosition;
					pProjectilePhysicInfo->vPosition+=vPosForward*pProjectileInfo->vOrigin.c[0];
					pProjectilePhysicInfo->vPosition+=vPosUp*pProjectileInfo->vOrigin.c[1];
					pProjectilePhysicInfo->vPosition+=vPosRight*pProjectileInfo->vOrigin.c[2];
					
					if(m_piEntity->GetAlignment()==ENTITY_ALIGNMENT_ENEMIES)
					{
						pProjectilePhysicInfo->vPosition=ProjectToAirPlane(pProjectilePhysicInfo->vPosition);
					}
					
					
					if(pProjectileInfo->dwVelocityReferenceSystem==eProjectileLauncherReferenceSystem_Target && piTarget)
					{
						CVector vTargetAngles,vTargetForward,vTargetRight,vTargetUp;
						if(piTarget)
						{
							SPhysicInfo *pTargetPhysicInfo=piTarget->GetPhysicInfo();
							CVector vDirection=GetIdealHeadingToTarget(pProjectilePhysicInfo->vPosition,pTargetPhysicInfo->vPosition,pTargetPhysicInfo->vVelocity);
							AnglesFromVector(vDirection,&vVelAngles);
						}						
						if(m_pType->m_bIgnoreRoll){vVelAngles.c[ROLL]=0;}
						VectorsFromAngles(vVelAngles,&vVelForward,&vVelRight,&vVelUp);
					}					
					
					pProjectilePhysicInfo->vVelocity+=vVelForward*vDirection.c[0]*pProjectileInfo->dVelocity;
					pProjectilePhysicInfo->vVelocity+=vVelUp*vDirection.c[1]*pProjectileInfo->dVelocity;
					pProjectilePhysicInfo->vVelocity+=vVelRight*vDirection.c[2]*pProjectileInfo->dVelocity;
					pProjectilePhysicInfo->vAngleVelocity=pProjectileInfo->vAngularVelocity;
					pProjectilePhysicInfo->dMaxVelocity=pProjectileInfo->dVelocity;
					
					AnglesFromVector(pProjectilePhysicInfo->vVelocity,&pProjectilePhysicInfo->vAngles);
				}
			}
		}
		m_dwLastFireTime=dwCurrentTime;
	}
}

CVector CProjectileLauncher::ProjectToAirPlane(CVector vPos)
{
	CVector vPlayerStart,vPlayerEnd;
	g_PlayAreaManagerWrapper.m_piInterface->GetPlayerRoute(&vPlayerStart,&vPlayerEnd);
	IGenericCamera *piCamera=g_PlayAreaManagerWrapper.m_piInterface->GetCamera();
	CVector vCameraPos=piCamera?piCamera->GetPosition():Origin;

	CVector cut;
	CPlane plane(AxisPosY,vPlayerStart.c[1]);
	if(plane.Cut(vPos,vCameraPos,&cut))
	{
		vPos=cut;
	}
	return vPos;
}

bool CProjectileLauncher::IsReady(unsigned int dwCurrentTime)
{
  if(m_pCurrentLevel)
  {
    return ((m_dwLastFireTime+m_pCurrentLevel->dwReloadTime)<=dwCurrentTime);
  }
  else
  {
    return false;
  }
}

unsigned int CProjectileLauncher::GetSlot()
{
  return m_pType->GetSlot();
}

unsigned int CProjectileLauncher::GetCurrentLevel()
{
  return m_dwCurrentLevel;
}

void CProjectileLauncher::SetCurrentLevel(unsigned int dwLevel)
{
  m_dwCurrentLevel=dwLevel;
  if(m_dwCurrentLevel>m_pType->GetMaxLevel())
  {
    m_dwCurrentLevel=m_pType->GetMaxLevel();
  }
  m_pCurrentLevel=m_pType->GetLevel(m_dwCurrentLevel);
}

CVector CProjectileLauncher::GetIdealHeadingToTarget(CVector vLaunchPosition,CVector vTargetPosition,CVector vTargetVelocity)
{
	double dFastestProjectile=0;
	if(!m_pCurrentLevel){return vTargetPosition-vLaunchPosition;}
	
	for(unsigned int nProjectile=0;nProjectile<m_pCurrentLevel->dProjectiles.size();nProjectile++)
	{
		SProjectileLauncherProjectile *pProjectileInfo=&m_pCurrentLevel->dProjectiles[nProjectile];
		if(pProjectileInfo->dVelocity>dFastestProjectile){dFastestProjectile=pProjectileInfo->dVelocity;}
	}
	return PredictInterceptionPosition(vLaunchPosition,dFastestProjectile,vTargetPosition,vTargetVelocity)-vLaunchPosition;
}

CVector CProjectileLauncher::GetIdealHeadingToTarget(CVector vTargetPosition,CVector vTargetVelocity)
{
	CVector vPosition;
	if(m_piEntity){vPosition=m_piEntity->GetPhysicInfo()->vPosition;}
	return GetIdealHeadingToTarget(vPosition,vTargetPosition,vTargetVelocity);
}


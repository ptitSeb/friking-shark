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

CProjectileLauncherType::CProjectileLauncherType(void)
{
  m_bProjectileEntityTypesResolved=false;
  m_dwWeaponSlot=0;
  m_bIgnoreRoll=true;
  m_bUsesAmmo=false;
  m_nInitialAmmo=0;
  m_nAmmoPerRound=1;
}

CProjectileLauncherType::~CProjectileLauncherType(void)
{
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
	

		if(piTarget)
		{
		SPhysicInfo *pTargetPhysicInfo=piTarget->GetPhysicInfo();
		CVector vTargetDirection=GetIdealHeadingToTarget(pTargetPhysicInfo->vPosition,pTargetPhysicInfo->vVelocity);
		AnglesFromVector(vTargetDirection,&vTargetAngles);
		}

		if(m_pType->m_bIgnoreRoll)
		{
		vOwnerAngles.c[ROLL]=0;
		vTargetAngles.c[ROLL]=0;
		}

		VectorsFromAngles(vOwnerAngles,&vOwnerForward,&vOwnerRight,&vOwnerUp);
		VectorsFromAngles(vTargetAngles,&vTargetForward,&vTargetRight,&vTargetUp);

		for(nProjectile=0;nProjectile<m_pCurrentLevel->dProjectiles.size();nProjectile++)
		{
		SProjectileLauncherProjectile *pProjectileInfo=&m_pCurrentLevel->dProjectiles[nProjectile];
		if(pProjectileInfo->projectileEntityType.m_piEntityType)
		{
			CVector vAngles,vForward,vRight,vUp;

			if(pProjectileInfo->dwReferenceSystem==eProjectileLauncherReferenceSystem_Owner)
			{
			vAngles=vOwnerAngles;
			vForward=vOwnerForward;
			vRight=vOwnerRight;
			vUp=vOwnerUp;
			}
			else if(pProjectileInfo->dwReferenceSystem==eProjectileLauncherReferenceSystem_Target)
			{
			if(piTarget==NULL){continue;}
			vAngles=vTargetAngles;
			vForward=vTargetForward;
			vRight=vTargetRight;
			vUp=vTargetUp;
			}
			else
			{
			continue;
			}

			IEntity *piProjectile=pProjectileInfo->projectileEntityType.m_piEntityType->CreateInstance(m_piEntity,dwCurrentTime);
			if(piProjectile)
			{
			SPhysicInfo *pProjectilePhysicInfo=piProjectile->GetPhysicInfo();
			pProjectilePhysicInfo->vPosition=pEntityPhysicInfo->vPosition;
			pProjectilePhysicInfo->vPosition+=vForward*pProjectileInfo->vOrigin.c[0];
			pProjectilePhysicInfo->vPosition+=vUp*pProjectileInfo->vOrigin.c[1];
			pProjectilePhysicInfo->vPosition+=vRight*pProjectileInfo->vOrigin.c[2];
			pProjectilePhysicInfo->vVelocity+=vForward*pProjectileInfo->vDirection.c[0]*pProjectileInfo->dVelocity;
			pProjectilePhysicInfo->vVelocity+=vUp*pProjectileInfo->vDirection.c[1]*pProjectileInfo->dVelocity;
			pProjectilePhysicInfo->vVelocity+=vRight*pProjectileInfo->vDirection.c[2]*pProjectileInfo->dVelocity;
			pProjectilePhysicInfo->vAngleVelocity=pProjectileInfo->vAngularVelocity;
			pProjectilePhysicInfo->dMaxVelocity=pProjectileInfo->dVelocity;
			AnglesFromVector(pProjectilePhysicInfo->vVelocity,&pProjectilePhysicInfo->vAngles);
			}
		}
	}
    m_dwLastFireTime=dwCurrentTime;
  }
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

CVector CProjectileLauncher::GetIdealHeadingToTarget(CVector vTargetPosition,CVector vTargetVelocity)
{
	CVector vPosition;
	double dFastestProjectile=0;
	if(m_piEntity){vPosition=m_piEntity->GetPhysicInfo()->vPosition;}
	if(!m_pCurrentLevel){return vTargetPosition-vPosition;}
	
	for(unsigned int nProjectile=0;nProjectile<m_pCurrentLevel->dProjectiles.size();nProjectile++)
	{
		SProjectileLauncherProjectile *pProjectileInfo=&m_pCurrentLevel->dProjectiles[nProjectile];
		if(pProjectileInfo->dVelocity>dFastestProjectile){dFastestProjectile=pProjectileInfo->dVelocity;}
	}
	return PredictInterceptionPosition(vPosition,dFastestProjectile,vTargetPosition,vTargetVelocity)-vPosition;
}






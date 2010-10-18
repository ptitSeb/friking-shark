#include "StdAfx.h"
#include "WeaponTypes.h"
#include ".\projectilelaunchertype.h"

CProjectileLauncherType::CProjectileLauncherType(void)
{
  m_bProjectileEntityTypesResolved=false;
  m_dwWeaponSlot=0;
  m_bIgnoreRoll=true;
}

CProjectileLauncherType::~CProjectileLauncherType(void)
{
  size_t nLevel=0,nProjectile=0;
  for(nLevel=0;nLevel<m_dLevels.size();nLevel++)
  {
    SProjectileLauncherLevel *pLevel=&m_dLevels[nLevel];
    for(nProjectile=0;nProjectile<pLevel->dProjectiles.size();nProjectile++)
    {
      REL(pLevel->dProjectiles[nProjectile].piProjectileEntityType);
    }
  }
}

IWeapon *CProjectileLauncherType::CreateInstance(IEntity *piEntity,DWORD dwCurrentTime)
{
  size_t nLevel=0,nProjectile=0;

  // Cuando se crea la instancia del primer objeto se intentan resolver los tipos de entidades
  // de los proyectiles para que la instanciacion despues sea mas rapida.

  if(!m_bProjectileEntityTypesResolved)
  {
    ISystemManager  *piSystemManager=GetSystemManager();
    ISystem         *piSystem=NULL;
    if(piSystemManager){piSystem=piSystemManager->GetSystem("WeaponTypes");}
    if(piSystem)
    {
      for(nLevel=0;nLevel<m_dLevels.size();nLevel++)
      {
        SProjectileLauncherLevel *pLevel=&m_dLevels[nLevel];
        for(nProjectile=0;nProjectile<pLevel->dProjectiles.size();nProjectile++)
        {
          ISystemObject   *piObject=NULL;
          if(piSystem){piSystem->GetObject(pLevel->dProjectiles[nProjectile].sProjectileEntityType,&piObject);}
          if(piObject){pLevel->dProjectiles[nProjectile].piProjectileEntityType=QI(IEntityType,piObject);}
          REL(piObject);
        }
      }
      m_bProjectileEntityTypesResolved=true;
    }
    REL(piSystemManager);
    REL(piSystem);
  }

  CProjectileLauncher *pProyectileLauncher=new CProjectileLauncher(this,piEntity,dwCurrentTime);
  return pProyectileLauncher;
}

DWORD CProjectileLauncherType::GetSlot()
{
  return m_dwWeaponSlot;
}

DWORD CProjectileLauncherType::GetMaxLevel()
{
  if(m_dLevels.size()==0){return 0;}
  DWORD dwMaxLevel=(DWORD)m_dLevels.size();
  return dwMaxLevel-1;
}

SProjectileLauncherLevel *CProjectileLauncherType::GetLevel(DWORD dwLevel)
{
  if(m_dLevels.size()==0){return NULL;}

  if(dwLevel>(m_dLevels.size()-1))
  {
    dwLevel=(DWORD)m_dLevels.size()-1;
  }
  return &m_dLevels[dwLevel];
}

CProjectileLauncher::CProjectileLauncher(CProjectileLauncherType *pType,IEntity *piEntity,DWORD dwCurrentTimeBase)
{
  m_pType=pType;
  m_piEntity=piEntity;
  m_dwLastFireTime=0;
  m_dwCurrentLevel=0;
  m_pCurrentLevel=m_pType->GetLevel(m_dwCurrentLevel);
}

CProjectileLauncher::~CProjectileLauncher()
{
}

void CProjectileLauncher::Fire(DWORD dwCurrentTime)
{
  if(IsReady(dwCurrentTime))
  {
    size_t nProjectile;
    SPhysicInfo *pEntityPhysicInfo=m_piEntity->GetPhysicInfo();
    CVector vOwnerAngles=pEntityPhysicInfo->vAngles,vOwnerForward,vOwnerRight,vOwnerUp;
    CVector vTargetAngles,vTargetForward,vTargetRight,vTargetUp;
    IEntity *piTarget=m_piEntity->GetTarget();
  

    if(piTarget)
    {
      SPhysicInfo *pTargetPhysicInfo=piTarget->GetPhysicInfo();
      AnglesFromVector(pTargetPhysicInfo->vPosition-pEntityPhysicInfo->vPosition,&vTargetAngles);
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
      if(pProjectileInfo->piProjectileEntityType)
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

        IEntity *piProjectile=pProjectileInfo->piProjectileEntityType->CreateInstance(m_piEntity,dwCurrentTime);
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
          AnglesFromVector(pProjectilePhysicInfo->vVelocity,&pProjectilePhysicInfo->vAngles);
        }
      }
    }
    m_dwLastFireTime=dwCurrentTime;
  }
}

bool CProjectileLauncher::IsReady(DWORD dwCurrentTime)
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

DWORD CProjectileLauncher::GetSlot()
{
  return m_pType->GetSlot();
}

DWORD CProjectileLauncher::GetCurrentLevel()
{
  return m_dwCurrentLevel;
}

void CProjectileLauncher::SetCurrentLevel(DWORD dwLevel)
{
  m_dwCurrentLevel=dwLevel;
  if(m_dwCurrentLevel>m_pType->GetMaxLevel())
  {
    m_dwCurrentLevel=m_pType->GetMaxLevel();
  }
  m_pCurrentLevel=m_pType->GetLevel(m_dwCurrentLevel);
}
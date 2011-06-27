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


#pragma once

#define eProjectileLauncherReferenceSystem_Unknown    0
#define eProjectileLauncherReferenceSystem_Owner      1
#define eProjectileLauncherReferenceSystem_Target     2

struct SProjectileLauncherProjectile
{
  CVector     vOrigin;
  CVector     vHeading;
  CVector     vHeadingJitter;
  CVector     vAngularVelocity;
  double      dVelocity;
  CEntityTypeWrapper projectileEntityType;
  
  unsigned int       dwPositionReferenceSystem;
  unsigned int       dwVelocityReferenceSystem;
  
  
  SProjectileLauncherProjectile()
  {
	 dwPositionReferenceSystem=eProjectileLauncherReferenceSystem_Owner;
	 dwVelocityReferenceSystem=eProjectileLauncherReferenceSystem_Owner;
	 dVelocity=1;
  }
};

BEGIN_STRUCT_PROPS(SProjectileLauncherProjectile)
  PROP(projectileEntityType,"Entity")
  PROP_VALUE_FLAGS(vOrigin,"Position",Origin,MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(vHeading,"Heading",CVector(1,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(vHeadingJitter,"HeadingJitter",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(dVelocity,"Velocity",1,MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(vAngularVelocity,"AngularVelocity",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(dwPositionReferenceSystem,"PositionReferenceSystem",eProjectileLauncherReferenceSystem_Owner,MRPF_NORMAL|MRPF_OPTIONAL)
  PROP_VALUE_FLAGS(dwVelocityReferenceSystem,"VelocityReferenceSystem",eProjectileLauncherReferenceSystem_Owner,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

struct SProjectileLauncherLevel
{
  double                                dRange;
  unsigned int                          dwReloadTime;
  deque<SProjectileLauncherProjectile>  dProjectiles;

  SProjectileLauncherLevel(){dRange=0;dwReloadTime=1000;}
};

BEGIN_STRUCT_PROPS(SProjectileLauncherLevel)
  PROP(dwReloadTime,"ReloadTime")
  PROP(dProjectiles,"Projectiles")
  PROP_VALUE_FLAGS(dRange,"Range",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CProjectileLauncherType: virtual public CSystemObjectBase,virtual public IWeaponType
{
  bool m_bProjectileEntityTypesResolved;

  unsigned int                    m_dwWeaponSlot;
  deque<SProjectileLauncherLevel> m_dLevels;
  
  bool               m_bUsesAmmo;
  unsigned int       m_nInitialAmmo;
  unsigned int       m_nAmmoPerRound;
  
public:

  bool m_bIgnoreRoll;

  IWeapon *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime);
  
  bool    PrepareResources();
  
  unsigned int                     GetSlot();
  unsigned int                     GetMaxLevel();
  bool                             UsesAmmo();
  unsigned int                     GetInitialAmmo();
  unsigned int                     GetAmmoPerRound();
  
  SProjectileLauncherLevel *GetLevel(unsigned int dwLevel);
 
  BEGIN_PROP_MAP(CProjectileLauncherType)
    PROP_FLAGS(m_dLevels,"Levels",MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwWeaponSlot,"Slot",0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_bIgnoreRoll,"IgnoreRoll",true,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_bUsesAmmo,"UsesAmmo",false,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nInitialAmmo,"InitialAmmo",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nAmmoPerRound,"AmmoPerRound",1,MRPF_NORMAL|MRPF_OPTIONAL)
  END_PROP_MAP()

  CProjectileLauncherType();
  ~CProjectileLauncherType();
};

class CProjectileLauncher:public IWeapon
{
  CProjectileLauncherType  *m_pType;
  SProjectileLauncherLevel *m_pCurrentLevel;
  IEntity                  *m_piEntity;
  unsigned int              m_dwLastFireTime;
  unsigned int              m_dwCurrentLevel;
  unsigned int              m_nAmmo;
  
  CVector ProjectToAirPlane(CVector vPos);
    
public:
  
  // IWeapon

  unsigned int GetSlot();
  
  bool         UsesAmmo();
  void 		   SetAmmo(unsigned int nAmmo);
  void 		   AddAmmo(unsigned int nAmmo);
  unsigned int GetAmmo();
  
  void Fire(unsigned int dwCurrentTime);
  bool IsReady(unsigned int dwCurrentTime);

  unsigned int GetCurrentLevel();
  void  SetCurrentLevel(unsigned int dwLevel);
  
  CVector  GetIdealHeadingToTarget(CVector vTargetPosition,CVector vTargetVelocity);
  CVector  GetIdealHeadingToTarget(CVector vLaunchPosition,CVector vTargetPosition,CVector vTargetVelocity);
  
  CProjectileLauncher(CProjectileLauncherType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase);
  ~CProjectileLauncher();
};

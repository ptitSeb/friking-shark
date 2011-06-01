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

enum EWeaponUpgradeBonusState
{
	eWeaponUpgradeBonusState_Normal=ENTITY_STATE_BASE,
	eWeaponUpgradeBonusState_Taken
};

class CWeaponUpgradeBonusType: public CEntityTypeBase
{
public:
	CPlayAreaManagerWrapper m_PlayAreaManager;
	
  unsigned int m_dwLevels;
  unsigned int m_dwSlot;
  double m_dAngularVelocity;
  double m_dForwardVelocity;
  double m_dExitVelocity;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CWeaponUpgradeBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
    PROP(m_dwLevels,"Levels");
    PROP(m_dwSlot,"Slot");
	PROP_VALUE_FLAGS(m_dAngularVelocity,"AngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dForwardVelocity,"ForwardVelocity",20,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dExitVelocity,"ExitVelocity",5,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eWeaponUpgradeBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
  CWeaponUpgradeBonusType();
  ~CWeaponUpgradeBonusType();
};


class CWeaponUpgradeBonus: public CEntityBase
{
  CWeaponUpgradeBonusType  *m_pType;

  CVector m_vCurrentForwardDirection;
  double  m_dCurrentAngularVelocity;
 
public:
 
  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
  void SetInitialVelocity();
  
  CWeaponUpgradeBonus(CWeaponUpgradeBonusType *pType);
};
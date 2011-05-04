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

enum EBombState
{
	eBombState_Normal=ENTITY_STATE_BASE,
	eBombState_Hit
};

class CBombProjectileType: public CEntityTypeBase
{
public:

	unsigned int m_nTimeToExplode;
	double m_dDamagePerSecond;
	double m_dDamageStartRadius;
	double m_dDamageEndRadius;
	unsigned int m_nDamageStartTime;
	unsigned int m_nDamageEndTime;
	
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBombProjectileType)
	PROP_CLASS_CHAIN(CEntityTypeBase)
	PROP_VALUE_FLAGS(m_nTimeToExplode,"TimeToExplode",400,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_dDamagePerSecond,"DamagePerSecond",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_dDamageStartRadius,"DamageStartRadius",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_dDamageEndRadius,"DamageEndRadius",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nDamageStartTime,"DamageStartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nDamageEndTime,"DamageEndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBombState_Hit,"Hit")
  END_ENTITY_STATE_MAP()
  
  CBombProjectileType();
  ~CBombProjectileType();
};


class CBombProjectile: public CEntityBase
{
  CBombProjectileType  *m_pType;
  IEntity *m_piParent;
  unsigned int m_nCurrentTime;
public:
	
  static void ApplyDamageOperation(IEntity *piEntity,void *pParam1,void *pParam2);
  
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

  CBombProjectile(CBombProjectileType *pType,IEntity *piParent);
  ~CBombProjectile();
};



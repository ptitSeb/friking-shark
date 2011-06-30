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

enum EBulletState
{
	eBulletState_Normal=ENTITY_STATE_BASE,
	eBulletState_Hit
};

class CBulletProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;
  unsigned int  m_dwDuration;
  
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBulletProjectileType)
    PROP_VALUE_FLAGS(m_dDamage,"Damage",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwDuration,"Duration",1000,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBulletState_Hit,"Hit")
  END_ENTITY_STATE_MAP()
  
  CBulletProjectileType();
  ~CBulletProjectileType();
};


class CBulletProjectile: public CEntityBase
{
  CBulletProjectileType  *m_pType;
  IEntity *m_piParent;
  unsigned int  m_dwNextCollisionCheck;
  double m_dRadius;
  
  void Render(IGenericRender *piRender,IGenericCamera *piCamera);
  
public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

  static void CheckCollisions(IEntity *piEntity,void *pParam1,void *pParam2);

  CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent);
  ~CBulletProjectile();
};
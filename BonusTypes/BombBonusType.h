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

enum EBombBonusState
{
	eBombBonusState_Normal=ENTITY_STATE_BASE,
	eBombBonusState_Taken
};

class CBombBonusType: public CEntityTypeBase
{
public:
	CPlayAreaManagerWrapper m_PlayAreaManager;
	
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBombBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
  CBombBonusType();
  ~CBombBonusType();
};


class CBombBonus: public CEntityBase
{
  CBombBonusType  *m_pType;
 
  CVector 		   m_vOriginalPosition;
  CPlane 		   m_PlayAreaPlane;
  IGenericCamera  *m_piCamera;
  IEntity		  *m_piOwner;
  double		   m_dRadius;
  
public:
 
  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void OnRemoved(IEntity *piEntity);
  
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
  
  CBombBonus(CBombBonusType *pType,IEntity *piOwner);
  ~CBombBonus();
};